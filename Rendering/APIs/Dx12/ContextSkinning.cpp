#include "stdafx.h"

#include "Context.h"
#include "StaticSubmesh.h"
#include "Submesh.h"

#include "Shaders/ComputeShader.h"

namespace Rendering::Dx12
{
	void Dx12Context::UpdateSkinning()
	{
		bool isFirstObject = true;
		for (const RenderObject& renderObject : m_renderQueue)
		{
			if (renderObject.m_skinningBufferHandle)
			{
				if (isFirstObject)
				{
					isFirstObject = false;
					m_skinningShader->GetNativeObject()->Bind();
				}

				m_commandList->SetComputeRootDescriptorTable(0, renderObject.m_weightsBuffer->GetGPUHandle());
				m_commandList->SetComputeRootDescriptorTable(1, renderObject.m_boneTransforms->GetGPUHandle());

				StaticSubmesh* staticSubmesh = static_cast<StaticSubmesh*>(renderObject.m_submesh);
				
				DescriptorHeapHandle& heapHandle = *staticSubmesh->GetSkinningHeapHandle();
				m_commandList->SetComputeRootDescriptorTable(2, heapHandle.GetGPUHandle());

				m_commandList->SetComputeRootDescriptorTable(3, renderObject.m_skinningBufferHandle->GetGPUHandle());
				
				m_commandList->Dispatch(ceil((float) renderObject.m_submesh->GetVertexCount() * renderObject.m_instanceCount / 64),
										1, 1);
			}
		}
	}
}
