#pragma once

#include "CameraData.h"
#include "CameraDragSelectComponent.h"
#include "CameraScrollComponent.h"

#include "Abilities/AbilityStackComponent.h"
#include "Abilities/AbilityTriggerComponent.h"

#include "Components/CameraComponent.h"

#include "Input/InputQueue.h"

#include "Systems/System.h"


class Mesh;
struct AbilityBinding;
class BVH;
struct TransformComponent;

class PlayerCameraSystem : public System<TransformComponent, Rendering::CameraComponent, CameraScrollComponent, CameraDragSelectComponent>
{
	void Init() override;

	void OnUpdateStart() override;
	void OnUpdateEnd() override;

	void Update(Entity* entity, TransformComponent* transform, Rendering::CameraComponent* camera, CameraScrollComponent* cameraScroll,
				CameraDragSelectComponent* cameraDrag) override;

	Matrix4D GetPerspectiveMatrix(const Rendering::CameraComponent* camera) const;
	Frustum GetFrustum(Rendering::CameraComponent* camera, Rect sectionRect) const;
	Ray GetCameraRay(TransformComponent* transform, Rendering::CameraComponent* camera) const;
	std::vector<Entity*> GetEntitiesFromScreenArea(TransformComponent* cameraTransform, Rendering::CameraComponent* camera,
												   Point2DInt startPos, Point2DInt endPos) const;

	void TryTriggerAbilitiesFromSelection();
	Entity* FindSuitableEntityForAbility(const std::string& abilityId) const;
	void ActivateAbility(Entity* entity, const std::string& abilityId);
	void AddAbilityToStack(AbilityStackComponent* abilityStack, std::shared_ptr<Ability> ability, Entity* entity) const;

	static bool IsAbilityInputPressed(const AbilityBinding& abilityBinding, const InputCommand& inputCommand);
	static bool CanEntityActivateAbility(const AbilityTriggerComponent* abilityTrigger, const std::string& abilityId);

public:

	[[nodiscard]] const std::optional<Point3D>& GetCursorWorldPosition() const { return m_cursorWorldPosition; }

	//TODO: Remove
	[[nodiscard]] std::vector<Rendering::CameraData>& GetCameras();



	//TODO: Handle selection in another file
	//TODO: Remove entities from selection when destroyed
	void AddToSelection(Entity* entity);
	void ClearSelection();
	uint32_t GetSelectedCount() const;

	void CreateDragIndicator(CameraDragSelectComponent* cameraDrag) const;
	void UpdateDragIndicator(TransformComponent* cameraTransform, Rendering::CameraComponent* camera, CameraDragSelectComponent* cameraDrag) const;
	void OnDragEnd(TransformComponent* cameraTransform, Rendering::CameraComponent* camera, CameraDragSelectComponent* cameraDrag);

	void SetupTerrainBvh(const std::shared_ptr<Mesh> terrain);

private:

	InputQueue m_inputQueue;

	Entity* m_selectionGroupEntity = nullptr;
	std::set<Entity*> m_selectedEntities;

	std::shared_ptr<AbilityActivator> m_abilityActivator = nullptr;

	float m_aspectRatio = 1.0f;

	std::optional<Point3D> m_cursorWorldPosition;

	std::vector<Rendering::CameraData> m_cameras;

	std::shared_ptr<BVH> m_terrainBvh;
};
