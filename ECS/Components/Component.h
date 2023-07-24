#pragma once

#include "Common/Class.h"

struct Component
{
	friend class Entity;

	virtual ~Component() = default;

	Component(const Component& other)            = delete;
	Component& operator=(const Component& other) = delete;

	const Class* m_class;

protected:

	Component() = default;
};