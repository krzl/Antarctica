#include "stdafx.h"
#include "PerformanceMonitorSystem.h"

#include "imgui/imgui.h"

#include "Managers/TimeManager.h"

#include "Performance/PerformanceMonitor.h"

void PerformanceMonitorSystem::Run()
{
	PerformanceMonitor& monitor = PerformanceMonitor::GetMonitor();
	monitor.CombineAllMonitors();

	++m_fpsCounterFramesElapsed;
	const float timeSinceStart = TimeManager::GetInstance()->GetTimeSinceStart();
	if (timeSinceStart - m_fpsCounterLastUpdate > 1.0f)
	{
		m_currentFps = (uint32_t) (m_fpsCounterFramesElapsed / (timeSinceStart - m_fpsCounterLastUpdate));
		m_fpsCounterLastUpdate = timeSinceStart;
		m_fpsCounterFramesElapsed = 0;
	}
	
	ImGui::SetNextWindowCollapsed(true, ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_FirstUseEver);
	ImGui::Begin("FPS Counter", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground);
	ImGui::SetWindowFontScale(4.0f);
	ImGui::Text("%d", m_currentFps);
	ImGui::End();
	
	ImGui::SetNextWindowCollapsed(true, ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowPos(ImVec2(600.0f, 50.0f), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300.0f, 0.0f), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Performance Window", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		static ImGuiTableFlags flags = ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_BordersOuter |
			ImGuiTableFlags_BordersV;
		
		ImGui::BeginTable("PerformanceTable", 2, flags);

		ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_NoHide);
		ImGui::TableSetupColumn("Time", ImGuiTableColumnFlags_WidthFixed, 100.0f);
		ImGui::TableHeadersRow();

		ProcessNode(monitor.m_rootNode);
		ImGui::EndTable();
	}
	ImGui::End();

	monitor.Clear();
}

void PerformanceMonitorSystem::ProcessNode(const PerformanceMonitor::Node& node)
{
	for (auto [id, childNode] : node.m_childNodes)
	{
		static char array[10];

		ImGui::TableNextRow();
		ImGui::TableNextColumn();

		if (childNode.m_childNodes.size() > 0)
		{
			const bool open = ImGui::TreeNodeEx(childNode.m_name.c_str(), ImGuiTreeNodeFlags_SpanFullWidth);
			ImGui::TableNextColumn();
			snprintf(array, sizeof(array), "%f", childNode.m_time);
			ImGui::TextUnformatted(array);
			if (open)
			{
				ProcessNode(childNode);
				ImGui::TreePop();
			}
		}
		else
		{
			ImGui::TreeNodeEx(childNode.m_name.c_str(),
				ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_NoTreePushOnOpen);
			ImGui::TableNextColumn();
			snprintf(array, sizeof(array), "%f", childNode.m_time);
			ImGui::TextUnformatted(array);
		}
	}
}
