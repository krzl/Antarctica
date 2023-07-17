#pragma once

namespace Collision
{
	enum class CollisionChannel
	{
		DEFAULT    = 1 << 0,
		CHARACTERS = 1 << 1,
		STRUCTURES = 1 << 2,
		TERRAIN    = 1 << 3
	};
}
