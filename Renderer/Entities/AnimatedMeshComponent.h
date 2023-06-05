#pragma once

#include "StaticMeshComponent.h"
#include "Buffers/DynamicBuffer.h"
#include "Solver/Solver.h"

typedef std::unique_ptr<Renderer::IBuffer, void(*)(Renderer::IBuffer*)> NativeBufferPtr;

namespace Renderer
{
	struct SkinningData
	{
		std::vector<Matrix4D> m_boneTransforms;
		NativeBufferPtr&      m_weightsBuffer;
		DynamicBuffer&        m_transformBuffer;
		DynamicBuffer&        m_outputBuffer;
		const Submesh&        m_submesh;
		uint32_t              m_offset;
	};

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

		static std::vector<SkinningData> GetAllSkinningData();

	protected:

		std::shared_ptr<Anim::Animator> m_animator;

		Transform4D GetAttachedNodeTransform(int32_t nodeId, bool ignoreAttachmentRotation) override;

	private:

		Anim::Solver m_animationSolver;

		std::vector<Transform4D> m_animatedTransforms;

		NativeBufferPtr            m_weightsBuffer = NativeBufferPtr(nullptr, Renderer::Deleter);
		DynamicBuffer              m_transformBuffer;
		std::vector<DynamicBuffer> m_skinningOutputBuffers;

	protected:

		std::vector<RenderHandle> PrepareForRender() override;

		DEFINE_CLASS()
	};

	CREATE_CLASS(AnimatedMeshComponent)
}
