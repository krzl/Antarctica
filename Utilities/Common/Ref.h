#pragma once

template<typename T>
class Ref
{
public:

	// Constructors

	Ref() :
		m_ptr(std::weak_ptr<T>()) { }

	Ref(const Ref& other) = default;

	// ReSharper disable once CppNonExplicitConvertingConstructor
	Ref(std::weak_ptr<T> ptr) :
		m_ptr(ptr) { }

	template<
		class D,
		class = std::enable_if_t<std::is_base_of_v<T, D>>>
	// ReSharper disable once CppNonExplicitConvertingConstructor
	Ref(const Ref<D>& other) :
		Ref(other.m_ptr) { }

	template<
		class D,
		class = std::enable_if_t<std::is_base_of_v<T, D>>>
	// ReSharper disable once CppNonExplicitConvertingConstructor
	Ref(std::weak_ptr<D> ptr) :
		m_ptr(std::weak_ptr<T>(ptr)) { }

	// ReSharper disable once CppNonExplicitConvertingConstructor
	Ref(std::shared_ptr<T> ptr) :
		m_ptr(std::weak_ptr<T>(ptr)) { }

	template<
		class D,
		class = std::enable_if_t<std::is_base_of_v<T, D>>>
	// ReSharper disable once CppNonExplicitConvertingConstructor
	Ref(std::shared_ptr<D> ptr) :
		m_ptr(std::weak_ptr<T>(ptr)) { }

	// Assignment operators

	Ref& operator=(const Ref& other)
	{
		m_ptr = other.m_ptr;
		return *this;
	}

	Ref& operator=(std::weak_ptr<T> ptr)
	{
		m_ptr = ptr;
		return *this;
	}

	template<
		class D,
		class = std::enable_if_t<std::is_base_of_v<T, D>>>
	Ref& operator=(std::weak_ptr<D> ptr)
	{
		m_ptr = std::weak_ptr<T>(ptr);
		return *this;
	}

	Ref& operator=(std::shared_ptr<T> ptr)
	{
		m_ptr = ptr;
		return *this;
	}

	template<
		class D,
		class = std::enable_if_t<std::is_base_of_v<T, D>>>
	Ref& operator=(std::shared_ptr<D> ptr)
	{
		m_ptr = std::weak_ptr<T>(ptr);
		return *this;
	}

	template<class T2>
	Ref<T2> Cast() const
	{
		if (IsValid())
		{
			return Ref<T2>(std::dynamic_pointer_cast<T2, T>(m_ptr.lock()));
		}
		return Ref<T2>(nullptr);
	}

	friend bool operator==(const Ref& lhs, const Ref& rhs) { return lhs.m_ptr.lock() == rhs.m_ptr.lock(); }
	friend bool operator!=(const Ref& lhs, const Ref& rhs) { return !(lhs == rhs); }
	// Dereferencing

	T* operator*()
	{
		return IsValid() ? m_ptr.lock().get() : nullptr;
	}

	T* operator->()
	{
		return IsValid() ? m_ptr.lock().get() : nullptr;
	}

	const T* operator*() const
	{
		return IsValid() ? m_ptr.lock().get() : nullptr;
	}

	const T* operator->() const
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