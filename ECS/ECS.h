#pragma once

class SystemBase;

class ECS
{
	friend class Application;

	~ECS();

	void RunBeginFrame();
	void RunStepLock();
	void RunEndFrame();

	void ExecuteForAllSystems(std::function<void(SystemBase*)> function) const;

	template<typename T, class = std::enable_if_t<std::is_base_of_v<SystemBase, T>>>
	T* GetSystem()
	{
		return static_cast<T*>(GetSystem(typeid(T).hash_code()));
	}

	SystemBase* GetSystem(uint64_t hash);

	enum SystemExecType
	{
		PRE_STEP_LOCK,
		STEP_LOCK,
		POST_STEP_LOCK
	};

	template<typename T, class = std::enable_if_t<std::is_base_of_v<SystemBase, T>>>
	void AddSystem(const SystemExecType execType)
	{
		T* system = new T();
		switch (execType)
		{
			case PRE_STEP_LOCK:
				m_preStepLockSystems.emplace_back(system);
				break;
			case STEP_LOCK:
				m_stepLockSystems.emplace_back(system);
				break;
			case POST_STEP_LOCK:
				m_postStepLockSystems.emplace_back(system);
				break;
		}

		m_lookupMap[typeid(T).hash_code()] = system;
	}

	std::vector<SystemBase*> m_preStepLockSystems;
	std::vector<SystemBase*> m_stepLockSystems;
	std::vector<SystemBase*> m_postStepLockSystems;

	std::map<uint64_t, SystemBase*> m_lookupMap;
};
