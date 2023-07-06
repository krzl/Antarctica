#pragma once

#include "Submesh.h"
#include "Assets/SubmeshData.h"
#include "Buffers/ScratchBufferHandle.h"

namespace Renderer::Dx12
{
	class DynamicSubmesh : public NativeSubmesh
	{
	public:

		void Update(const ::Submesh* submesh) override;
		void Bind(const RenderObject& renderObject) const override;

		static NativeSubmesh* Create(const ::Submesh* submesh);

	protected:

		void Init(const ::Submesh* submesh) override;

		DynamicSubmesh() = default;

	private:

		uint32_t m_lastUpdateFrame = 0xFFFFFFFF;

		ScratchBufferHandle m_vertexBufferHandle;
		ScratchBufferHandle m_indexBufferHandle;

		AttributeUsage m_attributes;
	};
}
