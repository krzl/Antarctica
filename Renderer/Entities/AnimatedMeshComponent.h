#pragma once

#include "StaticMeshComponent.h"
#include "Buffers/DynamicBuffer.h"
#include "Solver/Solver.h"

typedef std::unique_ptr<Renderer::IBuffer, void(*)(Renderer::IBuffer*)> NativeBufferPtr;

namespace Renderer
{
	class AnimatedMeshComponent : public StaticMeshComponent
	{
	public:

		[[nodiscard]] const std::shared_ptr<Anim::Animator>& GetAnimator() const
		{
			return m_animator;
		}

		void SetMesh(const std::shared_ptr<Mesh>& mesh) override;

		void SetAnimator(const std::shared_ptr<Anim::Animator> animator);

		void SetTrigger(int32_t id, bool value);

		BoundingBox GetBoundingBox() const override;
		
	protected:

		std::shared_ptr<Anim::Animator> m_animator;

		Transform4D GetAttachedNodeTransform(int32_t nodeId, bool ignoreAttachmentRotation) override;
		void SetupRenderHandle(uint32_t submeshId, Material& material, QueuedRenderObject& renderObject) override;
		void PrepareForRender(RenderQueue& renderQueue) override;

	private:

		Anim::Solver m_animationSolver;

		std::vector<Transform4D> m_animatedTransforms;

		DEFINE_CLASS()
	};

	CREATE_CLASS(AnimatedMeshComponent)
}
