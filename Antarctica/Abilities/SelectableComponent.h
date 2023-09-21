#pragma once

#include "Components/Component.h"

struct SelectableComponent : Component
{
	bool m_isSelected = false;

	DEFINE_CLASS()
};

CREATE_CLASS(SelectableComponent)
