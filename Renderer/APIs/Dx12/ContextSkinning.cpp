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

			data.m_weightsBuffer->BindCompute(0);

			if (!data.m_transformBuffer.IsInitialized())
			{
				data.m_transformBuffer.Init(static_cast<uint32_t>(skeleton.m_bones.size()), sizeof(float) * 16);
			}

			data.m_transformBuffer.GetCurrentBuffer()->SetData(data.m_boneTransforms.data());
			data.m_transformBuffer.GetCurrentBuffer()->BindCompute(1);

			if (!data.m_outputBuffer.IsInitialized())
			{
				data.m_outputBuffer.InitUAV(
					vertexBuffer.m_elementCount * data.m_submesh.GetNativeObject()->GetSkinnedAttributeCount(),
					sizeof(float) * 3);
			}

			DescriptorHeapHandle& heapHandle = *data.m_submesh.GetNativeObject()->GetSkinningHeapHandle();
			GetCommandList()->SetComputeRootDescriptorTable(2, heapHandle.GetGPUHandle());

			data.m_outputBuffer.GetCurrentBuffer()->BindCompute(3, 0);

			m_commandList->Dispatch(ceil((float) vertexBuffer.m_elementCount / 64),
									data.m_submesh.GetNativeObject()->GetSkinnedAttributeCount(), 1);
		}
	}
}
