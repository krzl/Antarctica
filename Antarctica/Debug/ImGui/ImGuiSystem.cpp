#include "stdafx.h"
#include "ImGuiSystem.h"

#include "AssetManager.h"
#include "imgui.h"
#include "ImGuiManager.h"
#include "Assets/DynamicMesh.h"
#include "Assets/Material.h"
#include "Assets/Mesh.h"
#include "Components/MeshComponent.h"
#include "Core/Application.h"
#include "Managers/TimeManager.h"

void ImGuiSystem::Init()
{
	System::Init();

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

	io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
	io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
	io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;

	ImGui::StyleColorsDark();

	ImGui::GetMainViewport()->PlatformHandleRaw = (void*) Application::Get().GetWindow().GetHandle();

	uint8_t* fontData;
	int32_t texWidth, texHeight, bytesPerPixel;

	io.Fonts->GetTexDataAsRGBA32(&fontData, &texWidth, &texHeight, &bytesPerPixel);

	m_texture = std::make_shared<Texture>(fontData, texWidth, texHeight, bytesPerPixel / 4, "ImGui Texture");

	m_shader   = AssetManager::GetAsset<Shader>("../Resources/Shaders/imgui.hlsl");
	m_material = std::make_shared<Material>(m_shader);

	m_material->GetShaderParams().m_depthTestEnabled = false;
	m_material->GetShaderParams().m_blendingEnabled  = true;
	m_material->GetShaderParams().m_isDoubleSided    = true;

	m_material->SetTexture("tex", m_texture);
	m_material->SetOrder(UI + 500);

	World::Get()->Spawn<ImGuiManager>({});

	m_attributeUsage = { false, false, false, 1, 2, 0, 0, 0 };
}

void ImGuiSystem::OnFrameStart()
{
	System::OnFrameStart();

	const Platform::Window& window = Application::Get().GetWindow();

	ImGuiIO& io    = ImGui::GetIO();
	io.DisplaySize = ImVec2(window.GetWidth(), window.GetHeight());
	io.DeltaTime   = TimeManager::GetInstance()->GetDeltaTime();

	const auto& inputSystem   = InputManager::GetInstance();
	const Point2DInt mousePos = inputSystem->GetMousePosition();

	io.MousePos     = ImVec2((float) mousePos.x, (float) mousePos.y);
	io.MouseDown[0] = inputSystem->IsLeftMousePressed();
	io.MouseDown[1] = inputSystem->IsRightMousePressed();

	ImGui::NewFrame();

	m_onNewFrame.Dispatch();
}

void ImGuiSystem::OnUpdateStart()
{
	ImGui::Render();
}

struct Vertex
{
	Point3D m_position;
	Color m_color;
	Vector2D m_texcoord;
};

void ImGuiSystem::Update(Entity* entity, ImGuiComponent* imgui, Rendering::MeshComponent* mesh)
{
	const ImDrawData* imDrawData = ImGui::GetDrawData();

	const Vector2D scale = Vector2D(2.0f / imDrawData->DisplaySize.x, -2.0f / imDrawData->DisplaySize.y);
	m_material->SetVariable<Vector2D>("scale", scale);

	uint32_t submeshCount = 0;
	for (uint32_t drawListId = 0; drawListId < (uint32_t) imDrawData->CmdListsCount; ++drawListId)
	{
		for (const ImDrawCmd& cmd : imDrawData->CmdLists[drawListId]->CmdBuffer)
		{
			if (cmd.UserCallback == nullptr)
			{
				submeshCount++;
			}
		}
	}

	if (mesh->m_renderItems.size() == 0)
	{
		Rendering::RenderItem& renderItem = mesh->m_renderItems.emplace_back();

		renderItem.m_mesh      = std::make_shared<DynamicMesh>();
		renderItem.m_materials = { m_material };
	}

	Rendering::RenderItem& renderItem              = mesh->m_renderItems[0];
	const std::shared_ptr<DynamicMesh> dynamicMesh = std::static_pointer_cast<DynamicMesh>(renderItem.m_mesh);

	dynamicMesh->SetSubmeshCount(submeshCount);
	renderItem.m_rectMasks.resize(submeshCount);

	uint32_t cmdIndex = 0;

	for (uint32_t drawListId = 0; drawListId < (uint32_t) imDrawData->CmdListsCount; ++drawListId)
	{
		const ImDrawList* drawList = imDrawData->CmdLists[drawListId];

		for (const ImDrawCmd& cmd : drawList->CmdBuffer)
		{
			if (cmd.UserCallback != nullptr)
			{
				continue;
			}

			Submesh& submesh = dynamicMesh->GetSubmesh(cmdIndex);

			MeshBuffer& vertexData = submesh.GetVertexBuffer();
			submesh.SetAttributesUsage(m_attributeUsage);

			vertexData.m_elementSize = sizeof(Point3D) + sizeof(Color) + sizeof(Vector2D);
			vertexData.m_data.resize(vertexData.m_elementSize * drawList->VtxBuffer.Size);

			for (uint32_t j = 0; j < (uint32_t) drawList->VtxBuffer.size(); ++j)
			{
				const ImDrawVert& vertex = drawList->VtxBuffer[j];

				const Vertex v =
				{
					Point3D(vertex.pos.x, vertex.pos.y, 1.0f),
					Color(vertex.col),
					Vector2D(vertex.uv.x, vertex.uv.y),
				};

				memcpy(&vertexData.m_data[drawList->VtxBuffer.Size * 0 + j * 12], &v.m_position, sizeof(Point3D));
				memcpy(&vertexData.m_data[drawList->VtxBuffer.Size * 12 + j * 16], &v.m_color, sizeof(Color));
				memcpy(&vertexData.m_data[drawList->VtxBuffer.Size * 28 + j * 8], &v.m_texcoord, sizeof(Vector2D));
			}

			MeshBuffer& indexData = submesh.GetIndexBuffer();

			indexData.m_elementSize = sizeof(Vertex);
			indexData.m_data.resize(indexData.m_elementSize * cmd.ElemCount);

			for (uint32_t j = 0; j < (uint32_t) cmd.ElemCount; ++j)
			{
				const uint32_t idx = drawList->IdxBuffer[j + cmd.IdxOffset];
				memcpy(&indexData.m_data[j * sizeof(uint32_t)], &idx, sizeof(uint32_t));
			}

			submesh.SetDynamic();

			renderItem.m_rectMasks[cmdIndex] = Rect{
				{
					cmd.ClipRect.x,
					cmd.ClipRect.y,
				},
				{
					cmd.ClipRect.z,
					cmd.ClipRect.w
				}
			};

			++cmdIndex;
		}
	}
}
