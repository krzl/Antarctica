#pragma once

template<typename T>
class Ref
{
public:

	// Constructors

	Ref() :
		m_ptr(std::weak_ptr<T>()),
		m_cachedPtr(nullptr) { }

	Ref(const Ref& other) = default;

	// ReSharper disable once CppNonExplicitConvertingConstructor
	Ref(std::weak_ptr<T> ptr) :
		m_ptr(ptr),
		m_cachedPtr(GetPtrSafe()) { }

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
		m_ptr(std::weak_ptr<T>(ptr)),
		m_cachedPtr(GetPtrSafe()) { }

	// ReSharper disable once CppNonExplicitConvertingConstructor
	Ref(std::shared_ptr<T> ptr) :
		m_ptr(std::weak_ptr<T>(ptr)),
		m_cachedPtr(GetPtrSafe()) { }

	template<
		class D,
		class = std::enable_if_t<std::is_base_of_v<T, D>>>
	// ReSharper disable once CppNonExplicitConvertingConstructor
	Ref(std::shared_ptr<D> ptr) :
		m_ptr(std::weak_ptr<T>(ptr)),
		m_cachedPtr(GetPtrSafe()) { }


	// Assignment operators

	Ref& operator=(const Ref& other)
	{
		m_ptr       = other.m_ptr;
		m_cachedPtr = GetPtrSafe();
		return *this;
	}

	Ref& operator=(std::weak_ptr<T> ptr)
	{
		m_ptr       = ptr;
		m_cachedPtr = GetPtrSafe();
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
		m_ptr       = ptr;
		m_cachedPtr = GetPtrSafe();
		return *this;
	}

	template<
		class D,
		class = std::enable_if_t<std::is_base_of_v<T, D>>>
	Ref& operator=(std::shared_ptr<D> ptr)
	{
		m_ptr       = std::weak_ptr<T>(ptr);
		m_cachedPtr = GetPtrSafe();
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

	// ReSharper disable once CppNonExplicitConversionOperator
	operator Ref<void>() const
	{
		return Ref<void>(m_ptr);
	}

	friend bool operator==(const Ref& lhs, const Ref& rhs) { return lhs.m_ptr.lock() == rhs.m_ptr.lock(); }
	friend bool operator!=(const Ref& lhs, const Ref& rhs) { return !(lhs == rhs); }
	// Dereferencing

	T* operator*()
	{
		return m_cachedPtr;
	}

	T* operator->()
	{
		return m_cachedPtr;
	}

	const T* operator*() const
	{
		return m_cachedPtr;
	}

	const T* operator->() const
	{
		return m_cachedPtr;
	}

	T* GetPtrSafe()
	{
		m_cachedPtr = IsValid() ? m_ptr.lock().get() : nullptr;
		return m_cachedPtr;
	}

	const T* GetPtrSafe() const
	{
		m_cachedPtr = IsValid() ? m_ptr.lock().get() : nullptr;
		return m_cachedPtr;
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
	mutable T* m_cachedPtr;
};
