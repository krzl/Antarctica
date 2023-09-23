#pragma once

#include "Terrain.h"

namespace Navigation
{
	class TerrainLoader
	{
	public:

		static std::unique_ptr<Terrain> Load(const std::string& path);
	};
}
