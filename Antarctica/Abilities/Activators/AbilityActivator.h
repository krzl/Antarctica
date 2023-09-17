#pragma once

class Ability;
class Entity;

class AbilityActivator
{
public:

	virtual ~AbilityActivator() = default;

	virtual std::shared_ptr<Ability> Activate(Entity* entity) = 0;

	virtual float GetEntitySuitability(Entity* entity) { return 1.0f; }

	virtual void Update() = 0;

	virtual bool ShouldBeCancelled() = 0;
	virtual bool CanBeFinished() = 0;
	
	virtual void OnFinished() = 0;

	bool ShouldTriggerImmediately() const { return m_triggerImmediately; };
	bool ShouldActivateForAllSelected() const { return m_activateForAllSelected; }

	std::string m_abilityId;

protected:

	bool m_triggerImmediately     = false;
	bool m_activateForAllSelected = false;
};
