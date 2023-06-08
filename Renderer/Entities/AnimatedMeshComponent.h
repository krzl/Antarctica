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

		void OnEnabled() override;
		void OnDisabled() override;

		[[nodiscard]] const std::shared_ptr<Anim::Animator>& GetAnimator() const
		{
			return m_animator;
		}

		void SetMesh(const std::shared_ptr<Mesh>& mesh) override;

		void SetAnimator(const std::shared_ptr<Anim::Animator> animator);

		void SetTrigger(int32_t id, bool value);

	protected:

		std::shared_ptr<Anim::Animator> m_animator;
		std::vector<DynamicBuffer>      m_skinningBuffers;

		Transform4D GetAttachedNodeTransform(int32_t nodeId, bool ignoreAttachmentRotation) override;

	private:

		Anim::Solver m_animationSolver;

		std::vector<Transform4D> m_animatedTransforms;

	protected:

		std::vector<QueuedRenderObject> PrepareForRender() override;

		DEFINE_CLASS()
	};

	CREATE_CLASS(AnimatedMeshComponent)
}
