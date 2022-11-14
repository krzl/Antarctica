#include "stdafx.h"
#include "ShaderObject.h"

#include "d3dcompiler.h"
#include "RenderSystem.h"

namespace Renderer
{
	bool ShaderObject::IsCompiled() const
	{
		struct stat fileStat;
		stat(m_path.c_str(), &fileStat);
		
		return m_lastCompileTime >= fileStat.st_mtime;
	}

	void ShaderObject::Compile()
	{
		// ReSharper disable once CppInitializedValueIsAlwaysRewritten
		UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
		compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

		ComPtr<ID3DBlob> errors;

		const std::wstring wPath(m_path.begin(), m_path.end());

		D3DCompileFromFile(wPath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "vs",
						   "vs_5_0", compileFlags, 0, &m_vsByteCode, &errors);

		if (errors != nullptr)
		{
			OutputDebugStringA((char*) errors->GetBufferPointer());
			throw "Error compiling vertex shader";
		}

		if (m_vsByteCode == nullptr)
		{
			return;
		}

		RenderSystem::Get().GetDevice()->CreateRootSignature(0, m_vsByteCode->GetBufferPointer(),
															 m_vsByteCode->GetBufferSize(),
															 IID_PPV_ARGS(m_rootSignature.ReleaseAndGetAddressOf()));

		D3DCompileFromFile(wPath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "ps",
						   "ps_5_0", compileFlags, 0, &m_psByteCode, &errors);

		if (errors != nullptr)
		{
			OutputDebugStringA((char*) errors->GetBufferPointer());
			throw "Error compiling pixel shader";
		}

		D3DCompileFromFile(wPath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "ds",
						   "ds_5_0", compileFlags, 0, &m_dsByteCode, &errors);

		D3DCompileFromFile(wPath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "hs",
						   "hs_5_0", compileFlags, 0, &m_hsByteCode, &errors);

		D3DCompileFromFile(wPath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "gs",
						   "gs_5_0", compileFlags, 0, &m_gsByteCode, &errors);


		D3DReflect(m_vsByteCode->GetBufferPointer(), m_vsByteCode->GetBufferSize(), IID_PPV_ARGS(&m_vsReflector));
		m_vsReflector->GetDesc(&m_vsDescriptor);
		
		D3DReflect(m_psByteCode->GetBufferPointer(), m_psByteCode->GetBufferSize(), IID_PPV_ARGS(&m_psReflector));
		m_psReflector->GetDesc(&m_psDescriptor);
		
		struct stat fileStat;
		stat(m_path.c_str(), &fileStat);
		
		m_lastCompileTime = fileStat.st_mtime;
		
	}

	ComPtr<ID3D12PipelineState> ShaderObject::CreatePipelineState(const AttributeUsage attributeUsage) const
	{
		std::vector<D3D12_INPUT_ELEMENT_DESC> inputElements = GetInputElements(attributeUsage);
		std::vector<DXGI_FORMAT> outputFormats              = GetOutputFormats();

		D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDesc = {
			m_rootSignature.Get(),
			{
				m_vsByteCode->GetBufferPointer(),
				m_vsByteCode->GetBufferSize()
			},
			{
				m_psByteCode->GetBufferPointer(),
				m_psByteCode->GetBufferSize()
			},
			{
				m_dsByteCode.Get() != nullptr ? m_dsByteCode->GetBufferPointer() : nullptr,
				m_dsByteCode.Get() != nullptr ? m_dsByteCode->GetBufferSize() : 0
			},
			{
				m_hsByteCode.Get() != nullptr ? m_hsByteCode->GetBufferPointer() : nullptr,
				m_hsByteCode.Get() != nullptr ? m_hsByteCode->GetBufferSize() : 0
			},
			{
				m_gsByteCode.Get() != nullptr ? m_gsByteCode->GetBufferPointer() : nullptr,
				m_gsByteCode.Get() != nullptr ? m_gsByteCode->GetBufferSize() : 0
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
				(uint32_t) RenderSystem::Get().GetMsNumQualityLevels() - 1
			},
			0,
			{},
			D3D12_PIPELINE_STATE_FLAG_NONE
		};

		for (uint32_t i = 0; i < outputFormats.size(); i++)
		{
			pipelineStateDesc.RTVFormats[i] = outputFormats[i];
		}

		ComPtr<ID3D12PipelineState> pipelineState;
		RenderSystem::Get().GetDevice()->
							CreateGraphicsPipelineState(&pipelineStateDesc, IID_PPV_ARGS(&pipelineState));
		
		for (auto& inputElement : inputElements)
		{
			delete[] inputElement.SemanticName;
		}

		return pipelineState;
	}
	
	ID3D12PipelineState* ShaderObject::GetPipelineState(const AttributeUsage attributeUsage) const
	{
		const auto it = m_pipelineStates.find(attributeUsage);
		if (it != m_pipelineStates.end())
		{
			return it->second.Get();
		}

		const ComPtr<ID3D12PipelineState> pipelineState = CreatePipelineState(attributeUsage);
		m_pipelineStates[attributeUsage] = pipelineState;
		return pipelineState.Get();
	}

	std::vector<D3D12_INPUT_ELEMENT_DESC> ShaderObject::GetInputElements(const AttributeUsage attributeUsage) const
	{
		std::vector<D3D12_INPUT_ELEMENT_DESC> inputElements(m_vsDescriptor.InputParameters);

		const AttributeOffsets& offsets = AttributeUsage::m_attributeOffsets[attributeUsage];

		for (uint32_t i = 0; i < m_vsDescriptor.InputParameters; ++i)
		{
			D3D12_SIGNATURE_PARAMETER_DESC signatureParameter;
			m_vsReflector->GetInputParameterDesc(i, &signatureParameter);
			
			inputElements[i].SemanticName = _strdup(signatureParameter.SemanticName);
			inputElements[i].SemanticIndex = signatureParameter.SemanticIndex;
			inputElements[i].Format = GetLayoutElement(signatureParameter.Mask, signatureParameter.ComponentType);
			inputElements[i].AlignedByteOffset = offsets.GetOffset(inputElements[i].SemanticName, inputElements[i].SemanticIndex);
			inputElements[i].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
			inputElements[i].InstanceDataStepRate = 0;
		}

		return inputElements;
	}

	DXGI_FORMAT ShaderObject::GetLayoutElement(const uint8_t mask, const D3D_REGISTER_COMPONENT_TYPE componentType)
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

	std::vector<DXGI_FORMAT> ShaderObject::GetOutputFormats() const
	{
		std::vector<DXGI_FORMAT> outputElements(m_psDescriptor.OutputParameters);

		for (uint32_t i = 0; i < m_psDescriptor.OutputParameters; ++i)
		{
			D3D12_SIGNATURE_PARAMETER_DESC signatureParameter;
			m_psReflector->GetOutputParameterDesc(i, &signatureParameter);

			outputElements[i] = DXGI_FORMAT_R8G8B8A8_UNORM;
		}

		return outputElements;
	}
}
