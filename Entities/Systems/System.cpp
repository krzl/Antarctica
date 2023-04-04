#include "stdafx.h"
#include "System.h"

std::vector<System*> SystemCreator::CreateSystems()
{
	std::vector<System*> systems;

	for (auto& constructor : m_systemConstructors)
	{
		systems.push_back(constructor());
	}

	return systems;
}
