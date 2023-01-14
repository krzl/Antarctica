#pragma once

#include <d3d12shader.h>
#include "Objects/SubmeshData.h"
#include "ShaderDescriptor.h"

namespace Renderer
{
	class ShaderObject
	{
		
	public:

		explicit ShaderObject(const std::string& path) :
			m_path(path)
		{
		}

		~ShaderObject();

		void Bind(const AttributeUsage& attributeUsage) const;
		
		[[nodiscard]] const ShaderDescriptor& GetShaderDescriptor() const
		{
			return shaderDescriptor;
		}
		
		[[nodiscard]] ShaderDescriptor& GetShaderDescriptor()
		{
			return shaderDescriptor;
		}
		
	private:

		bool IsCompiled() const;

		void Compile();
		void PopulateShaderDescriptor();
		
		ComPtr<ID3D12PipelineState> CreatePipelineState(AttributeUsage attributeUsage) const;	

		ID3D12PipelineState* GetPipelineState(AttributeUsage attributeUsage) const;

		std::vector<D3D12_INPUT_ELEMENT_DESC> GetInputElements(AttributeUsage attributeUsage) const;
		static DXGI_FORMAT GetLayoutElement(const uint8_t mask, const D3D_REGISTER_COMPONENT_TYPE componentType);

		std::vector<DXGI_FORMAT> GetOutputFormats() const;
		
		D3D12_SHADER_DESC m_vsDescriptor = {};
		ComPtr<ID3D12ShaderReflection> m_vsReflector = nullptr;
		
		D3D12_SHADER_DESC m_psDescriptor = {};
		ComPtr<ID3D12ShaderReflection> m_psReflector = nullptr;
		
		ComPtr<ID3D12RootSignature> m_rootSignature = nullptr;
		mutable std::unordered_map<uint16_t, ComPtr<ID3D12PipelineState>> m_pipelineStates;

		std::string m_path = "";

		time_t m_lastCompileTime = 0;

		ComPtr<ID3DBlob> m_vsByteCode = nullptr;
		ComPtr<ID3DBlob> m_psByteCode = nullptr;
		ComPtr<ID3DBlob> m_dsByteCode = nullptr;
		ComPtr<ID3DBlob> m_hsByteCode = nullptr;
		ComPtr<ID3DBlob> m_gsByteCode = nullptr;

		ShaderDescriptor shaderDescriptor;
	};
}
