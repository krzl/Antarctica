#include "stdafx.h"
#include "InputSystem.h"

#include "Entities/Entity.h"
#include "Input/InputListener.h"
#include "Input/InputManager.h"

void InputSystem::Init()
{
	System::Init();
	
	InputManager* inputManager = InputManager::GetInstance();

	inputManager->OnLeftMouseButtonPressed.AddListener([this]()
	{
		m_inputQueue.AddMousePressCommand(InputCommand::MouseButtonId::LEFT, true);
	});

	inputManager->OnMiddleMouseButtonPressed.AddListener([this]()
	{
		m_inputQueue.AddMousePressCommand(InputCommand::MouseButtonId::MIDDLE, true);
	});

	inputManager->OnRightMouseButtonPressed.AddListener([this]()
	{
		m_inputQueue.AddMousePressCommand(InputCommand::MouseButtonId::RIGHT, true);
	});

	inputManager->OnLeftMouseButtonReleased.AddListener([this]()
	{
		m_inputQueue.AddMousePressCommand(InputCommand::MouseButtonId::LEFT, false);
	});

	inputManager->OnMiddleMouseButtonReleased.AddListener([this]()
	{
		m_inputQueue.AddMousePressCommand(InputCommand::MouseButtonId::MIDDLE, false);
	});

	inputManager->OnRightMouseButtonReleased.AddListener([this]()
	{
		m_inputQueue.AddMousePressCommand(InputCommand::MouseButtonId::RIGHT, false);
	});
}

void InputSystem::OnUpdateStart()
{
	m_inputQueue.TryAddMouseMoveCommand();

	m_selectedEntitiesId.clear();
	for (const Entity* entity : m_selectedEntities)
	{
		m_selectedEntitiesId.insert(entity->GetInstanceId());
	}
}

bool InputSystem::IsSelected(const uint64_t entityId, const InputListenerComponent* inputListener) const
{
	return inputListener->m_alwaysActive || m_selectedEntitiesId.find(entityId) != m_selectedEntitiesId.end();
}

void InputSystem::Update(const uint64_t entityId, InputListenerComponent* inputListener)
{
	inputListener->m_inputQueue = IsSelected(entityId, inputListener) ? &m_inputQueue : nullptr;
}

void InputSystem::ResetInput()
{
	m_inputQueue.Clear();
}
