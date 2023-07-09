#include "stdafx.h"
#include "Debug/ImGuiSystem.h"

#include "AssetManager.h"
#include "imgui.h"
#include "ImGuiShader.h"
#include "RenderObject.h"
#include "Assets/DynamicSubmesh.h"
#include "Core/Application.h"

void ImGuiSystem::Init()
{
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
	int32_t  texWidth, texHeight, bytesPerPixel;

	io.Fonts->GetTexDataAsRGBA32(&fontData, &texWidth, &texHeight, &bytesPerPixel);

	m_texture = std::make_shared<Texture>(fontData, texWidth, texHeight, bytesPerPixel / 4, "ImGui Texture");

	m_shader   = AssetManager::GetAsset<ImGuiShader>("../Resources/Shaders/imgui.hlsl");
	m_material = std::make_shared<Material>(m_shader);

	m_material->SetTexture("tex", m_texture);

	m_attributeUsage = {
		false,
		false,
		false,
		1,
		2,
		0,
		0,
		0
	};
}

void ImGuiSystem::Update()
{
	if (!m_wasRendered)
	{
		ImGui::Render();
	}
	const Platform::Window& window = Application::Get().GetWindow();

	ImGuiIO& io    = ImGui::GetIO();
	io.DisplaySize = ImVec2(window.GetWidth(), window.GetHeight());
	io.DeltaTime   = TimeSystem::GetInstance()->GetDeltaTime();

	const auto&         inputSystem = InputSystem::GetInstance();
	const MousePosition mousePos    = inputSystem->GetMousePosition();

	io.MousePos     = ImVec2((float) mousePos.first, (float) mousePos.second);
	io.MouseDown[0] = inputSystem->IsLeftMousePressed();
	io.MouseDown[1] = inputSystem->IsRightMousePressed();

	ImGui::NewFrame();
	ImGui::SetNextWindowPos(ImVec2(-500, -500));
	ImGui::ShowDemoWindow();

	m_wasRendered = false;
}

struct Vertex
{
	Point3D  m_position;
	Color    m_color;
	Vector2D m_texcoord;
};

std::vector<Renderer::QueuedRenderObject>& ImGuiSystem::Render()
{
	m_wasRendered = true;
	ImGui::Render();

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

	if (m_submeshes.size() != submeshCount)
	{
		m_submeshes.resize(submeshCount);
		m_renderObjectsCache.resize(submeshCount);
	}

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

			MeshBuffer& vertexData = m_submeshes[cmdIndex].GetVertexBuffer();
			m_submeshes[cmdIndex].SetAttributeUsage(m_attributeUsage);

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

			MeshBuffer& indexData = m_submeshes[cmdIndex].GetIndexBuffer();

			indexData.m_elementSize = sizeof(Vertex);
			indexData.m_data.resize(indexData.m_elementSize * cmd.ElemCount);

			for (uint32_t j = 0; j < (uint32_t) cmd.ElemCount; ++j)
			{
				const uint32_t idx = drawList->IdxBuffer[j + cmd.IdxOffset];
				memcpy(&indexData.m_data[j * sizeof(uint32_t)], &idx, sizeof(uint32_t));
			}

			m_renderObjectsCache[cmdIndex].m_submesh         = &m_submeshes[cmdIndex];
			m_renderObjectsCache[cmdIndex].m_material        = &*m_material;
			m_renderObjectsCache[cmdIndex].m_order           = 5000.0f + cmdIndex;
			m_renderObjectsCache[cmdIndex].m_perObjectBuffer = Renderer::PerObjectBuffer::DEFAULT_BUFFER;
			m_renderObjectsCache[cmdIndex].m_clipRect        = Rect{
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

	return m_renderObjectsCache;
}
