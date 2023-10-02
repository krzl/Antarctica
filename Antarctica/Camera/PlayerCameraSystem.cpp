#include "stdafx.h"
#include "PlayerCameraSystem.h"

#include "AssetManager.h"
#include "CameraData.h"

#include "Abilities/Ability.h"
#include "Abilities/AbilityBinding.h"
#include "Abilities/AbilityStackComponent.h"
#include "Abilities/AbilityTriggerComponent.h"
#include "Abilities/SelectableComponent.h"
#include "Abilities/Activators/AbilityActivator.h"

#include "Assets/BVH.h"
#include "Assets/DynamicMesh.h"

#include "Buffers/Types/PerCameraBuffer.h"
#include "Camera/CameraScrollComponent.h"
#include "Components/CameraComponent.h"
#include "Components/RenderCullComponent.h"
#include "Components/TransformComponent.h"
#include "Core/Application.h"

#include "Entities/DynamicMeshEntity.h"

#include "Input/InputQueue.h"

void PlayerCameraSystem::Init()
{
	System::Init();

	InputManager* inputManager = InputManager::GetInstance();

	inputManager->OnLeftMouseButtonPressed.AddListener([this]
	{
		m_inputQueue.AddMousePressCommand(InputCommand::MouseButtonId::LEFT, true);
	});

	inputManager->OnMiddleMouseButtonPressed.AddListener([this]
	{
		m_inputQueue.AddMousePressCommand(InputCommand::MouseButtonId::MIDDLE, true);
	});

	inputManager->OnRightMouseButtonPressed.AddListener([this]
	{
		m_inputQueue.AddMousePressCommand(InputCommand::MouseButtonId::RIGHT, true);
	});

	inputManager->OnLeftMouseButtonReleased.AddListener([this]
	{
		m_inputQueue.AddMousePressCommand(InputCommand::MouseButtonId::LEFT, false);
	});

	inputManager->OnMiddleMouseButtonReleased.AddListener([this]
	{
		m_inputQueue.AddMousePressCommand(InputCommand::MouseButtonId::MIDDLE, false);
	});

	inputManager->OnRightMouseButtonReleased.AddListener([this]
	{
		m_inputQueue.AddMousePressCommand(InputCommand::MouseButtonId::RIGHT, false);
	});

	inputManager->OnKeyPressed.AddListener([this](const Key key)
	{
		m_inputQueue.AddKeyPressCommand(key, true);
	});

	inputManager->OnKeyReleased.AddListener([this](const Key key)
	{
		m_inputQueue.AddKeyPressCommand(key, false);
	});
}

void PlayerCameraSystem::OnUpdateStart()
{
	m_aspectRatio = Application::Get().GetWindow().GetAspectRatio();
	m_cameras.clear();

	m_inputQueue.TryAddMouseMoveCommand();
}

void PlayerCameraSystem::Update(uint64_t entityId, TransformComponent* transform, Rendering::CameraComponent* camera,
								CameraScrollComponent* cameraScroll, CameraDragSelectComponent* cameraDrag)
{
	if (m_inputQueue.GetMouseButtonPress(InputCommand::MouseButtonId::MIDDLE) && !cameraDrag->m_isDragging)
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

	camera->m_frustum = GetFrustum(camera, Rect{ { 0, 0 }, { 1, 1 } });

	const Matrix4D viewProj = camera->m_perspectiveMatrix * camera->m_viewMatrix;

	Rendering::PerCameraBuffer buffer = Rendering::PerCameraBuffer::DEFAULT_BUFFER;
	buffer.m_viewProjMatrix           = viewProj.transpose;
	buffer.m_projMatrix               = camera->m_perspectiveMatrix.transpose;

	//TODO: add lock so m_cameras is not changed on multiple threads
	m_cameras.emplace_back(Rendering::CameraData{
		buffer,
		camera->m_order
	});

	const Ray ray = GetCameraRay(transform, camera);

	m_cursorWorldPosition = m_terrainBvh->Intersect(ray);

	if (m_abilityActivator == nullptr && !cameraScroll->m_isDragging)
	{
		if (cameraDrag->m_isDragging)
		{
			if (m_inputQueue.GetMouseButtonRelease(InputCommand::MouseButtonId::LEFT))
			{
				OnDragEnd(transform, camera, cameraDrag);
			}
			else
			{
				UpdateDragIndicator(transform, camera, cameraDrag);
			}
		}
		else if (m_inputQueue.GetMouseButtonPress(InputCommand::MouseButtonId::LEFT))
		{
			if (!cameraDrag->m_dragIndicator.IsValid())
			{
				CreateDragIndicator(cameraDrag);
			}
			else
			{
				cameraDrag->m_dragIndicator->GetComponentAccessor().GetComponent<Rendering::RenderComponent>()->m_isEnabled = true;
			}

			cameraDrag->m_isDragging        = true;
			cameraDrag->m_dragStartPosition = InputManager::GetInstance()->GetMousePosition();
			UpdateDragIndicator(transform, camera, cameraDrag);
		}
	}

	if (m_selectionGroupEntity != nullptr && m_abilityActivator == nullptr && !cameraDrag->m_isDragging)
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
				if (m_abilityActivator->CanBeFinished())
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

Frustum PlayerCameraSystem::GetFrustum(Rendering::CameraComponent* camera, Rect sectionRect) const
{
	Matrix4D matrix = Inverse(camera->m_perspectiveMatrix * camera->m_viewMatrix);

	sectionRect.m_lowerBoundary.x = sectionRect.m_lowerBoundary.x * 2.0f - 1.0f;
	sectionRect.m_lowerBoundary.y = sectionRect.m_lowerBoundary.y * 2.0f - 1.0f;
	sectionRect.m_upperBoundary.x = sectionRect.m_upperBoundary.x * 2.0f - 1.0f;
	sectionRect.m_upperBoundary.y = sectionRect.m_upperBoundary.y * 2.0f - 1.0f;

	Vector4D pointFearTopLeft  = matrix * Point3D(sectionRect.m_lowerBoundary.x, sectionRect.m_upperBoundary.y, -1.0f);
	Vector4D pointFearTopRight = matrix * Point3D(sectionRect.m_upperBoundary.x, sectionRect.m_upperBoundary.y, -1.0f);
	Vector4D pointFearBotLeft  = matrix * Point3D(sectionRect.m_lowerBoundary.x, sectionRect.m_lowerBoundary.y, -1.0f);
	Vector4D pointFearBotRight = matrix * Point3D(sectionRect.m_upperBoundary.x, sectionRect.m_lowerBoundary.y, -1.0f);
	Vector4D pointFarTopLeft   = matrix * Point3D(sectionRect.m_lowerBoundary.x, sectionRect.m_upperBoundary.y, 1.0f);
	Vector4D pointFarTopRight  = matrix * Point3D(sectionRect.m_upperBoundary.x, sectionRect.m_upperBoundary.y, 1.0f);
	Vector4D pointFarBotLeft   = matrix * Point3D(sectionRect.m_lowerBoundary.x, sectionRect.m_lowerBoundary.y, 1.0f);
	Vector4D pointFarBotRight  = matrix * Point3D(sectionRect.m_upperBoundary.x, sectionRect.m_lowerBoundary.y, 1.0f);


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

Ray PlayerCameraSystem::GetCameraRay(TransformComponent* transform, Rendering::CameraComponent* camera) const
{
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

	return Ray{ transform->m_localPosition, direction };
}

std::vector<Entity*> PlayerCameraSystem::GetEntitiesFromScreenArea(TransformComponent* cameraTransform, Rendering::CameraComponent* camera,
																   const Point2DInt startPos, const Point2DInt endPos) const
{
	const float w = Application::Get().GetWindow().GetWidth();
	const float h = Application::Get().GetWindow().GetHeight();

	if (endPos.x - startPos.x > 3 && endPos.y - startPos.y > 3)
	{
		const Frustum frustum = GetFrustum(camera, Rect{ { startPos.x / w, 1.0f - endPos.y / h }, { endPos.x / w, 1.0f - startPos.y / h } });

		return Application::Get().GetWorld().GetQuadtree().Intersect(frustum);
	}

	Quadtree::TraceResult result = Application::Get().GetWorld().GetQuadtree().TraceObject(
		RayIntersectionTester(GetCameraRay(cameraTransform, camera)));
	if (result.m_object.IsValid())
	{
		return { *result.m_object };
	}

	return {};
}

std::vector<Rendering::CameraData>& PlayerCameraSystem::GetCameras()
{
	return m_cameras;
}

void PlayerCameraSystem::AddToSelection(Entity* entity)
{
	const SelectableComponent* selectable = entity->GetComponentAccessor().GetComponent<SelectableComponent>();

	if (selectable == nullptr || selectable->m_isSelected)
	{
		return;
	}

	if (m_selectedEntities.size() == 0)
	{
		m_selectionGroupEntity = entity;
	}

	m_selectedEntities.emplace(entity);
}

void PlayerCameraSystem::ClearSelection()
{
	for (Entity* entity : m_selectedEntities)
	{
		entity->GetComponentAccessor().GetComponent<SelectableComponent>()->m_isSelected = false;
	}

	m_selectionGroupEntity = nullptr;
	m_selectedEntities.clear();
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
						m_abilityActivator->Update();
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
		case InputCommand::Type::KEY_PRESS:
			return (uint32_t) inputCommand.m_keyPressInput.m_button == abilityBinding.m_inputId;
		case InputCommand::Type::KEY_RELEASE:
			return (uint32_t) inputCommand.m_keyReleaseInput.m_button == abilityBinding.m_inputId;
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
			const ComponentAccessor& accessor             = selectedEntity->GetComponentAccessor();
			const AbilityTriggerComponent* abilityTrigger = accessor.GetComponent<AbilityTriggerComponent>();

			if (CanEntityActivateAbility(abilityTrigger, abilityId) && m_abilityActivator->GetEntitySuitability(selectedEntity) < 0.0f)
			{
				continue;
			}

			AbilityStackComponent* abilityStack = accessor.GetComponent<AbilityStackComponent>();

			if (!abilityStack)
			{
				continue;
			}

			const std::shared_ptr<Ability> ability = m_abilityActivator->Activate(selectedEntity);
			AddAbilityToStack(abilityStack, ability, selectedEntity);
		}
	}
	else
	{
		const ComponentAccessor& accessor   = entity->GetComponentAccessor();
		AbilityStackComponent* abilityStack = accessor.GetComponent<AbilityStackComponent>();

		const std::shared_ptr<Ability> ability = m_abilityActivator->Activate(entity);
		AddAbilityToStack(abilityStack, ability, entity);
	}

	m_abilityActivator->OnFinished();

	m_abilityActivator.reset();
}

void PlayerCameraSystem::AddAbilityToStack(AbilityStackComponent* abilityStack, std::shared_ptr<Ability> ability, Entity* entity) const
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
	ability->Init(*entity);
	ability->Tick();
	abilityStack->m_stack.push(std::move(ability));
}

void PlayerCameraSystem::CreateDragIndicator(CameraDragSelectComponent* cameraDrag) const
{
	std::shared_ptr<DynamicMesh> dynamicMesh = std::make_shared<DynamicMesh>();

	cameraDrag->m_dragIndicator                     = World::Get()->Spawn<Rendering::DynamicMeshEntity>(SpawnParams{}, dynamicMesh);
	Rendering::DynamicMeshEntity* dynamicMeshEntity = *cameraDrag->m_dragIndicator;

	std::shared_ptr<Shader> shader                 = AssetManager::GetAsset<Shader>("../Resources/Shaders/drag_indicator.hlsl");
	std::shared_ptr<Material> material             = std::make_shared<Material>(shader);
	material->GetShaderParams().m_depthTestEnabled = true;
	material->GetShaderParams().m_blendingEnabled  = true;
	material->SetOrder(4000); //TODO: create enum for sorting order

	Rendering::MeshComponent* meshComponent = dynamicMeshEntity->GetComponentAccessor().GetComponent<Rendering::MeshComponent>();
	dynamicMeshEntity->GetComponentAccessor().GetComponent<Rendering::RenderCullComponent>()->m_neverCull = true;

	meshComponent->m_materials = { material };

	dynamicMesh->SetSubmeshCount(1);

	Submesh& submesh = dynamicMesh->GetSubmesh(0);
	submesh.SetDynamic();
	submesh.SetAttributesUsage({
		false,
		false,
		false,
		1,
		0,
		0,
		0,
		0
	});

	MeshBuffer& vertexBuffer   = submesh.GetVertexBuffer();
	vertexBuffer.m_elementSize = sizeof(Point3D) + sizeof(Color);
	vertexBuffer.m_data.resize(vertexBuffer.m_elementSize * 20);

	MeshBuffer& indexBuffer   = submesh.GetIndexBuffer();
	indexBuffer.m_elementSize = sizeof(uint32_t);
	indexBuffer.m_data.resize(indexBuffer.m_elementSize * 9 * 6);

	void* vertexData    = vertexBuffer.m_data.data();
	uint32_t* indexData = reinterpret_cast<uint32_t*>(indexBuffer.m_data.data());

	constexpr uint32_t quads[] = {
		0, 1, 4, 5,
		1, 2, 5, 6,
		2, 3, 6, 7,
		4, 5, 8, 9,
		6, 7, 10, 11,
		8, 9, 12, 13,
		9, 10, 13, 14,
		10, 11, 14, 15,
		16, 17, 18, 19
	};

	for (uint32_t i = 0; i < 9; ++i)
	{
		indexData[i * 6 + 0] = quads[i * 4 + 0];
		indexData[i * 6 + 1] = quads[i * 4 + 1];
		indexData[i * 6 + 2] = quads[i * 4 + 3];
		indexData[i * 6 + 3] = quads[i * 4 + 3];
		indexData[i * 6 + 4] = quads[i * 4 + 2];
		indexData[i * 6 + 5] = quads[i * 4 + 0];
	}

	Color* colorArray = reinterpret_cast<Color*>(static_cast<uint8_t*>(vertexData) + sizeof(Point3D) * vertexBuffer.GetElementCount());

	for (uint32_t i = 0; i < 20; ++i)
	{
		const Color borderColor = Color(0.0f, 1.0f, 0.0f, 1.0f);
		const Color centerColor = Color(0.0f, 1.0f, 0.0f, 0.2f);

		colorArray[i] = i >= 16 ? centerColor : borderColor;
	}
}

void PlayerCameraSystem::UpdateDragIndicator(TransformComponent* cameraTransform, Rendering::CameraComponent* camera,
											 CameraDragSelectComponent* cameraDrag) const
{
	const Point2DInt currentPos = InputManager::GetInstance()->GetMousePosition();

	const Point2DInt startPos = Point2DInt::Min(currentPos, cameraDrag->m_dragStartPosition);
	const Point2DInt endPos   = Point2DInt::Max(currentPos, cameraDrag->m_dragStartPosition);

	const float halfW = 2.0f / Application::Get().GetWindow().GetWidth();
	const float halfH = 2.0f / Application::Get().GetWindow().GetHeight();

	const float xValues[4] = { startPos.x * halfW - 1.0f, (startPos.x + 1) * halfW - 1.0f, (endPos.x - 1) * halfW - 1.0f, endPos.x * halfW - 1.0f };
	const float yValues[4] = { startPos.y * halfH - 1.0f, (startPos.y + 1) * halfH - 1.0f, (endPos.y - 1) * halfH - 1.0f, endPos.y * halfH - 1.0f };

	Rendering::DynamicMeshEntity* dynamicMeshEntity = *cameraDrag->m_dragIndicator;

	const ComponentAccessor& accessor              = dynamicMeshEntity->GetComponentAccessor();
	const Rendering::MeshComponent* meshComponent  = accessor.GetComponent<Rendering::MeshComponent>();
	const std::shared_ptr<DynamicMesh> dynamicMesh = std::static_pointer_cast<DynamicMesh>(meshComponent->m_mesh);

	Submesh& submesh         = dynamicMesh->GetSubmesh(0);
	MeshBuffer& vertexBuffer = submesh.GetVertexBuffer();

	void* vertexData    = vertexBuffer.m_data.data();
	Point3D* pointArray = static_cast<Point3D*>(vertexData);

	for (uint32_t i = 0; i < 4; ++i)
	{
		for (uint32_t j = 0; j < 4; ++j)
		{
			pointArray[i * 4 + j] = Point2D(xValues[j], - yValues[i]);
		}
	}

	pointArray[16 + 0] = Point2D(xValues[1], - yValues[1]);
	pointArray[16 + 1] = Point2D(xValues[2], - yValues[1]);
	pointArray[16 + 2] = Point2D(xValues[1], - yValues[2]);
	pointArray[16 + 3] = Point2D(xValues[2], - yValues[2]);

	for (Entity* entity : GetEntitiesFromScreenArea(cameraTransform, camera, startPos, endPos))
	{
		if (SelectableComponent* selectableComponent = entity->GetComponentAccessor().GetComponent<SelectableComponent>()) { }
	}
}

void PlayerCameraSystem::OnDragEnd(TransformComponent* cameraTransform, Rendering::CameraComponent* camera, CameraDragSelectComponent* cameraDrag)
{
	cameraDrag->m_isDragging                                                                                    = false;
	cameraDrag->m_dragIndicator->GetComponentAccessor().GetComponent<Rendering::RenderComponent>()->m_isEnabled = false;

	const Point2DInt currentPos = InputManager::GetInstance()->GetMousePosition();

	const Point2DInt startPos = Point2DInt::Min(currentPos, cameraDrag->m_dragStartPosition);
	const Point2DInt endPos   = Point2DInt::Max(currentPos, cameraDrag->m_dragStartPosition);

	ClearSelection();

	for (Entity* entity : GetEntitiesFromScreenArea(cameraTransform, camera, startPos, endPos))
	{
		if (SelectableComponent* selectableComponent = entity->GetComponentAccessor().GetComponent<SelectableComponent>())
		{
			AddToSelection(entity);
		}
	}
}
