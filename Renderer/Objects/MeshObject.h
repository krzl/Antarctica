#pragma once

#include "../Renderer/Objects/SubmeshObject.h"

namespace Renderer
{
	class MeshObject
	{
	public:

		void Init(const std::vector<Submesh>& submeshes);

		void Bind(uint32_t submeshIndex);

		[[nodiscard]] const std::vector<SubmeshObject>& GetSubmeshObjects() const
		{
			return m_submeshObjects;
		}

		[[nodiscard]] std::vector<SubmeshObject>& GetSubmeshObjects()
		{
			return m_submeshObjects;
		}

	private:

		std::vector<SubmeshObject> m_submeshObjects;
	};
}
