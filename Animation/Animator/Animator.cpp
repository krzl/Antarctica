#include "stdafx.h"
#include "Animator.h"

namespace Anim
{
	Animator::Animator(std::vector<std::shared_ptr<StateMachine>>&& stateMachines) :
		m_stateMachines(std::move(stateMachines)) { }
}
