#include "stdafx.h"
#include "System.h"

struct SystemPrioritySorter
{
	bool operator()(const System* a, const System* b) const
	{
		return a->GetPriority() > b->GetPriority();
	}
};

std::vector<System*> SystemCreator::CreateSystems()
{
	std::vector<System*> systems;

	for (auto& constructor : m_systemConstructors)
	{
		systems.push_back(constructor());
	}

	std::sort(systems.begin(), systems.end(), SystemPrioritySorter());

	return systems;
}
