#include "stdafx.h"
#include "PerformanceMonitor.h"

std::vector<PerformanceMonitor*> PerformanceMonitor::m_monitors;
std::mutex PerformanceMonitor::m_updateListMutex;

static thread_local PerformanceMonitor threadLocalMonitor;

PerformanceMonitor::PerformanceMonitor()
{
	std::lock_guard lock(m_updateListMutex);
	m_monitors.emplace_back(this);

	m_currentNode = &m_rootNode;
}

PerformanceMonitor::~PerformanceMonitor()
{
	std::lock_guard lock(m_updateListMutex);
	m_monitors.erase(std::remove(m_monitors.begin(), m_monitors.end(), this), m_monitors.end());
}

PerformanceMonitor& PerformanceMonitor::GetMonitor()
{
	return threadLocalMonitor;
}

void PerformanceMonitor::OpenNode(const uint64_t id, const std::string& name)
{
	const auto it = m_currentNode->m_childNodes.find(id);
	if (it != m_currentNode->m_childNodes.end())
	{
		m_currentNode = &it->second;
		return;
	}

	m_currentNode->m_childNodes[id] = Node{
		id,
		0.0f,
		name,
		{},
		m_currentNode
	};

	m_currentNode = &m_currentNode->m_childNodes.find(id)->second;
}

void PerformanceMonitor::CloseNode(const float time)
{
	m_currentNode->m_time += time;
	m_currentNode = m_currentNode->m_parent;
}


void PerformanceMonitor::CombineAllMonitors()
{
	if (m_rootNode.m_childNodes.size() == 0)
	{
		return;
	}

	Node* workerNode = &m_rootNode.m_childNodes[0];
	if (workerNode->m_id == 0)
	{
		workerNode->m_id   = 0xFFFFFFFFFFFFFFFF;
		workerNode->m_name = "Workers";
		return;
	}

	std::lock_guard lock(m_updateListMutex);
	for (uint32_t i = 0; i < m_monitors.size(); ++i)
	{
		PerformanceMonitor* monitor = m_monitors[i];
		if (monitor == this)
		{
			continue;
		}


		Node* threadWorkerNode = nullptr;

		auto it = workerNode->m_childNodes.find(i - 1);
		if (it == workerNode->m_childNodes.end())
		{
			workerNode->m_childNodes[i - 1] = Node{
				i - 1,
				0.0f,
				"WorkerThread" + std::to_string(i - 1),
				{},
				workerNode
			};

			threadWorkerNode = &workerNode->m_childNodes.find(i - 1)->second;
		}
		else
		{
			threadWorkerNode = &it->second;
		}

		threadWorkerNode->m_time = MergeNodes(threadWorkerNode, &monitor->m_rootNode);
	}
}

void PerformanceMonitor::Clear()
{
	ClearNode(&m_rootNode);
}

float PerformanceMonitor::MergeNodes(Node* targetNode, Node* sourceNode)
{
	float accumulator = 0.0f;

	for (auto& [id, childNode] : sourceNode->m_childNodes)
	{
		auto it = targetNode->m_childNodes.find(id);
		if (it == targetNode->m_childNodes.end())
		{
			targetNode->m_childNodes[id] = Node{
				id,
				childNode.m_time,
				childNode.m_name,
				{},
				targetNode
			};

			accumulator += MergeNodes(&targetNode->m_childNodes[id], &childNode);
		}
		else
		{
			it->second.m_time += childNode.m_time;
			accumulator += childNode.m_time;
			accumulator += MergeNodes(&it->second, &childNode);
		}

		childNode.m_time = 0.0f;
	}

	return accumulator;
}

void PerformanceMonitor::ClearNode(Node* node)
{
	for (auto& [id, childNode] : node->m_childNodes)
	{
		childNode.m_time = 0.0f;
		ClearNode(&childNode);
	}
}
