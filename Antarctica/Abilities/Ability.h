#pragma once

#include "Common/Class.h"

class Entity;

class Ability
{
	friend class Class;
	
public:

	virtual ~Ability() = default;

	bool HasStarted() const { return m_hasStarted; }

	virtual void Init(Entity& entity) = 0;

	bool Run(Entity& entity)
	{
		if (!m_hasStarted)
		{
			m_hasStarted = true;
			Start(entity);
		}
		const bool hasFinished = Update(entity);
		if (hasFinished)
		{
			End(entity);
		}
		return hasFinished;
	}

protected:

	virtual void Start(Entity& entity) = 0;
	virtual bool Update(Entity& entity) = 0;
	virtual void End(Entity& entity) = 0;
	
private:

	bool m_hasStarted = false;
	const Class* m_class;
};
