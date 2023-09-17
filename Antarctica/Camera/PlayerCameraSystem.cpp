#include "stdafx.h"
#include "PlayerCameraSystem.h"

#include <complex>

#include "CameraData.h"

#include "Abilities/Ability.h"
#include "Abilities/AbilityBinding.h"
#include "Abilities/AbilityStackComponent.h"
#include "Abilities/AbilityTriggerComponent.h"
#include "Abilities/Activators/AbilityActivator.h"

#include "Assets/BVH.h"

#include "Buffers/Types/PerCameraBuffer.h"
#include "Camera/CameraScrollComponent.h"
#include "Components/CameraComponent.h"
#include "Components/TransformComponent.h"
#include "Core/Application.h"

#include "Input/InputQueue.h"

void PlayerCameraSystem::Init()
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

void PlayerCameraSystem::OnUpdateStart()
{
	m_aspectRatio = Application::Get().GetWindow().GetAspectRatio();
	m_cameras.clear();

	m_inputQueue.TryAddMouseMoveCommand();
}

void PlayerCameraSystem::Update(uint64_t entityId, TransformComponent* transform, Rendering::CameraComponent* camera,
								CameraScrollComponent* cameraScroll)
{
	if (m_inputQueue.GetMouseButtonPress(InputCommand::MouseButtonId::MIDDLE))
	{
		cameraScroll->m_isDragging = true;
	}
	else
	{
		if (m_inputQueue.GetMouseButtonRelease(InputCommand::MouseButtonId::MIDDLE))
		{
			cameraScroll->m_isDragging = false;
		}
	}

	if (cameraScroll->m_isDragging)
	{
		if (const InputCommand::MouseMoveInput* mouseMove = m_inputQueue.GetMouseMove())
		{
			const Vector3D positionDelta = Vector3D(-mouseMove->m_deltaX * cameraScroll->m_cameraSpeed,
				mouseMove->m_deltaY * cameraScroll->m_cameraSpeed, 0.0f);
			transform->m_localPosition += positionDelta;
		}
	}

	camera->m_viewMatrix        = Inverse(Transform4D::MakeTranslation(transform->m_localPosition) * transform->m_localRotation.GetRotationMatrix());
	camera->m_perspectiveMatrix = GetPerspectiveMatrix(camera);

	camera->m_frustum = GetFrustum(camera);

	const Matrix4D viewProj = camera->m_perspectiveMatrix * camera->m_viewMatrix;

	Rendering::PerCameraBuffer buffer = Rendering::PerCameraBuffer::DEFAULT_BUFFER;
	buffer.m_viewProjMatrix           = viewProj.transpose;
	buffer.m_projMatrix               = camera->m_perspectiveMatrix.transpose;

	//TODO: add lock so m_cameras is not changed on multiple threads
	m_cameras.emplace_back(Rendering::CameraData{
		buffer,
		camera->m_order
	});


	const Point2DInt pos = InputManager::GetInstance()->GetMousePosition();

	const float ndcX = (2.0f * pos.x) / Application::Get().GetWindow().GetWidth() - 1.0f;
	const float ndcY = 1.0f - (2.0f * pos.y) / Application::Get().GetWindow().GetHeight();

	const Vector4D clipCoords = Vector4D(ndcX, ndcY, 1.0f, 1.0f);

	Matrix4D inversePerspectiveMatrix = Inverse(camera->m_perspectiveMatrix);
	Vector4D eyeCoordinates           = inversePerspectiveMatrix * clipCoords;
	eyeCoordinates /= eyeCoordinates.w;

	Matrix4D inverseViewMatrix = Inverse(camera->m_viewMatrix);
	Vector4D rayWorld          = inverseViewMatrix * eyeCoordinates;

	Vector3D direction = rayWorld.xyz - transform->m_localPosition;
	direction          = direction.Normalize();

	const Ray ray = { transform->m_localPosition, direction };

	m_cursorWorldPosition = m_terrainBvh->Intersect(ray);

	//TODO: handle selection of entities

	if (m_selectionGroupEntity != nullptr && m_abilityActivator == nullptr)
	{
		TryTriggerAbilitiesFromSelection();
	}

	if (m_abilityActivator != nullptr)
	{
		m_abilityActivator->Update();

		if (m_abilityActivator->ShouldBeCancelled())
		{
			m_abilityActivator->OnFinished();
			m_abilityActivator.reset();
		}
		else
		{
			if (m_inputQueue.GetMouseButtonPress(InputCommand::MouseButtonId::LEFT))
			{
				if (const bool activateAbility = m_abilityActivator->CanBeFinished())
				{
					if (Entity* suitableEntity = FindSuitableEntityForAbility(m_abilityActivator->m_abilityId))
					{
						ActivateAbility(suitableEntity, m_abilityActivator->m_abilityId);
					}
					else
					{
						m_abilityActivator->OnFinished();
						m_abilityActivator.reset();
					}
				}
				else
				{
					m_abilityActivator->OnFinished();
					m_abilityActivator.reset();
				}
			}
		}
	}
}

Matrix4D PlayerCameraSystem::GetPerspectiveMatrix(const Rendering::CameraComponent* camera) const
{
	const float tan = Terathon::Tan(0.5f * DegToRad(camera->m_fov));

	const float a = (camera->m_farZ + camera->m_nearZ) / (camera->m_nearZ - camera->m_farZ);
	const float b = (2.0f * camera->m_farZ * camera->m_nearZ) / (camera->m_nearZ - camera->m_farZ);

	return Matrix4D(
		1.0f / (tan * m_aspectRatio), 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f / tan, 0, 0.0f,
		0.0f, 0.0f, a, b,
		0.0f, 0.0f, -1.0f, 0.0f
	);
}

Frustum PlayerCameraSystem::GetFrustum(Rendering::CameraComponent* camera) const
{
	Matrix4D matrix = Inverse(camera->m_perspectiveMatrix * camera->m_viewMatrix);

	Vector4D pointFearTopLeft  = matrix * Point3D(-1.0f, 1.0f, -1.0f);
	Vector4D pointFearTopRight = matrix * Point3D(1.0f, 1.0f, -1.0f);
	Vector4D pointFearBotLeft  = matrix * Point3D(-1.0f, -1.0f, -1.0f);
	Vector4D pointFearBotRight = matrix * Point3D(1.0f, -1.0f, -1.0f);
	Vector4D pointFarTopLeft   = matrix * Point3D(-1.0f, 1.0f, 1.0f);
	Vector4D pointFarTopRight  = matrix * Point3D(1.0f, 1.0f, 1.0f);
	Vector4D pointFarBotLeft   = matrix * Point3D(-1.0f, -1.0f, 1.0f);
	Vector4D pointFarBotRight  = matrix * Point3D(1.0f, -1.0f, 1.0f);


	Point3D nearTopLeft  = (Point3D) (pointFearTopLeft / pointFearTopLeft.w).xyz;
	Point3D nearTopRight = (Point3D) (pointFearTopRight / pointFearTopRight.w).xyz;
	Point3D nearBotLeft  = (Point3D) (pointFearBotLeft / pointFearBotLeft.w).xyz;
	Point3D nearBotRight = (Point3D) (pointFearBotRight / pointFearBotRight.w).xyz;

	Point3D farTopLeft  = (Point3D) (pointFarTopLeft / pointFarTopLeft.w).xyz;
	Point3D farTopRight = (Point3D) (pointFarTopRight / pointFarTopRight.w).xyz;
	Point3D farBotLeft  = (Point3D) (pointFarBotLeft / pointFarBotLeft.w).xyz;
	Point3D farBotRight = (Point3D) (pointFarBotRight / pointFarBotRight.w).xyz;

	Point3D* frustumPoints[6][3] = {
		{ &nearTopRight, &nearTopLeft, &nearBotLeft },  // near plane
		{ &nearTopLeft, &farTopLeft, &farBotLeft },     // left plane
		{ &farTopRight, &nearTopRight, &nearBotRight }, // right plane
		{ &farTopRight, &farTopLeft, &nearTopLeft },    // top plane
		{ &nearBotRight, &nearBotLeft, &farBotLeft },   // bot plane
		{ &farTopLeft, &farTopRight, &farBotRight }     // far plane
	};

	Frustum frustum;

	for (uint32_t i = 0; i < 6; ++i)
	{
		frustum.m_planes[i].m_direction = -Cross(*frustumPoints[i][0] - *frustumPoints[i][1], *frustumPoints[i][2] - *frustumPoints[i][1]);
		frustum.m_planes[i].m_direction = Normalize(frustum.m_planes[i].m_direction);
		frustum.m_planes[i].m_distance  = Dot(*frustumPoints[i][0], -frustum.m_planes[i].m_direction);
	}

	return frustum;
}

std::vector<Rendering::CameraData>& PlayerCameraSystem::GetCameras()
{
	return m_cameras;
}

void PlayerCameraSystem::AddToSelection(Entity* entity)
{
	if (m_selectedEntities.size() == 0)
	{
		m_selectionGroupEntity = entity;
	}

	m_selectedEntities.emplace(entity);
}

void PlayerCameraSystem::SetupTerrainBvh(const std::shared_ptr<Mesh> terrain)
{
	m_terrainBvh = std::make_shared<BVH>();
	m_terrainBvh->Init(terrain);
}

void PlayerCameraSystem::OnUpdateEnd()
{
	m_inputQueue.Clear();
}

void PlayerCameraSystem::TryTriggerAbilitiesFromSelection()
{
	const ComponentAccessor& accessor             = m_selectionGroupEntity->GetComponentAccessor();
	const AbilityTriggerComponent* abilityTrigger = accessor.GetComponent<AbilityTriggerComponent>();
	if (abilityTrigger)
	{
		for (const InputCommand& inputCommand : m_inputQueue.m_commands)
		{
			for (const AbilityBinding& abilityBinding : abilityTrigger->m_abilityBindings)
			{
				if (IsAbilityInputPressed(abilityBinding, inputCommand))
				{
					m_abilityActivator              = abilityBinding.m_activatorCreator();
					m_abilityActivator->m_abilityId = abilityBinding.m_abilityId;

					Entity* suitableEntity = FindSuitableEntityForAbility(abilityBinding.m_abilityId);
					if (!suitableEntity)
					{
						m_abilityActivator.reset();
						continue;
					}

					if (m_abilityActivator->ShouldTriggerImmediately())
					{
						ActivateAbility(suitableEntity, abilityBinding.m_abilityId);
					}
				}
			}
		}
	}
}

bool PlayerCameraSystem::IsAbilityInputPressed(const AbilityBinding& abilityBinding, const InputCommand& inputCommand)
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

bool PlayerCameraSystem::CanEntityActivateAbility(const AbilityTriggerComponent* abilityTrigger, const std::string& abilityId)
{
	if (abilityTrigger)
	{
		for (const AbilityBinding& binding : abilityTrigger->m_abilityBindings)
		{
			if (binding.m_abilityId == abilityId)
			{
				return true;
			}
		}
	}

	return false;
}

Entity* PlayerCameraSystem::FindSuitableEntityForAbility(const std::string& abilityId) const
{
	if (m_abilityActivator->ShouldActivateForAllSelected())
	{
		for (Entity* entity : m_selectedEntities)
		{
			const ComponentAccessor& accessor             = entity->GetComponentAccessor();
			const AbilityTriggerComponent* abilityTrigger = accessor.GetComponent<AbilityTriggerComponent>();

			if (CanEntityActivateAbility(abilityTrigger, abilityId) && m_abilityActivator->GetEntitySuitability(entity) >= 0.0f)
			{
				return entity;
			}
		}
	}
	else
	{
		float bestSuitability      = -1.0f;
		Entity* mostSuitableEntity = nullptr;

		for (Entity* selectedEntity : m_selectedEntities)
		{
			const ComponentAccessor& accessor             = selectedEntity->GetComponentAccessor();
			const AbilityTriggerComponent* abilityTrigger = accessor.GetComponent<AbilityTriggerComponent>();

			if (CanEntityActivateAbility(abilityTrigger, abilityId))
			{
				const float suitability = m_abilityActivator->GetEntitySuitability(selectedEntity);
				if (suitability >= 0.0f && (suitability < bestSuitability || mostSuitableEntity == nullptr))
				{
					bestSuitability    = suitability;
					mostSuitableEntity = selectedEntity;
				}
			}
		}

		return mostSuitableEntity;
	}

	return nullptr;
}

void PlayerCameraSystem::ActivateAbility(Entity* entity, const std::string& abilityId)
{
	if (m_abilityActivator->ShouldActivateForAllSelected())
	{
		for (Entity* selectedEntity : m_selectedEntities)
		{
			const ComponentAccessor& accessor             = entity->GetComponentAccessor();
			const AbilityTriggerComponent* abilityTrigger = accessor.GetComponent<AbilityTriggerComponent>();

			if (CanEntityActivateAbility(abilityTrigger, abilityId) && m_abilityActivator->GetEntitySuitability(selectedEntity) < 0.0f)
			{
				continue;
			}

			AbilityStackComponent* abilityStack = accessor.GetComponent<AbilityStackComponent>();

			if (abilityStack)
			{
				continue;
			}

			const std::shared_ptr<Ability> ability = m_abilityActivator->Activate(selectedEntity);
			ability->Tick();
			AddAbilityToStack(abilityStack, ability);
		}
	}
	else
	{
		const ComponentAccessor& accessor   = entity->GetComponentAccessor();
		AbilityStackComponent* abilityStack = accessor.GetComponent<AbilityStackComponent>();

		const std::shared_ptr<Ability> ability = m_abilityActivator->Activate(entity);
		ability->Tick();
		AddAbilityToStack(abilityStack, ability);
	}

	m_abilityActivator->OnFinished();

	m_abilityActivator.reset();
}

void PlayerCameraSystem::AddAbilityToStack(AbilityStackComponent* abilityStack, std::shared_ptr<Ability> ability) const
{
	//TODO: Check if append modifier key was held
	if constexpr (true)
	{
		while (!abilityStack->m_stack.empty())
		{
			const std::shared_ptr<Ability> oldAbility = abilityStack->m_stack.front();
			abilityStack->m_stack.pop();

			oldAbility->Cancel();
		}
	}
	abilityStack->m_stack.push(std::move(ability));
}
