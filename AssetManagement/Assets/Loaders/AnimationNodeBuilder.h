#pragma once

#include "Assets/Animation.h"

class AnimationNodeBuilder
{
public:

	explicit AnimationNodeBuilder(std::string nodeName, const Transform4D& transform);

	void AddChildren(AnimationNode& node);
	void SetPositionKeys(std::vector<PositionKey>&& positionKeys);
	void SetRotationKeys(std::vector<RotationKey>&& rotationKeys);
	void SetScaleKeys(std::vector<ScaleKey>&& scaleKeys);

	AnimationNode Build();

	[[nodiscard]] const std::string& GetNodeName() const { return m_nodeName; }

private:

	std::string m_nodeName;
	Transform4D m_baseTransform;
	std::vector<PositionKey> m_positionKeys;
	std::vector<RotationKey> m_rotationKeys;
	std::vector<ScaleKey> m_scaleKeys;
	std::vector<AnimationNode*> m_children;
};
