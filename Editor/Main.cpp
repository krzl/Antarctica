#include "stdafx.h"

#include <GameObjects//World.h>
#include <Core/Application.h>
#include <Rendering/StaticMesh.h>

void main()
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	World world;

	world.Spawn<StaticMesh>();
}
