#pragma once
#include "Time/Timer.h"

template<typename T>
class ThreadPool
{
public:

	ThreadPool(std::function<void(T*)> function, const uint32_t threadCount)
		: m_function(function)
	{
		m_threads.resize(threadCount);
		for (uint32_t i = 0; i < threadCount; ++i)
		{
			m_threads[i] = std::thread(std::bind(&ThreadPool::ThreadLoop, this));
			m_threads[i].detach();
		}
	}

	void AddItem(T* item)
	{
		std::unique_lock lock(m_mutex);
		AddItem(item, lock);
	}

	void AddItem(T* item, std::unique_lock<std::mutex>& lock)
	{
		if (m_awaitingTaskCount >= MAX_ITEM_COUNT - 1)
		{
			m_updateEndCV.wait(lock, [this] { return m_awaitingTaskCount < MAX_ITEM_COUNT - 1; });
		}
		m_awaitingList[m_awaitingTaskCount++] = item;
		++m_tasksRemaining;

		m_awaitCV.notify_one();
	}

	void AddItems(T* const* start, uint32_t count)
	{
		std::unique_lock lock(m_mutex);
		m_updateEndCV.wait(lock, [this, count] { return m_awaitingTaskCount < MAX_ITEM_COUNT - count; });
		memcpy(m_awaitingList.data() + m_awaitingTaskCount, start, sizeof(T*) * count);
		m_awaitingTaskCount += count;
		m_tasksRemaining += count;
		m_awaitCV.notify_all();
	}

	void RemoveItem(T* item)
	{
		std::lock_guard lock(m_mutex);
		for (uint32_t i = 0; i < m_awaitingTaskCount;)
		{
			if (m_awaitingList[i] == item)
			{
				m_awaitingList[i] = m_awaitingList[m_awaitingTaskCount - 1];
				m_awaitingTaskCount--;
			}
			else
			{
				++i;
			}
		}
	}

	void ThreadLoop()
	{
		while (true)
		{
			T* awaitingItem;
			{
				std::unique_lock lock(m_mutex);
				m_awaitCV.wait(lock, [this] { return m_awaitingTaskCount != 0; });

				awaitingItem = m_awaitingList[--m_awaitingTaskCount];
			}

			m_function(awaitingItem);
			{
				std::lock_guard lock(m_mutex);
				--m_tasksRemaining;
			}
			m_updateEndCV.notify_one();
		}
	}

	void WaitForCompletion()
	{
		std::unique_lock lock(m_mutex);
		if (m_tasksRemaining != 0)
		{
			m_updateEndCV.wait(lock, [this] { return m_tasksRemaining == 0; });
		}
	}

private:

	static constexpr uint16_t MAX_ITEM_COUNT = 1024 * 16;

	uint16_t                       m_awaitingTaskCount = 0;
	uint16_t                       m_tasksRemaining = 0;
	std::array<T*, MAX_ITEM_COUNT> m_awaitingList = {};

	std::vector<std::thread> m_threads;

	std::mutex m_mutex;

	std::condition_variable m_awaitCV;
	std::condition_variable m_updateEndCV;

	std::function<void(T*)> m_function;
};
