#pragma once

#include "Entities/Entity.h"

namespace Rendering
{
	class Camera : public Entity
	{
	public:

		Camera();

		void DefineArchetype(ArchetypeBuilder& builder) override;

		//TODO: grant access to camera in other way (maybe variable in certain systems)
		static Camera* Get();

	private:

		static Camera* m_camera;
	};
}
