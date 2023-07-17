#pragma once

#include "../Common.h"

//TODO: DONT INCLUDE WINDOWS EVERYWHERE
#undef DOMAIN

namespace Rendering::Dx12
{
	class ShaderStage
	{
	public:

		enum class Type
		{
			VERTEX,
			PIXEL,
			DOMAIN,
			HULL,
			GEOMETRY,
			COMPUTE
		};

		explicit ShaderStage(Type type);

		void                        Compile(const std::string& path);
		ComPtr<ID3D12RootSignature> CreateRootSignature() const;

		void CreateReflectionData();

		[[nodiscard]] const D3D12_SHADER_DESC& GetDescriptor() const
		{
			return m_descriptor;
		}

		[[nodiscard]] ID3D12ShaderReflection* GetReflector() const
		{
			return m_reflector.Get();
		}

		[[nodiscard]] bool IsCompiled() const
		{
			return m_byteCode != nullptr;
		}

		[[nodiscard]] const ComPtr<ID3DBlob>& GetByteCode() const
		{
			return m_byteCode;
		}

		~ShaderStage();

	private:

		const Type m_type;

		ComPtr<ID3DBlob>               m_byteCode   = nullptr;
		ComPtr<ID3D12ShaderReflection> m_reflector  = nullptr;
		D3D12_SHADER_DESC              m_descriptor = {};
	};
}
