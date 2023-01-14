#pragma once

class System
{
	friend class Amplication;

public:

	virtual ~System() = default;

protected:

	virtual void Init() {}
	virtual void Update() {}
};
