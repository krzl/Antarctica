#pragma once

#include "Entities/Entity.h"

namespace Rendering
{
	class Camera : public Entity
	{
	public:

		Camera();

		void DefineArchetype(ArchetypeBuilder& builder) override;

		//TODO: ECS: grant access to camera in other way (maybe variable in certain systems) (also remove all managers and do it like this?)
		static Camera* Get();

	private:

		static Camera* m_camera;
	};
}
