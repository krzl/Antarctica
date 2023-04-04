#include "stdafx.h"
#include "ComputeShader.h"
#include "Assets/ComputeShader.h"

#include "ShaderStage.h"
#include "ShaderUtils.h"
#include "APIs/Dx12/Context.h"

namespace Renderer::Dx12
{
	ComputeShader::~ComputeShader()
	{
		RELEASE_DX(m_rootSignature);
	}

	void ComputeShader::Bind()
	{
		if (!IsCompiled())
		{
			Compile();
		}

		ID3D12GraphicsCommandList* commandList = Dx12Context::Get().GetCommandList();

		commandList->SetPipelineState(m_pipelineState.Get());
		commandList->SetComputeRootSignature(m_rootSignature.Get());
	}

	bool ComputeShader::IsCompiled() const
	{
		struct stat fileStat;
		stat(m_path.c_str(), &fileStat);

		return m_lastCompileTime >= fileStat.st_mtime;
	}

	void ComputeShader::Compile()
	{
		m_cs = std::make_unique<ShaderStage>(ShaderStage::Type::COMPUTE);
		m_cs->Compile(m_path);
		m_cs->CreateReflectionData();
		m_rootSignature = m_cs->CreateRootSignature();
		SetDebugName(m_rootSignature, m_path.data());

		PopulateShaderDescriptor();

		struct stat fileStat;
		stat(m_path.c_str(), &fileStat);

		m_lastCompileTime = fileStat.st_mtime;

		CreatePipelineState();
	}

	void ComputeShader::PopulateShaderDescriptor()
	{
		m_shaderDescriptor.Clear();
		UpdateDescriptor(m_cs.get(), m_shaderDescriptor);
	}

	void ComputeShader::CreatePipelineState()
	{
		const D3D12_COMPUTE_PIPELINE_STATE_DESC pipelineStateDesc =
		{
			m_rootSignature.Get(),
			{
				m_cs->GetByteCode()->GetBufferPointer(),
				m_cs->GetByteCode()->GetBufferSize()
			},
			0,
			{},
			D3D12_PIPELINE_STATE_FLAG_NONE
		};

		DXCALL(Dx12Context::Get().GetDevice()->
						   CreateComputePipelineState(&pipelineStateDesc, IID_PPV_ARGS(&m_pipelineState)));
		SetDebugName(m_pipelineState, m_path.data());
	}

	IComputeShader* ComputeShader::Create(const std::shared_ptr<::ComputeShader>& shader)
	{
		return static_cast<IComputeShader*>(new ComputeShader(shader->GetPath()));
	}
}

namespace Renderer
{
	extern void Deleter(IComputeShader* shader)
	{
		if (shader != nullptr)
		{
			delete shader;
		}
	}
}
