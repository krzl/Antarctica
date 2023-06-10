#include "stdafx.h"

#include "AssetManager.h"
#include "Context.h"
#include "Submesh.h"
#include "Assets/Skeleton.h"
#include "Assets/SubmeshData.h"
#include "Buffers/Types/PerCallBuffer.h"
#include "Shaders/ComputeShader.h"
#include "Shaders/Shader.h"

namespace Renderer::Dx12
{
	void Dx12Context::CreateRenderQueue(RenderQueue& objectsToRender)
	{
		m_renderQueue.clear();
		m_renderQueue.reserve(objectsToRender.size());

		bool foundAnyMeshToSkin = false;

		std::map<const Skeleton*, std::shared_ptr<DescriptorHeapHandle>> weightBufferHandles;

		std::vector<PerObjectBuffer> accumulatedConstantBuffers;
		std::vector<uint8_t>         accumulatedBoneTransforms;

		const ::Submesh* previousSubmesh    = nullptr;
		uint32_t         previousBonesCount = 0;

		uint32_t instanceCount = 1;

		for (const QueuedRenderObject* queuedObject : objectsToRender)
		{
			if (previousSubmesh)
			{
				const bool isLastElement        = *(objectsToRender.end() - 1)._Ptr == queuedObject;
				const bool canBatchWithPrevious = previousSubmesh == queuedObject->m_submesh &&
												  //previousQueuedObject->m_material == queuedObject->m_material &&
												  previousBonesCount == queuedObject->m_boneTransforms.size() &&
												  instanceCount < 2048;

				if (canBatchWithPrevious)
				{
					accumulatedConstantBuffers.push_back(queuedObject->m_perObjectBuffer);

					if (queuedObject->m_boneTransforms.size() != 0)
					{
						accumulatedBoneTransforms.resize(
							(instanceCount + 1) * queuedObject->m_boneTransforms.size() * sizeof(Matrix4D));
						memcpy(
							accumulatedBoneTransforms.data() + instanceCount * queuedObject->m_boneTransforms.size() *
							sizeof(Matrix4D), queuedObject->m_boneTransforms.data(),
							queuedObject->m_boneTransforms.size() * sizeof(Matrix4D));
					}


					++instanceCount;

					if (!isLastElement)
					{
						continue;
					}
				}

				RenderObject* lastRenderObject = &m_renderQueue[m_renderQueue.size() - 1];

				lastRenderObject->m_perObjectBuffer = GetScratchBuffer().CreateSRV(
					(uint32_t) accumulatedConstantBuffers.size(),
					sizeof(accumulatedConstantBuffers[0]),
					accumulatedConstantBuffers.data());
				accumulatedConstantBuffers.clear();

				const PerCallBuffer perCallBuffer = {
					instanceCount,
					lastRenderObject->m_submesh->GetVertexCount(),
					lastRenderObject->m_submesh->GetIndexCount(),
					0
				};


				lastRenderObject->m_perCallBuffer = GetScratchBuffer().CreateHandle(
					sizeof PerCallBuffer, &perCallBuffer);

				if (accumulatedBoneTransforms.size() != 0)
				{
					ScratchBufferHandle skinningBuffer = GetScratchBuffer().CreateHandle(
						sizeof(float) * 3 * instanceCount *
						previousSubmesh->GetVertexBuffer().m_elementCount);

					lastRenderObject->m_skinningBufferHandle = GetScratchBuffer().CreateUAV(
						skinningBuffer, sizeof(float) * 3);

					lastRenderObject->m_boneTransforms = GetScratchBuffer().CreateSRV(
						sizeof(float) * 16,
						(uint32_t) previousSubmesh->GetSkeleton().m_bones.size() * instanceCount,
						accumulatedBoneTransforms.data());
					accumulatedBoneTransforms.clear();
				}

				lastRenderObject->m_instanceCount = instanceCount;
				instanceCount                     = 1;

				if (isLastElement && canBatchWithPrevious)
				{
					continue;
				}
			}

			previousSubmesh    = queuedObject->m_submesh;
			previousBonesCount = (uint32_t) queuedObject->m_boneTransforms.size();

			RenderObject& renderObject = m_renderQueue.emplace_back();

			std::shared_ptr<::Shader> shader = queuedObject->m_material->GetShader();
			if (shader->GetNativeObject() == nullptr)
			{
				shader->SetNativeObject(Shader::Create(shader));
			}
			renderObject.m_shader = shader->GetNativeObject();

			UpdateMaterial(queuedObject->m_material, renderObject);

			accumulatedConstantBuffers.push_back(queuedObject->m_perObjectBuffer);

			if (queuedObject->m_submesh->GetNativeObject() == nullptr)
			{
				queuedObject->m_submesh->SetNativeObject(Submesh::Create(queuedObject->m_submesh));
			}
			renderObject.m_submesh = queuedObject->m_submesh->GetNativeObject();

			if (queuedObject->m_boneTransforms.size() != 0)
			{
				if (!foundAnyMeshToSkin)
				{
					foundAnyMeshToSkin = true;
					if (m_skinningShader == nullptr)
					{
						m_skinningShader = AssetManager::GetAsset<::ComputeShader>(
							"../Resources/Shaders/Compute/skinning.hlsl");
						m_skinningShader->SetNativeObject(IComputeShader::Create(m_skinningShader));
					}
				}

				const Skeleton& skeleton = queuedObject->m_submesh->GetSkeleton();

				auto it = weightBufferHandles.find(&skeleton);
				if (it != weightBufferHandles.end())
				{
					renderObject.m_weightsBuffer = it->second;
				}
				else
				{
					const std::shared_ptr<DescriptorHeapHandle> weightBufferHandle = GetScratchBuffer().CreateSRV(
						sizeof(VertexWeights),
						(uint32_t) skeleton.m_vertexWeights.size(), skeleton.m_vertexWeights.data());
					renderObject.m_weightsBuffer   = weightBufferHandle;
					weightBufferHandles[&skeleton] = weightBufferHandle;
				}

				accumulatedBoneTransforms.resize(queuedObject->m_boneTransforms.size() * sizeof(Matrix4D));
				memcpy(accumulatedBoneTransforms.data(),
					   queuedObject->m_boneTransforms.data(),
					   queuedObject->m_boneTransforms.size() * sizeof(Matrix4D));
			}
		}

		m_scratchBuffer.SubmitBuffers();
	}
}
