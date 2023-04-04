#pragma once

class System
{
	friend class Application;

public:

	virtual ~System() = default;

protected:

	virtual void Init() {}
	virtual void Update() {}
};

class SystemCreator
{
	template<typename T>
	friend class SystemRegistrator;
	friend class Application;

public:

	template<typename T>
	static void RegisterSystem()
	{
		m_systemConstructors.emplace_back([]()
		{
			T* ptr        = new T;
			T::m_instance = ptr;
			return ptr;
		});
	}

	static std::vector<System*> CreateSystems();

	static inline std::vector<std::function<System*()>> m_systemConstructors;
};

template<typename T>
class SystemRegistrator
{
public:

	SystemRegistrator()
	{
		SystemCreator::RegisterSystem<T>();
	}
};

#define REGISTER_SYSTEM(System) \
	friend class SystemCreator; \
	static inline SystemRegistrator<System> systemRegistrator;\
	public:\
	static System* GetInstance() { return m_instance; }\
	private:\
	static inline System* m_instance = nullptr;
