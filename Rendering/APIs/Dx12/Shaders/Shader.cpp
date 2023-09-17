#include "stdafx.h"
#include "Shader.h"

#include "ShaderStage.h"
#include "ShaderUtils.h"
#include "../Context.h"
#include "APIs/Dx12/Submesh.h"
#include "Assets/Material.h"

namespace Rendering::Dx12
{
	void Shader::Bind(const ShaderParams shaderParams)
	{
		if (!IsCompiled(shaderParams))
		{
			Compile(shaderParams);
		}

		ID3D12GraphicsCommandList* commandList = Dx12Context::Get().GetCommandList();

		commandList->SetPipelineState(m_pipelineStates[shaderParams].Get());
		commandList->SetGraphicsRootSignature(m_rootSignature.Get());
	}

	ID3D12PipelineState* Shader::GetPipelineState(const ShaderParams shaderParams) const
	{
		return m_pipelineStates[shaderParams].Get();
	}

	bool Shader::IsCompiled(const ShaderParams shaderParams) const
	{
		if (m_pipelineStates.find(shaderParams) == m_pipelineStates.end())
		{
			return false;
		}
		
		struct stat fileStat;
		stat(m_path.c_str(), &fileStat);

		return m_lastCompileTime >= fileStat.st_mtime;
	}

	void Shader::Compile(const ShaderParams shaderParams)
	{
		std::unique_ptr<ShaderStage>* stages[] = { &m_vs, &m_ps, &m_ds, &m_hs, &m_gs };
		for (uint32_t i = 0; i < std::size(stages); i++)
		{
			std::unique_ptr<ShaderStage>& shader = *stages[i];

			if (shader == nullptr)
			{
				shader = std::make_unique<ShaderStage>((ShaderStage::Type) i);
			}
			shader->Compile(m_path);
			if (shader->IsCompiled())
			{
				shader->CreateReflectionData();
			}
		}

		m_rootSignature = m_vs->CreateRootSignature();
		SetDebugName(m_rootSignature, m_path.data());

		CreatePipelineState(shaderParams);

		PopulateShaderDescriptor();

		struct stat fileStat;
		stat(m_path.c_str(), &fileStat);

		m_lastCompileTime = fileStat.st_mtime;
	}

	void Shader::PopulateShaderDescriptor()
	{
		m_shaderDescriptor.Clear();

		ShaderStage* stages[] = { &*m_vs, &*m_ps, &*m_ds, &*m_hs, &*m_gs };
		for (const ShaderStage* shader : stages)
		{
			UpdateDescriptor(shader, m_shaderDescriptor);
		}
	}

	D3D12_RASTERIZER_DESC Shader::GetRasterizerDescription(const ShaderParams shaderParams)
	{
		CD3DX12_RASTERIZER_DESC rasterizerDesc = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

		if (shaderParams.m_isWireframe)
		{
			rasterizerDesc.FillMode = D3D12_FILL_MODE_WIREFRAME;
		}

		if (shaderParams.m_isDoubleSided)
		{
			rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
		}

		return rasterizerDesc;
	}

	D3D12_BLEND_DESC Shader::GetBlendDescription(const ShaderParams shaderParams)
	{
		if (shaderParams.m_blendingEnabled)
		{
			D3D12_BLEND_DESC blendDesc;
			blendDesc.AlphaToCoverageEnable  = false;
			blendDesc.IndependentBlendEnable = false;

			constexpr D3D12_RENDER_TARGET_BLEND_DESC renderTargetBlendDesc =
			{
				true,
				false,
				D3D12_BLEND_SRC_ALPHA,
				D3D12_BLEND_INV_SRC_ALPHA,
				D3D12_BLEND_OP_ADD,
				D3D12_BLEND_ONE,
				D3D12_BLEND_ZERO,
				D3D12_BLEND_OP_ADD,
				D3D12_LOGIC_OP_NOOP,
				D3D12_COLOR_WRITE_ENABLE_ALL,
			};


			for (uint32_t i               = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
				blendDesc.RenderTarget[i] = renderTargetBlendDesc;

			return blendDesc;
		}

		return CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	}

	D3D12_DEPTH_STENCIL_DESC Shader::GetDepthStencilDescription(const ShaderParams shaderParams)
	{
		CD3DX12_DEPTH_STENCIL_DESC depthStencilDesc(D3D12_DEFAULT);

		if (!shaderParams.m_depthTestEnabled)
		{
			depthStencilDesc.DepthEnable = false;
			depthStencilDesc.DepthFunc   = D3D12_COMPARISON_FUNC_ALWAYS;
		}
		else if (shaderParams.m_blendingEnabled)
		{
			depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		}

		return depthStencilDesc;
	}

	void Shader::CreatePipelineState(const ShaderParams shaderParams)
	{
		std::vector<D3D12_INPUT_ELEMENT_DESC> inputElements = GetInputElements();
		const std::vector<DXGI_FORMAT> outputFormats        = GetOutputFormats();

		m_inputSlotBindings.clear();
		for (D3D12_INPUT_ELEMENT_DESC& inputElement : inputElements)
		{
			const MeshAttribute attribute = GetMeshAttributeFromName(inputElement.SemanticName,
				inputElement.SemanticIndex);
			if (attribute != MeshAttribute::UNKNOWN)
			{
				m_inputSlotBindings[inputElement.InputSlot] = attribute;
			}
		}

		D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDesc = {
			m_rootSignature.Get(),
			{
				m_vs->GetByteCode()->GetBufferPointer(),
				m_vs->GetByteCode()->GetBufferSize()
			},
			{
				m_ps->GetByteCode()->GetBufferPointer(),
				m_ps->GetByteCode()->GetBufferSize()
			},
			{
				m_ds->GetByteCode().Get() != nullptr ? m_ds->GetByteCode()->GetBufferPointer() : nullptr,
				m_ds->GetByteCode().Get() != nullptr ? m_ds->GetByteCode()->GetBufferSize() : 0
			},
			{
				m_hs->GetByteCode().Get() != nullptr ? m_hs->GetByteCode()->GetBufferPointer() : nullptr,
				m_hs->GetByteCode().Get() != nullptr ? m_hs->GetByteCode()->GetBufferSize() : 0
			},
			{
				m_gs->GetByteCode().Get() != nullptr ? m_gs->GetByteCode()->GetBufferPointer() : nullptr,
				m_gs->GetByteCode().Get() != nullptr ? m_gs->GetByteCode()->GetBufferSize() : 0
			},
			{},
			GetBlendDescription(shaderParams),
			UINT_MAX,
			GetRasterizerDescription(shaderParams),
			GetDepthStencilDescription(shaderParams),
			{
				inputElements.data(),
				(uint32_t) inputElements.size()
			},
			D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED,
			D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
			(uint32_t) outputFormats.size(),
			{
			},
			DXGI_FORMAT_D24_UNORM_S8_UINT,
			{
				1,
				Dx12Context::Get().GetMsNumQualityLevels() - 1
			},
			0,
			{},
			D3D12_PIPELINE_STATE_FLAG_NONE
		};

		for (uint32_t i = 0; i < outputFormats.size(); i++)
		{
			pipelineStateDesc.RTVFormats[i] = outputFormats[i];
		}

		ComPtr<ID3D12PipelineState>& pipelineState = m_pipelineStates[shaderParams];

		Dx12Context::Get().GetDevice()->CreateGraphicsPipelineState(&pipelineStateDesc, IID_PPV_ARGS(&pipelineState));

		// ReSharper disable once CppUseStructuredBinding
		for (const auto& inputElement : inputElements)
		{
			delete[] inputElement.SemanticName;
		}

		SetDebugName(pipelineState, m_path.c_str());
	}

	std::vector<D3D12_INPUT_ELEMENT_DESC> Shader::GetInputElements() const
	{
		std::vector<D3D12_INPUT_ELEMENT_DESC> inputElements(m_vs->GetDescriptor().InputParameters);

		for (uint32_t i = 0; i < m_vs->GetDescriptor().InputParameters; ++i)
		{
			D3D12_SIGNATURE_PARAMETER_DESC signatureParameter;
			m_vs->GetReflector()->GetInputParameterDesc(i, &signatureParameter);

			inputElements[i].SemanticName         = _strdup(signatureParameter.SemanticName);
			inputElements[i].SemanticIndex        = signatureParameter.SemanticIndex;
			inputElements[i].Format               = GetLayoutElement(signatureParameter.Mask, signatureParameter.ComponentType);
			inputElements[i].InputSlot            = i;
			inputElements[i].AlignedByteOffset    = 0;
			inputElements[i].InputSlotClass       = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
			inputElements[i].InstanceDataStepRate = 0;
		}

		return inputElements;
	}

	std::vector<DXGI_FORMAT> Shader::GetOutputFormats() const
	{
		std::vector<DXGI_FORMAT> outputElements(m_ps->GetDescriptor().OutputParameters);

		for (uint32_t i = 0; i < m_ps->GetDescriptor().OutputParameters; ++i)
		{
			D3D12_SIGNATURE_PARAMETER_DESC signatureParameter;
			m_ps->GetReflector()->GetOutputParameterDesc(i, &signatureParameter);

			outputElements[i] = DXGI_FORMAT_R8G8B8A8_UNORM;
		}

		return outputElements;
	}

	DXGI_FORMAT Shader::GetLayoutElement(const uint8_t mask, const D3D_REGISTER_COMPONENT_TYPE componentType)
	{
		if (mask == 1)
		{
			if (componentType == D3D_REGISTER_COMPONENT_UINT32)
				return DXGI_FORMAT_R32_UINT;
			if (componentType == D3D_REGISTER_COMPONENT_SINT32)
				return DXGI_FORMAT_R32_SINT;
			if (componentType == D3D_REGISTER_COMPONENT_FLOAT32)
				return DXGI_FORMAT_R32_FLOAT;
		}
		else if (mask <= 3)
		{
			if (componentType == D3D_REGISTER_COMPONENT_UINT32)
				return DXGI_FORMAT_R32G32_UINT;
			if (componentType == D3D_REGISTER_COMPONENT_SINT32)
				return DXGI_FORMAT_R32G32_SINT;
			if (componentType == D3D_REGISTER_COMPONENT_FLOAT32)
				return DXGI_FORMAT_R32G32_FLOAT;
		}
		else if (mask <= 7)
		{
			if (componentType == D3D_REGISTER_COMPONENT_UINT32)
				return DXGI_FORMAT_R32G32B32_UINT;
			if (componentType == D3D_REGISTER_COMPONENT_SINT32)
				return DXGI_FORMAT_R32G32B32_SINT;
			if (componentType == D3D_REGISTER_COMPONENT_FLOAT32)
				return DXGI_FORMAT_R32G32B32_FLOAT;
		}
		else if (mask <= 15)
		{
			if (componentType == D3D_REGISTER_COMPONENT_UINT32)
				return DXGI_FORMAT_R32G32B32A32_UINT;
			if (componentType == D3D_REGISTER_COMPONENT_SINT32)
				return DXGI_FORMAT_R32G32B32A32_SINT;
			if (componentType == D3D_REGISTER_COMPONENT_FLOAT32)
				return DXGI_FORMAT_R32G32B32A32_FLOAT;
		}
		return DXGI_FORMAT_UNKNOWN;
	}

	NativeShader* Shader::Create(const std::shared_ptr<::Shader>& shader)
	{
		return static_cast<NativeShader*>(new Shader(shader->GetPath()));
	}

	Shader::~Shader()
	{
		RELEASE_DX(m_rootSignature);
	}
}

namespace Rendering
{
	extern void Deleter(NativeShader* shader)
	{
		if (shader != nullptr)
		{
			delete shader;
		}
	}
}
