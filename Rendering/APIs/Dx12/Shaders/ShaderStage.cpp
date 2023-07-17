#include "stdafx.h"
#include "ShaderStage.h"

#include <d3dcompiler.h>

#include "APIs/Dx12/Context.h"

namespace Rendering::Dx12
{
	ShaderStage::ShaderStage(const Type type)
		: m_type(type) { }

	static std::vector<std::pair<const char*, const char*>> entryPoints = {
		{ "vs", "vs_5_0" },
		{ "ps", "ps_5_0" },
		{ "ds", "ds_5_0" },
		{ "hs", "hs_5_0" },
		{ "gs", "gs_5_0" },
		{ "cs", "cs_5_0" },
	};

	void ShaderStage::Compile(const std::string& path)
	{
		// ReSharper disable once CppInitializedValueIsAlwaysRewritten
		UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
		compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

		ComPtr<ID3DBlob> errors;

		const std::wstring wPath(path.begin(), path.end());

		auto& [entryPoint, target] = entryPoints[static_cast<uint8_t>(m_type)];

		uint32_t retries = 1;
		while (m_byteCode == nullptr && retries > 0)
		{
			const HRESULT res = D3DCompileFromFile(wPath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
												   entryPoint,
												   target, compileFlags, 0, &m_byteCode, &errors);

			if (res != ERROR_SHARING_VIOLATION)
			{
				--retries;
			}
		}

		if (m_type == Type::VERTEX || m_type == Type::PIXEL || m_type == Type::COMPUTE)
		{
			if (errors != nullptr)
			{
				OutputDebugStringA((char*) errors->GetBufferPointer());
			}

			if (errors != nullptr || m_byteCode == nullptr)
			{
				throw "Error compiling shader";
			}
		}
	}

	ComPtr<ID3D12RootSignature> ShaderStage::CreateRootSignature() const
	{
		ComPtr<ID3D12RootSignature> rootSignature;

		Dx12Context::Get().GetDevice()->CreateRootSignature(0, m_byteCode->GetBufferPointer(),
															m_byteCode->GetBufferSize(),
															IID_PPV_ARGS(rootSignature.ReleaseAndGetAddressOf()));

		return rootSignature;
	}

	void ShaderStage::CreateReflectionData()
	{
		D3DReflect(m_byteCode->GetBufferPointer(), m_byteCode->GetBufferSize(), IID_PPV_ARGS(&m_reflector));
		m_reflector->GetDesc(&m_descriptor);
	}

	ShaderStage::~ShaderStage()
	{
		RELEASE_DX(m_byteCode);
		RELEASE_DX(m_reflector);
	}
}
