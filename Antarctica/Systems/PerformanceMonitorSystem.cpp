#include "stdafx.h"
#include "PerformanceMonitorSystem.h"

#include "imgui/imgui.h"
#include "Performance/PerformanceMonitor.h"

void PerformanceMonitorSystem::Run()
{
	PerformanceMonitor& monitor = PerformanceMonitor::GetMonitor();
	monitor.CombineAllMonitors();

	static ImGuiTableFlags flags = ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_BordersOuter |
		ImGuiTableFlags_BordersV;

	if (ImGui::Begin("PerformanceWindow"))
	{
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
