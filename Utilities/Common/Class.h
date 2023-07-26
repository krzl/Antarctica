#pragma once

class Class
{
	typedef std::function<std::shared_ptr<void>()> DefaultConstructor;
	typedef std::function<void(void*)> PlacementConstructor;

public:

	template<typename T, class = std::enable_if_t<std::is_default_constructible_v<T>>>
	static Class CreateClassObject()
	{
		auto clazz          = Class(typeid(T).hash_code(), sizeof(T));
		clazz.m_constructor = [=]()
		{
			std::shared_ptr<T> ptr = std::make_shared<T>();
			ptr->m_class           = &clazz;
			return ptr;
		};

		clazz.m_placementConstructor = [=](void* ptr)
		{
			new(ptr) T();
		};

		return clazz;
	}

	bool operator==(const Class& other) const
	{
		return m_id == other.m_id;
	}

	bool operator!=(const Class& other) const
	{
		return m_id != other.m_id;
	}

	std::shared_ptr<void> CreateObject() const
	{
		return m_constructor();
	}

	void CreateObjectPlacement(void* ptr) const
	{
		return m_placementConstructor(ptr);
	}

	uint64_t GetId() const { return m_id; }
	uint64_t GetDataSize() const { return m_dataSize; }

private:

	explicit Class(const uint64_t id, const uint64_t dataSize) :
		m_id(id),
		m_dataSize(dataSize) { }

	const uint64_t m_id;
	const uint64_t m_dataSize;

	DefaultConstructor m_constructor;
	PlacementConstructor m_placementConstructor;
};

#define DEFINE_CLASS()											\
public:															\
	static inline const Class& GetClass();						\
private:

#define CREATE_CLASS(Clazz)										\
	const Class& Clazz::GetClass()								\
	{															\
		static Class clazz = Class::CreateClassObject<Clazz>();	\
		return clazz;											\
	}
