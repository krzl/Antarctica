#pragma once

#include "CameraData.h"
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

class PlayerCameraSystem : public System<TransformComponent, Rendering::CameraComponent, CameraScrollComponent>
{
	void Init() override;

	void OnUpdateStart() override;
	void OnUpdateEnd() override;

	void Update(uint64_t entityId, TransformComponent* transform, Rendering::CameraComponent* camera, CameraScrollComponent* cameraScroll) override;

	Matrix4D GetPerspectiveMatrix(const Rendering::CameraComponent* camera) const;
	Frustum GetFrustum(Rendering::CameraComponent* camera) const;

	void TryTriggerAbilitiesFromSelection();
	Entity* FindSuitableEntityForAbility(const std::string& abilityId) const;
	void ActivateAbility(Entity* entity, const std::string& abilityId);
	void AddAbilityToStack(AbilityStackComponent* abilityStack, std::shared_ptr<Ability> ability, Entity* entity) const;

	static bool IsAbilityInputPressed(const AbilityBinding& abilityBinding, const InputCommand& inputCommand);
	static bool CanEntityActivateAbility(const AbilityTriggerComponent* abilityTrigger, const std::string& abilityId);

public:

	[[nodiscard]] const std::optional<Point3D>& GetCursorWorldPosition() const { return m_cursorWorldPosition; }
	[[nodiscard]] std::vector<Rendering::CameraData>& GetCameras();

	void AddToSelection(Entity* entity);

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
