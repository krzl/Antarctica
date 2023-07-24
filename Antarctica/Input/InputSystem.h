#pragma once

#include "InputListener.h"
#include "InputQueue.h"
#include "Systems/System.h"

class InputSystem : public System<InputListenerComponent>
{
	void Init(FrameCounter* frameCounter) override;
	void OnUpdateStart() override;

	bool IsSelected(uint64_t entityId, const InputListenerComponent* inputListener) const;
	void Update(uint64_t entityId, InputListenerComponent* inputListener) override;

	bool IsLockStepSystem() override { return false; }
	
public:
	
	void ResetInput();

	std::set<Entity*> m_selectedEntities;

private:

	InputQueue m_inputQueue;

	std::set<uint64_t> m_selectedEntitiesId;
};
