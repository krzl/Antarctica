#pragma once

class PerformanceMonitor
{
	friend class PerformanceMonitorSystem;
	
	struct Node
	{
		uint64_t m_id;
		float m_time;
		std::string m_name;
		std::map<uint32_t, Node> m_childNodes;

		Node* m_parent;
	};

public:

	PerformanceMonitor();
	~PerformanceMonitor();

	static PerformanceMonitor& GetMonitor();

	void OpenNode(uint64_t id, const std::string& name);
	void CloseNode(float time);

	void CombineAllMonitors();
	void Clear();

private:

	Node m_rootNode = { 0, 0.0f, "", {}, nullptr };

	Node* m_currentNode = nullptr;

	static float MergeNodes(Node* targetNode, Node* sourceNode);
	static void ClearNode(Node* node);
	
	static std::mutex m_updateListMutex;
	static std::vector<PerformanceMonitor*> m_monitors;
};