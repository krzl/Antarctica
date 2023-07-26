#pragma once

#include "InputListener.h"
#include "InputQueue.h"
#include "Systems/System.h"

class InputSystem : public System<InputListenerComponent>
{
	void Init() override;
	void OnUpdateStart() override;

	bool IsSelected(uint64_t entityId, const InputListenerComponent* inputListener) const;
	void Update(uint64_t entityId, InputListenerComponent* inputListener) override;

public:

	void ResetInput();

	std::set<Entity*> m_selectedEntities;

private:

	InputQueue m_inputQueue;

	std::set<uint64_t> m_selectedEntitiesId;
};
