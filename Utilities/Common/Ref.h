#pragma once

template<typename T>
class Ref
{
public:

	Ref() :
		m_ptr(std::weak_ptr<T>())
	{
	}

	// ReSharper disable once CppNonExplicitConvertingConstructor
	Ref(std::weak_ptr<T> ptr) :
		m_ptr(ptr)
	{
	}

	// ReSharper disable once CppNonExplicitConvertingConstructor
	Ref(std::shared_ptr<T> ptr) :
		m_ptr(std::weak_ptr<T>(ptr))
	{
	}

	T* operator*() const
	{
		return IsValid() ? m_ptr.lock().get() : nullptr;
	}

	bool IsValid() const
	{
		return !m_ptr.expired();
	}

	explicit operator bool() const
	{
		return IsValid();
	}

private:

	std::weak_ptr<T> m_ptr;
};