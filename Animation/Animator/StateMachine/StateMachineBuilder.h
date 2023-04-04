#pragma once

#include "StateMachine.h"

namespace Anim
{
	class StateMachineBuilder
	{
	public:

		template<typename T, typename... Args>
		std::enable_if_t<std::is_base_of_v<State, T>, std::shared_ptr<T>> AddState(Args&&... args)
		{
			std::shared_ptr<T> ptr = std::make_shared<T>(std::forward<Args...>(args...));
			m_states.emplace_back(ptr);
			return ptr;
		}

		std::shared_ptr<StateMachine> Build(std::shared_ptr<State> initialState = nullptr);

	private:

		std::vector<std::shared_ptr<State>> m_states;
	};
}
