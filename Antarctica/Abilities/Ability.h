#pragma once

class Entity;

class Ability
{
public:

	virtual ~Ability() = default;

	bool HasStarted() const { return m_hasStarted; }

	virtual bool Init(Entity& entity) = 0;

	bool Tick()
	{
		if (!m_hasStarted)
		{
			m_hasStarted = true;
			Start();
		}
		const bool hasFinished = Update();
		if (hasFinished)
		{
			End();
		}
		return hasFinished;
	}

	virtual void Cancel()
	{
		End();
	}

protected:

	virtual void Start() = 0;
	virtual bool Update() = 0;
	virtual void End() = 0;

	Entity* m_entity = nullptr;

private:

	bool m_hasStarted = false;
};
