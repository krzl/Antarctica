#include "stdafx.h"

#include "Buffer.h"
#include "Context.h"
#include "Submesh.h"
#include "Entities/AnimatedMeshComponent.h"

#include "Shaders/ComputeShader.h"

namespace Renderer::Dx12
{
	void Dx12Context::UpdateSkinning(IComputeShader* computeShader, std::vector<SkinningData>& skinningData)
	{
		computeShader->Bind();

		uint32_t currentHeapId = 0;
		for (SkinningData& data : skinningData)
		{
			const Skeleton&   skeleton     = data.m_submesh.GetSkeleton();
			const MeshBuffer& vertexBuffer = data.m_submesh.GetVertexBuffer();

			if (skeleton.m_bones.size() == 0)
			{
				continue;
			}

			if (data.m_submesh.GetNativeObject() == nullptr)
			{
				data.m_submesh.SetNativeObject(Submesh::Create(&data.m_submesh));
			}

			if (data.m_weightsBuffer == nullptr)
			{
				data.m_weightsBuffer = NativeBufferPtr(new IBuffer(), Renderer::Deleter);
				data.m_weightsBuffer->Init(vertexBuffer.m_elementCount, sizeof(VertexWeights));
				data.m_weightsBuffer->SetData(skeleton.m_vertexWeights.data());
			}

			data.m_weightsBuffer->BindCompute(1);

			if (!data.m_transformBuffer.IsInitialized())
			{
				data.m_transformBuffer.Init(static_cast<uint32_t>(skeleton.m_bones.size()), sizeof(float) * 16);
			}

			data.m_transformBuffer.GetCurrentBuffer()->SetData(data.m_boneTransforms.data());
			data.m_transformBuffer.GetCurrentBuffer()->BindCompute(2);

			if (data.m_outputBuffers.size() == 0)
			{
				std::unordered_map<float, float> a;

				const AttributeUsage& attributes = data.m_submesh.GetAttributes();


				data.m_outputBuffers.resize(4);

				std::array<bool, 4> usedAttributes = {
					true,
					(bool) attributes.m_hasNormals,
					(bool) attributes.m_hasTangents,
					(bool) attributes.m_hasBitangents
				};

				for (uint32_t i = 0; i < usedAttributes.size(); ++i)
				{
					if (usedAttributes[i])
					{
						DynamicBuffer& buffer = data.m_outputBuffers[i];
						buffer.InitUAV(vertexBuffer.m_elementCount, sizeof(float) * 3);
					}
				}
			}

			constexpr MeshAttribute skinnableAttributes[] = {
				MeshAttribute::POSITION,
				MeshAttribute::NORMAL,
				MeshAttribute::TANGENT,
				MeshAttribute::BINORMAL
			};

			uint32_t outputBufferIdx = 0;

			for (const MeshAttribute attribute : skinnableAttributes)
			{
				const D3D12_VERTEX_BUFFER_VIEW* view =
					data.m_submesh.GetNativeObject()->GetVertexBufferViewForAttribute(attribute);

				if (view == nullptr)
				{
					continue;
				}

				DescriptorHeapHandle& heapHandle = *data.m_submesh.GetNativeObject()->GetAttributeHeapHandle(attribute);
				GetCommandList()->SetComputeRootDescriptorTable(0, heapHandle.GetGPUHandle());

				data.m_outputBuffers[outputBufferIdx++].GetCurrentBuffer()->BindCompute(3, 0);

				m_commandList->Dispatch(ceil((float) vertexBuffer.m_elementCount / 64), 1, 1);
			}
		}
	}
}
