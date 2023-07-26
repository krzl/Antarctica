#pragma once

struct FrameCounter;

class SystemBase
{
public:

	virtual ~SystemBase() = default;

	virtual void Init() {}
	
	virtual void OnFrameBegin() {}
	virtual void OnFrameEnd() {}
	virtual void OnStepLockBegin() {}
	virtual void OnStepLockEnd() {}
	virtual void OnUpdateStart() {}
	virtual void OnUpdateEnd() {}
	virtual void Run() = 0;
};
