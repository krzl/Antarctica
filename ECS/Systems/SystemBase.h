#pragma once
#include "ECS.h"

class FrameCounter;

class SystemBase
{
	friend class Application;

public:

	virtual ~SystemBase() = default;

	virtual void Init() {}

	virtual void OnFrameStart() {}
	virtual void OnFrameEnd() {}
	virtual void OnStepLockStart() {}
	virtual void OnStepLockEnd() {}
	virtual void OnUpdateStart() {}
	virtual void OnUpdateEnd() {}
	virtual void Run() = 0;

protected:

	const FrameCounter* m_frameCounter = nullptr;
	ECS* m_ecs;

	bool m_isMultiThreaded = true;
};
