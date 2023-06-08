#include "stdafx.h"

#include "Buffer.h"
#include "Context.h"
#include "Submesh.h"

#include "Shaders/ComputeShader.h"

namespace Renderer::Dx12
{
	void Dx12Context::UpdateSkinning()
	{
		bool isFirstObject = true;
		for (const RenderObject& renderObject : m_renderQueue)
		{
			if (renderObject.m_skinningBuffer)
			{
				if (isFirstObject)
				{
					isFirstObject = false;
					m_skinningShader->GetNativeObject()->Bind();
				}

				m_commandList->SetComputeRootDescriptorTable(0, renderObject.m_weightsBuffer->GetGPUHandle());
				m_commandList->SetComputeRootDescriptorTable(1, renderObject.m_boneTransforms->GetGPUHandle());

				DescriptorHeapHandle& heapHandle = *renderObject.m_submesh->GetSkinningHeapHandle();
				GetCommandList()->SetComputeRootDescriptorTable(2, heapHandle.GetGPUHandle());

				renderObject.m_skinningBuffer->BindCompute(3);

				m_commandList->Dispatch(ceil((float) renderObject.m_submesh->GetVertexCount() / 64),
										renderObject.m_submesh->GetSkinnedAttributeCount(), 1);
			}
		}
	}
}
