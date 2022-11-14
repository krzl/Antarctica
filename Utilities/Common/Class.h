#pragma once

class Component;
class Class
{
	
	using DefaultConstructor = std::function<std::shared_ptr<void>()>;
	
public:

	template<
		typename T,
		class = std::enable_if_t<std::is_default_constructible<T>::value>>
	static Class CreateClassObject()
	{
		Class clazz = Class(typeid(T).hash_code());
		clazz.m_constructor = [=]()
		{
			std::shared_ptr<T> ptr = std::make_shared<T>();
			ptr->m_class = &clazz;
			return ptr;
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
	
	uint64_t GetId() const
	{
		return m_id;
	}

private:

	explicit Class(const uint64_t id) :
		m_id(id)
	{
	}

	const uint64_t m_id;

	DefaultConstructor m_constructor;
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