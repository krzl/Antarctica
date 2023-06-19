#include "stdafx.h"
#include "Shader.h"

#include "ShaderStage.h"
#include "ShaderUtils.h"
#include "../Context.h"
#include "APIs/Dx12/Submesh.h"
#include "APIs/Dx12/Texture.h"
#include "Assets/Material.h"

namespace Renderer::Dx12
{
	Shader::~Shader()
	{
		RELEASE_DX(m_rootSignature);
	}

	void Shader::Bind()
	{
		if (!IsCompiled())
		{
			Compile();
		}

		ID3D12GraphicsCommandList* commandList = Dx12Context::Get().GetCommandList();

		commandList->SetPipelineState(m_pipelineState.Get());
		commandList->SetGraphicsRootSignature(m_rootSignature.Get());
	}

	void Shader::BindMaterial(const Material& materialData) const
	{
		for (const auto& [name, id] : m_shaderDescriptor.GetTextures())
		{
			auto it = materialData.GetTextures().find(name);
			if (it != materialData.GetTextures().end())
			{
				std::shared_ptr<::Texture> texture = it->second;
				if (texture->GetNativeObject() == nullptr)
				{
					texture->SetNativeObject(Texture::Create(texture));
				}
				texture->GetNativeObject()->Bind(id);
			}
		}
	}

	ID3D12PipelineState* Shader::GetPipelineState() const
	{
		return m_pipelineState.Get();
	}

	bool Shader::IsCompiled() const
	{
		struct stat fileStat;
		stat(m_path.c_str(), &fileStat);

		return m_lastCompileTime >= fileStat.st_mtime;
	}

	void Shader::Compile()
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

		CreatePipelineState();

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

	void Shader::CreatePipelineState()
	{
		std::vector<D3D12_INPUT_ELEMENT_DESC> inputElements = GetInputElements();
		const std::vector<DXGI_FORMAT>        outputFormats = GetOutputFormats();

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
			CD3DX12_BLEND_DESC(D3D12_DEFAULT),
			UINT_MAX,
			CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT),
			CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT),
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

		pipelineStateDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;

		Dx12Context::Get().GetDevice()->
						   CreateGraphicsPipelineState(&pipelineStateDesc, IID_PPV_ARGS(&m_pipelineState));

		// ReSharper disable once CppUseStructuredBinding
		for (const auto& inputElement : inputElements)
		{
			delete[] inputElement.SemanticName;
		}

		SetDebugName(m_pipelineState, m_path.c_str());
	}

	std::vector<D3D12_INPUT_ELEMENT_DESC> Shader::GetInputElements() const
	{
		std::vector<D3D12_INPUT_ELEMENT_DESC> inputElements(m_vs->GetDescriptor().InputParameters);

		for (uint32_t i = 0; i < m_vs->GetDescriptor().InputParameters; ++i)
		{
			D3D12_SIGNATURE_PARAMETER_DESC signatureParameter;
			m_vs->GetReflector()->GetInputParameterDesc(i, &signatureParameter);

			inputElements[i].SemanticName = _strdup(signatureParameter.SemanticName);
			inputElements[i].SemanticIndex = signatureParameter.SemanticIndex;
			inputElements[i].Format = GetLayoutElement(signatureParameter.Mask, signatureParameter.ComponentType);
			inputElements[i].InputSlot = i;
			inputElements[i].AlignedByteOffset = 0;
			inputElements[i].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
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

	IShader* Shader::Create(const std::shared_ptr<::Shader>& shader)
	{
		return static_cast<IShader*>(new Shader(shader->GetPath()));
	}
}

namespace Renderer
{
	extern void Deleter(IShader* shader)
	{
		if (shader != nullptr)
		{
			delete shader;
		}
	}
}
