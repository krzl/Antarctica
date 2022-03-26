#include "stdafx.h"
#include "Component.h"

#include "GameObject.h"

bool Component::IsEnabled() const
{
	if (GameObject* owner = *GetOwner())
	{
		return owner->IsEnabled() && m_isEnabled;
	}
	
	return false;
}

void Component::SetEnabled(const bool isEnabled)
{
	if (m_isEnabled != isEnabled)
	{
		if (GameObject* owner = *GetOwner())
		{
			if (owner->IsEnabled())
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

void Component::Init(const Ref<GameObject> owner, const Ref<Component> weakPtr)
{
	m_owner = owner;
	m_self = weakPtr;
	
	if (GameObject* ptr = *GetOwner())
	{
		ptr->OnObjectEnabled.AddListener([this](Ref<GameObject> object)
		{
			if (m_isEnabled)
			{
				OnComponentEnabled.Dispatch(m_self);
			}
		});
		ptr->OnObjectDisabled.AddListener([this](Ref<GameObject> object)
		{
			if (m_isEnabled)
			{
				OnComponentDisabled.Dispatch(m_self);
			}
		});
	}
}
