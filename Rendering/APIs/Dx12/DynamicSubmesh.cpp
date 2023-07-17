#include "stdafx.h"
#include "DynamicSubmesh.h"

#include "Context.h"
#include "Shaders/Shader.h"

namespace Rendering::Dx12
{
	void DynamicSubmesh::Init(const ::Submesh* submesh) { }

	void DynamicSubmesh::Update(const ::Submesh* submesh)
	{
		if (m_lastUpdateFrame == Dx12Context::Get().GetCurrentFenceId())
		{
			return;
		}

		m_lastUpdateFrame = Dx12Context::Get().GetCurrentFenceId();
		m_vertexCount     = submesh->GetVertexBuffer().GetElementCount();
		m_indexCount      = submesh->GetIndexBuffer().GetElementCount();
		m_attributes      = submesh->GetAttributes();

		m_vertexBufferHandle = Dx12Context::Get().GetScratchBuffer().CreateHandle(
			submesh->GetVertexBuffer().m_elementSize * m_vertexCount,
			submesh->GetVertexBuffer().m_data.data());

		m_indexBufferHandle = Dx12Context::Get().GetScratchBuffer().CreateHandle(
			submesh->GetIndexBuffer().m_elementSize * m_indexCount,
			submesh->GetIndexBuffer().m_data.data());
	}

	void DynamicSubmesh::Bind(const RenderObject& renderObject) const
	{
		const D3D12_GPU_VIRTUAL_ADDRESS vertexBufferAddress =
			Dx12Context::Get().GetScratchBuffer().GetGpuPointer(m_vertexBufferHandle);

		const AttributeOffsets& offsets = m_attributes.GetAttributeOffsets();

		for (auto& [inputSlot, attribute] : renderObject.m_shader->GetInputSlotBindings())
		{
			const uint32_t offset            = GetAttributeOffset(attribute, offsets);
			const uint32_t attributeDataSize = GetAttributeDataSize(attribute, offsets);

			D3D12_VERTEX_BUFFER_VIEW vertexBufferView =
			{
				vertexBufferAddress + offset * m_vertexCount,
				attributeDataSize * m_vertexCount,
				attributeDataSize
			};

			Dx12Context::Get().GetCommandList()->IASetVertexBuffers(inputSlot, 1, &vertexBufferView);
		}

		Dx12Context::Get().GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		const D3D12_INDEX_BUFFER_VIEW indexBufferView =
		{
			Dx12Context::Get().GetScratchBuffer().GetGpuPointer(m_indexBufferHandle),
			4 * m_indexCount,
			DXGI_FORMAT_R32_UINT
		};

		Dx12Context::Get().GetCommandList()->IASetIndexBuffer(&indexBufferView);
	}

	NativeSubmesh* DynamicSubmesh::Create(const ::Submesh* submesh)
	{
		DynamicSubmesh* const nativeSubmesh = new DynamicSubmesh();
		nativeSubmesh->Init(submesh);
		return nativeSubmesh;
	}
}
