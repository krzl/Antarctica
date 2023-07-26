#pragma once

#include "Solver/Solver.h"
#include "StateMachine/StateMachine.h"

namespace Anim
{
	class Animator
	{
		friend class TreeBuilder;
		friend class Solver;

	public:

		explicit Animator(std::vector<std::shared_ptr<StateMachine>>&& stateMachines);

	private:

		std::vector<std::shared_ptr<StateMachine>> m_stateMachines;
	};
}
