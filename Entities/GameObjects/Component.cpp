#include "stdafx.h"
#include "Component.h"

#include "GameObject.h"

bool Component::IsEnabled() const
{
	if (GetOwner().IsValid())
	{
		return GetOwner()->IsEnabled() && m_isEnabled;
	}

	return false;
}

void Component::SetEnabled(const bool isEnabled)
{
	if (m_isEnabled != isEnabled)
	{
		if (GetOwner().IsValid())
		{
			if (GetOwner()->IsEnabled())
			{
				if (m_isEnabled)
				{
					OnEnabled();
					OnComponentEnabled.Dispatch(GetRef());
				}
				else
				{
					OnDisabled();
					OnComponentDisabled.Dispatch(GetRef());
				}
			}
		}
	}
}

void Component::Init(const Ref<GameObject> owner, const Ref<Component> self)
{
	m_owner = owner;
	m_self  = self;

	if (GameObject* ptr = *GetOwner())
	{
		ptr->OnObjectEnabled.AddListener([this](Ref<GameObject> object)
		{
			if (m_isEnabled)
			{
				OnEnabled();
				OnComponentEnabled.Dispatch(GetRef());
			}
		}, m_self);

		ptr->OnObjectDisabled.AddListener([this](Ref<GameObject> object)
		{
			if (m_isEnabled)
			{
				OnDisabled();
				OnComponentDisabled.Dispatch(GetRef());
			}
		}, m_self);

		ptr->OnObjectDestroyed.AddListener([this](Ref<GameObject> object)
		{
			if (IsEnabled())
			{
				OnDisabled();
				OnComponentDisabled.Dispatch(GetRef());
			}
			OnDestroy();
			OnComponentDestroyed.Dispatch(GetRef());
		}, m_self);
	}

	OnCreated();

	if (IsEnabled())
	{
		OnEnabled();
		OnComponentEnabled.Dispatch(GetRef());
	}
}
