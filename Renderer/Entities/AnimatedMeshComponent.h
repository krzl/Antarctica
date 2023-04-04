#pragma once

#include "StaticMeshComponent.h"
#include "Buffers/DynamicBuffer.h"
#include "Solver/Solver.h"

typedef std::unique_ptr<Renderer::IBuffer, void(*)(Renderer::IBuffer*)> NativeBufferPtr;

namespace Renderer
{
	struct SkinningData
	{
		std::vector<Matrix4D>       m_boneTransforms;
		NativeBufferPtr&            m_weightsBuffer;
		DynamicBuffer&              m_transformBuffer;
		std::vector<DynamicBuffer>& m_outputBuffers;
		const Submesh&              m_submesh;
		uint32_t                    m_offset;
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

		void SetAnimator(const std::shared_ptr<Anim::Animator> animator);

		static std::vector<SkinningData> GetAllSkinningData();

	protected:

		std::shared_ptr<Anim::Animator> m_animator;

	private:

		Anim::Solver m_animationSolver;

		NativeBufferPtr                         m_weightsBuffer = NativeBufferPtr(nullptr, Renderer::Deleter);
		DynamicBuffer                           m_transformBuffer;
		std::vector<std::vector<DynamicBuffer>> m_skinningOutputBuffers;

	protected:

		std::vector<RenderHandle> PrepareForRender() override;

	private:

		DEFINE_CLASS()
	};

	CREATE_CLASS(AnimatedMeshComponent)
}
