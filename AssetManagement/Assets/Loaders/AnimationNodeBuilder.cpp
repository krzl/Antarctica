#include "stdafx.h"
#include "AnimationNodeBuilder.h"

AnimationNodeBuilder::AnimationNodeBuilder(std::string nodeName, const Transform4D& transform) :
	m_nodeName(std::move(nodeName)),
	m_baseTransform(transform) {}

void AnimationNodeBuilder::AddChildren(AnimationNode& node)
{
	m_children.emplace_back(&node);
}

void AnimationNodeBuilder::SetPositionKeys(std::vector<PositionKey>&& positionKeys)
{
	m_positionKeys = std::move(positionKeys);
}

void AnimationNodeBuilder::SetRotationKeys(std::vector<RotationKey>&& rotationKeys)
{
	m_rotationKeys = std::move(rotationKeys);
}

void AnimationNodeBuilder::SetScaleKeys(std::vector<ScaleKey>&& scaleKeys)
{
	m_scaleKeys = std::move(scaleKeys);
}

AnimationNode AnimationNodeBuilder::Build()
{
	return AnimationNode(std::move(m_nodeName), std::move(m_baseTransform), std::move(m_positionKeys),
						 std::move(m_rotationKeys), std::move(m_scaleKeys), std::move(m_children));
}
