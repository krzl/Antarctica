#pragma once

class Test
{
	
};

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
			ptr->m_clazz = clazz;
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

	Class(const uint64_t id) :
		m_id(id)
	{
	}

	const uint64_t m_id;

	DefaultConstructor m_constructor;
};

#define CREATE_CLASS(clazz)								*\
public:													*\
	static const Class& GetClassObject()				*\
	{													*\
		static Class clazz = CreateClassObject<Test>();	*\
		return clazz;									*\
	}													*\
private: