#include "stdafx.h"
#include "RTSCamera.h"

#include "CameraScrollComponent.h"
#include "MousePickerComponent.h"
#include "Debug/DebugDraw/DebugDrawComponent.h"
#include "Debug/ImGui/ImGuiComponent.h"

RTSCamera::RTSCamera()
{
	m_cameraScrollComponent = AddComponent<CameraScrollComponent>();
	m_mousePicker           = AddComponent<MousePickerComponent>();
	AddComponent<ImGuiComponent>();
	AddComponent<DebugDrawComponent>();
}
