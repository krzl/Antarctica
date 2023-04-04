#pragma once

#include <memory>

#include "ShaderStage.h"
#include "../Common.h"
#include "Shaders/ShaderDescriptor.h"

class Shader;
class Material;
struct AttributeUsage;

namespace Renderer::Dx12
{
	enum class MeshAttribute;

	class Shader
	{
	public:

		~Shader();

		bool IsCompiled() const;
		void Compile();

		void Bind();
		void BindMaterial(const Material& materialData) const;

		ID3D12PipelineState* GetPipelineState() const;

		[[nodiscard]] const ShaderDescriptor& GetShaderDescriptor() const
		{
			return m_shaderDescriptor;
		}

		[[nodiscard]] ShaderDescriptor& GetShaderDescriptor()
		{
			return m_shaderDescriptor;
		}

		[[nodiscard]] const std::map<uint32_t, MeshAttribute>& GetInputSlotBindings() const
		{
			return m_inputSlotBindings;
		}
		
		static IShader* Create(const std::shared_ptr<::Shader>& shader);

	private:

		explicit Shader(std::string path) :
			m_path(std::move(path)),
			m_lastCompileTime(0) { }

		void PopulateShaderDescriptor();
		void CreatePipelineState();

		std::vector<D3D12_INPUT_ELEMENT_DESC> GetInputElements() const;
		std::vector<DXGI_FORMAT>              GetOutputFormats() const;

		static DXGI_FORMAT GetLayoutElement(const uint8_t mask, const D3D_REGISTER_COMPONENT_TYPE componentType);

		std::string m_path;

		ComPtr<ID3D12RootSignature>         m_rootSignature = nullptr;
		mutable ComPtr<ID3D12PipelineState> m_pipelineState = nullptr;

		std::map<uint32_t, MeshAttribute> m_inputSlotBindings;

		time_t m_lastCompileTime;//TODO: Move to Asset class and add callback on asset changed

		ShaderDescriptor m_shaderDescriptor; //TODO: MOVE TO ASSET

		std::unique_ptr<ShaderStage> m_vs;
		std::unique_ptr<ShaderStage> m_ps;

		std::unique_ptr<ShaderStage> m_ds;
		std::unique_ptr<ShaderStage> m_hs;
		std::unique_ptr<ShaderStage> m_gs;
	};
}

namespace Renderer
{
	class IShader : public Dx12::Shader {};

	extern void Deleter(IShader* shader);
}
