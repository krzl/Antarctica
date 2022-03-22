#include "stdafx.h"
#include "Component.h"

#include "GameObject.h"

bool Component::IsEnabled() const
{
	if (const auto ptr = GetOwner().lock())
	{
		return ptr->IsEnabled() && m_isEnabled;
	}
	
	return false;
}

void Component::SetEnabled(const bool isEnabled)
{
	if (m_isEnabled != isEnabled)
	{
		if (const auto ptr = GetOwner().lock())
		{
			if (ptr->IsEnabled())
			{
				if (m_isEnabled)
				{
					OnComponentEnabled.Dispatch(m_self);
				}
				else
				{
					OnComponentDisabled.Dispatch(m_self);
				}
			}
		}
	}
}

void Component::Init(const std::weak_ptr<GameObject> owner, const std::weak_ptr<Component> weakPtr)
{
	m_owner = owner;
	m_self = weakPtr;
	
	if (const auto ptr = GetOwner().lock())
	{
		ptr->OnObjectEnabled.AddListener([this](std::weak_ptr<GameObject> object)
		{
			if (m_isEnabled)
			{
				OnComponentEnabled.Dispatch(m_self);
			}
		});
		ptr->OnObjectDisabled.AddListener([this](std::weak_ptr<GameObject> object)
		{
			if (m_isEnabled)
			{
				OnComponentDisabled.Dispatch(m_self);
			}
		});
	}
}
