#pragma once

class Manager
{
	friend class Application;

public:

	virtual ~Manager() = default;

	virtual float GetPriority() const { return 0.0f; }

protected:

	virtual void Init() {}
	virtual void Update() {}
};

class ManagerCreator
{
	template<typename T>
	friend class ManagerRegistrator;
	friend class Application;

public:

	template<typename T>
	static void RegisterManager()
	{
		m_managerConstructors.emplace_back([]()
		{
			T* ptr        = new T;
			T::m_instance = ptr;
			return ptr;
		});
	}

	static std::vector<Manager*> CreateManagers();

	static inline std::vector<std::function<Manager*()>> m_managerConstructors;
};

template<typename T>
class ManagerRegistrator
{
public:

	ManagerRegistrator()
	{
		ManagerCreator::RegisterManager<T>();
	}
};

#define REGISTER_MANAGER(Manager) \
	friend class ManagerCreator; \
	static inline ManagerRegistrator<Manager> managerRegistrator;\
	public:\
	static Manager* GetInstance() { return m_instance; }\
	private:\
	static inline Manager* m_instance = nullptr;
