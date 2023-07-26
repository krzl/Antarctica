#include "stdafx.h"
#include "AbilityTriggerSystem.h"

#include "Ability.h"
#include "AbilityBinding.h"
#include "AbilityStackComponent.h"
#include "AbilityTriggerComponent.h"
#include "Entities/World.h"
#include "Input/InputListener.h"
#include "Input/InputQueue.h"

void AbilityTriggerSystem::Update(const uint64_t entityId, AbilityTriggerComponent* abilityTrigger, AbilityStackComponent* abilityStack,
								  InputListenerComponent* inputListener)
{
	if (inputListener->m_inputQueue == nullptr)
	{
		return;
	}

	for (const InputCommand& inputCommand : inputListener->m_inputQueue->m_commands)
	{
		for (const AbilityBinding& abilityBinding : abilityTrigger->m_abilityBindings)
		{
			if (IsAbilityTriggered(abilityBinding, inputCommand))
			{
				Entity* entity = *World::Get()->GetEntity(entityId);

				std::shared_ptr<Ability> ability = std::static_pointer_cast<Ability>(abilityBinding.m_abilityClass->CreateObject());
				ability->Init(*entity);

				//TODO: Check if append modifier key was held
				if (!abilityTrigger->m_alwaysAppendAbilities)
				{
					abilityStack->m_stack = {};
				}
				abilityStack->m_stack.push(std::move(ability));
				return;
			}
		}
	}
}

bool AbilityTriggerSystem::IsAbilityTriggered(const AbilityBinding& abilityBinding, const InputCommand& inputCommand)
{
	if (inputCommand.m_type != abilityBinding.m_inputType)
	{
		return false;
	}

	switch (inputCommand.m_type)
	{
		case InputCommand::Type::MOUSE_PRESS:
			return (uint32_t) inputCommand.m_mousePressInput.m_button == abilityBinding.m_inputId;
		case InputCommand::Type::MOUSE_RELEASE:
			return (uint32_t) inputCommand.m_mouseReleaseInput.m_button == abilityBinding.m_inputId;
		case InputCommand::Type::MOUSE_MOVE:
			return true;
		default:
			return false;
	}
}
