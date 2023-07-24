#include "stdafx.h"
#include "Context.h"

#include "AssetManager.h"
#include "DynamicSubmesh.h"
#include "RenderObject.h"
#include "StaticSubmesh.h"
#include "Submesh.h"
#include "Assets/Shader.h"
#include "Assets/Skeleton.h"
#include "Assets/SubmeshData.h"
#include "Buffers/Types/PerCallBuffer.h"
#include "Buffers/Types/PerObjectBuffer.h"
#include "Shaders/ComputeShader.h"
#include "Shaders/Shader.h"
#include "Systems/RenderSystem.h"

namespace Rendering::Dx12
{
	void Dx12Context::CreateRenderQueue(const RenderQueue& objectsToRender)
	{
		m_renderQueue.clear();
		m_renderQueue.reserve(objectsToRender.size());

		bool foundAnyMeshToSkin = false;

		std::map<const Skeleton*, std::shared_ptr<DescriptorHeapHandle>> weightBufferHandles;

		std::vector<PerObjectBuffer> accumulatedConstantBuffers;
		std::vector<uint8_t>         accumulatedBoneTransforms;

		bool isCurrentBatched = false;

		uint32_t instanceCount = 0;

		for (uint32_t i = 0; i < objectsToRender.size(); ++i)
		{
			const QueuedRenderObject* queuedObject = objectsToRender[i];

			if (!isCurrentBatched)
			{
				RenderObject& renderObject = m_renderQueue.emplace_back();

				std::shared_ptr<::Shader> shader = queuedObject->m_material->GetShader();
				if (shader->GetNativeObject() == nullptr)
				{
					shader->SetNativeObject(Shader::Create(shader));
				}
				renderObject.m_shader = shader->GetNativeObject();

				UpdateMaterial(queuedObject->m_material, renderObject);

				if (queuedObject->m_submesh->GetNativeObject() == nullptr)
				{
					if (queuedObject->m_submesh->IsDynamic())
					{
						queuedObject->m_submesh->SetNativeObject(DynamicSubmesh::Create(queuedObject->m_submesh));
					}
					else
					{
						queuedObject->m_submesh->SetNativeObject(StaticSubmesh::Create(queuedObject->m_submesh));
					}
				}
				renderObject.m_submesh = queuedObject->m_submesh->GetNativeObject();
				renderObject.m_submesh->Update(queuedObject->m_submesh);

				if (queuedObject->m_boneTransforms.size() != 0)
				{
					if (!foundAnyMeshToSkin)
					{
						foundAnyMeshToSkin = true;
						if (m_skinningShader == nullptr)
						{
							m_skinningShader = AssetManager::GetAsset<::ComputeShader>(
								"../Resources/Shaders/Compute/skinning.hlsl");
							m_skinningShader->SetNativeObject(NativeComputeShader::Create(m_skinningShader));
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
							(uint32_t) skeleton.m_vertexWeights.size(),
							skeleton.m_vertexWeights.data());
						renderObject.m_weightsBuffer   = weightBufferHandle;
						weightBufferHandles[&skeleton] = weightBufferHandle;
					}
				}
			}

			++instanceCount;

			accumulatedConstantBuffers.push_back(queuedObject->m_perObjectBuffer);

			if (queuedObject->m_boneTransforms.size() != 0)
			{
				accumulatedBoneTransforms.resize(
					queuedObject->m_boneTransforms.size() * sizeof(Matrix4D) * instanceCount);
				memcpy(
					accumulatedBoneTransforms.data() +
					sizeof(Matrix4D) * queuedObject->m_boneTransforms.size() * (instanceCount - 1),
					queuedObject->m_boneTransforms.data(),
					queuedObject->m_boneTransforms.size() * sizeof(Matrix4D));
			}


			const bool isNextBatched =
				(objectsToRender.size() != i + 1) &&
				objectsToRender[i + 1]->m_submesh == queuedObject->m_submesh &&
				//objectsToRender[i + 1]->->m_material == queuedObject->m_material &&
				objectsToRender[i + 1]->m_boneTransforms.size() == queuedObject->m_boneTransforms.size() &&
				!objectsToRender[i + 1]->m_clipRect.has_value() &&
				!queuedObject->m_clipRect.has_value();

			if (!isNextBatched)
			{
				RenderObject* lastRenderObject = &m_renderQueue[m_renderQueue.size() - 1];

				lastRenderObject->m_perObjectBuffer = GetScratchBuffer().CreateSRV(
					sizeof(accumulatedConstantBuffers[0]),
					(uint32_t) accumulatedConstantBuffers.size(),
					accumulatedConstantBuffers.data());
				accumulatedConstantBuffers.clear();

				const PerCallBuffer perCallBuffer = {
					instanceCount,
					lastRenderObject->m_submesh->GetVertexCount(),
					lastRenderObject->m_submesh->GetIndexCount(),
					0
				};


				lastRenderObject->m_perCallBuffer = GetScratchBuffer().CreateHandle(
					256,
					&perCallBuffer);

				if (accumulatedBoneTransforms.size() != 0)
				{
					ScratchBufferHandle skinningBuffer = GetScratchBuffer().CreateHandle(
						sizeof(float) * 3 * instanceCount *
						queuedObject->m_submesh->GetVertexBuffer().GetElementCount(),
						nullptr,
						true);

					lastRenderObject->m_skinningBufferHandle = GetScratchBuffer().CreateUAV(
						skinningBuffer,
						sizeof(float) * 3);

					lastRenderObject->m_boneTransforms = GetScratchBuffer().CreateSRV(
						sizeof(float) * 16,
						(uint32_t) queuedObject->m_submesh->GetSkeleton().m_bones.size() * instanceCount,
						accumulatedBoneTransforms.data());
					accumulatedBoneTransforms.clear();
				}

				lastRenderObject->m_clipRect = queuedObject->m_clipRect;

				lastRenderObject->m_instanceCount = instanceCount;
				instanceCount                     = 0;
			}

			isCurrentBatched = isNextBatched;
		}

		m_scratchBuffer.SubmitBuffers();
	}
}
