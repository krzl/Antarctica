#pragma once
#include <d3d12shader.h>

namespace Renderer
{
	class ShaderObject
	{
		
	public:

		explicit ShaderObject(const std::string& path) :
			m_path(path)
		{
		}

		~ShaderObject()
		{
			
		}

		void Bind() const;

	private:

		bool IsCompiled() const;

		void Compile();
		void CreatePipelineState();

		std::vector<D3D12_INPUT_ELEMENT_DESC> GetInputElements() const;
		static DXGI_FORMAT GetLayoutElement(const uint8_t mask, const D3D_REGISTER_COMPONENT_TYPE componentType);

		std::vector<DXGI_FORMAT> GetOutputFormats() const;

		ComPtr<ID3D12RootSignature> m_rootSignature;
		ComPtr<ID3D12PipelineState> m_pipelineState;

		std::string m_path;

		ComPtr<ID3DBlob> m_vsByteCode = nullptr;
		ComPtr<ID3DBlob> m_psByteCode = nullptr;
		ComPtr<ID3DBlob> m_dsByteCode = nullptr;
		ComPtr<ID3DBlob> m_hsByteCode = nullptr;
		ComPtr<ID3DBlob> m_gsByteCode = nullptr;
	};
}
