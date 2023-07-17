#include "stdafx.h"
#include "Manager.h"

struct ManagerPrioritySorter
{
	bool operator()(const Manager* a, const Manager* b) const
	{
		return a->GetPriority() > b->GetPriority();
	}
};

std::vector<Manager*> ManagerCreator::CreateManagers()
{
	std::vector<Manager*> managers;

	for (auto& constructor : m_managerConstructors)
	{
		managers.push_back(constructor());
	}

	std::sort(managers.begin(), managers.end(), ManagerPrioritySorter());

	return managers;
}
