#pragma once

#include "Components/Component.h"

class InputQueue;

struct InputListenerComponent : Component
{
	InputQueue const* m_inputQueue   = nullptr;
	bool              m_alwaysActive = false;

	DEFINE_CLASS()
};

CREATE_CLASS(InputListenerComponent)
