#pragma once

#include <memory>

#include "ShaderStage.h"
#include "../Common.h"
#include "Assets/ComputeShader.h"
#include "Shaders/ShaderDescriptor.h"

namespace Rendering::Dx12
{
	class ComputeShader
	{
	public:

		~ComputeShader();

		bool IsCompiled() const;
		void Compile();

		void Bind();

		[[nodiscard]] const ShaderDescriptor& GetShaderDescriptor() const
		{
			return m_shaderDescriptor;
		}

		[[nodiscard]] ShaderDescriptor& GetShaderDescriptor()
		{
			return m_shaderDescriptor;
		}

		static NativeComputeShader* Create(const std::shared_ptr<::ComputeShader>& shader);

	private:

		explicit ComputeShader(std::string path) :
			m_path(std::move(path)),
			m_lastCompileTime(0) { }

		void PopulateShaderDescriptor();
		void CreatePipelineState();

		std::string m_path;

		ComPtr<ID3D12RootSignature> m_rootSignature = nullptr;
		ComPtr<ID3D12PipelineState> m_pipelineState = nullptr;

		time_t m_lastCompileTime;//TODO: Move to Asset class and add callback on asset changed

		ShaderDescriptor m_shaderDescriptor;//TODO: MOVE TO ASSET

		std::unique_ptr<ShaderStage> m_cs;
	};
}

namespace Rendering
{
	class NativeComputeShader : public Dx12::ComputeShader {};

	extern void Deleter(NativeComputeShader* shader);
}
