#pragma once

struct FrameCounter;

class SystemBase
{
public:

	virtual ~SystemBase() = default;

	virtual void Init(FrameCounter* frameCounter) { m_frameCounter = frameCounter; }
	
	virtual void OnFrameBegin() {}
	virtual void OnFrameEnd() {}
	virtual void OnUpdateStart() {}
	virtual void OnUpdateEnd() {}
	virtual void Run() = 0;

	virtual bool IsLockStepSystem() = 0;

protected:

	const FrameCounter* m_frameCounter = nullptr;
};
