#pragma once

#include "Asset.h"

struct Skeleton;

struct PositionKey
{
	const Vector3D m_position;
	const float    m_time;
};

struct RotationKey
{
	const Quaternion m_rotation;
	const float      m_time;
};

struct ScaleKey
{
	const Vector3D m_scale;
	const float    m_time;
};

struct AnimationNode
{
	AnimationNode() { }

	AnimationNode(std::string&&                 nodeName,
				  Transform4D&&                 baseTransform,
				  std::vector<PositionKey>&&    positionKeys,
				  std::vector<RotationKey>&&    rotationKeys,
				  std::vector<ScaleKey>&&       scaleKeys,
				  std::vector<AnimationNode*>&& children) :
		m_nodeName(std::move(nodeName)),
		m_baseTransform(std::move(baseTransform)),
		m_positionKeys(std::move(positionKeys)),
		m_rotationKeys(std::move(rotationKeys)),
		m_scaleKeys(std::move(scaleKeys)),
		m_children(std::move(children)) {}
	
	std::string m_nodeName;

	Transform4D m_baseTransform = {};

	std::vector<PositionKey> m_positionKeys;
	std::vector<RotationKey> m_rotationKeys;
	std::vector<ScaleKey>    m_scaleKeys;

	std::vector<AnimationNode*> m_children;
};

struct SkeletonBinding
{
	explicit SkeletonBinding(const Skeleton& skeleton) :
		m_skeleton(skeleton) {}

	const Skeleton& m_skeleton;
};

class Animation : public Asset
{
public:

	Animation() = default;

	Animation(std::vector<AnimationNode>&& nodes, const AnimationNode& rootNode, const float duration) :
		m_nodes(std::move(nodes)),
		m_rootNode(&rootNode),
		m_duration(duration) {}

	const std::vector<AnimationNode> m_nodes;
	const AnimationNode*             m_rootNode;

	const float m_duration = 0.0f;
};
