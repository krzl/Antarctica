#pragma once

template<typename... _Args>
class Dispatcher;

template<typename... _Args>
class DispatchHandle
{
	typedef std::function<void(_Args...)> FunctionType;
	typedef Dispatcher<_Args...>          DispatcherType;

	friend class Dispatcher<_Args...>;

public:

	~DispatchHandle()
	{
		if (m_autoClear)
		{
			Clear();
		}
	}

	void Clear();

private:

	explicit DispatchHandle(DispatcherType& dispatcher, const uint32_t id, const bool autoClear = true) :
		m_dispatcher(&dispatcher),
		m_autoClear(autoClear),
		m_id(id) { }

	DispatcherType* m_dispatcher;
	bool            m_autoClear;
	uint32_t        m_id;
};

template<typename... _Args>
class Dispatcher
{
	typedef std::function<void(_Args...)> FunctionType;
	typedef DispatchHandle<_Args...>      HandleType;

	friend class DispatchHandle<_Args...>;

private:

	struct DispatchListElem
	{
		FunctionType m_function;
		uint32_t     m_id;

		DispatchListElem(FunctionType&& function, const uint32_t id) :
			m_function(std::move(function)),
			m_id(id) { }
	};

public:

	Dispatcher& operator=(const Dispatcher& other)     = delete;
	Dispatcher& operator=(Dispatcher&& other) noexcept = delete;

	~Dispatcher()
	{
		RemoveAllListeners();
	}

	HandleType AddListener(FunctionType listener, const bool autoClear = true)
	{
		const uint32_t listenerId = counter++;

		m_dispatchList.emplace_back(std::move(listener), listenerId);
		return HandleType(*this, listenerId, autoClear);
	}

	void RemoveAllListeners()
	{
		m_dispatchList.clear();
	}

	void Dispatch(_Args... arguments)
	{
		for (auto& dispatchElement : m_dispatchList)
		{
			dispatchElement.m_function(std::forward<_Args>(arguments)...);
		}
	}

private:

	void RemoveListener(HandleType& handle)
	{
		for (auto& it = m_dispatchList.begin(); it != m_dispatchList.end(); ++it)
		{
			if (it->m_id == handle.m_id)
			{
				handle.m_dispatcher = nullptr;
				m_dispatchList.erase(it);
				return;
			}
			if (it->m_id > handle.m_id)
			{
				return;
			}
		}
	}

	std::vector<DispatchListElem> m_dispatchList;

	uint32_t counter = 0;
};

template<typename... _Args>
void DispatchHandle<_Args...>::Clear()
{
	if (m_dispatcher != nullptr)
	{
		m_dispatcher->RemoveListener(*this);
	}
}
