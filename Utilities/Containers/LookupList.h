#pragma once

template<typename T, typename L>
class LookupList
{
	friend class World;

public:

	explicit LookupList(std::function<L(const T&)> lookupFunction) :
		m_lookupFunction(lookupFunction) { }

	void Add(const T& entity, L id)
	{
		m_lookupList[id] = (uint32_t) m_list.size();
		m_list.emplace_back(entity);
	}

	bool Contains(L id) const
	{
		// ReSharper disable once CppUseAssociativeContains
		return m_lookupList.find(id) != m_lookupList.end();
	}

	uint32_t GetCount() const
	{
		return (uint32_t) m_list.size();
	}


	void Clear()
	{
		m_list.clear();
		m_lookupList.clear();
	}

	void Remove(L id)
	{
		if (GetCount() == 1)
		{
			Clear();
			return;
		}

		const uint32_t index    = m_lookupList[id];
		const T& objectToInsert = m_list[m_list.size() - 1];

		m_lookupList.erase(id);
		m_lookupList[m_lookupFunction(objectToInsert)] = index;

		m_list[index] = objectToInsert;

		m_list.erase(m_list.end() - 1);
	}

	T* Find(L id)
	{
		const auto it = m_lookupList.find(id);
		if (it != m_lookupList.end())
		{
			return &m_list[it->second];
		}
		return nullptr;
	}

	[[nodiscard]] const std::vector<T>& GetItems() const { return m_list; }

private:

	std::map<L, uint32_t> m_lookupList;
	std::vector<T> m_list;

	std::function<L(const T&)> m_lookupFunction;
};
