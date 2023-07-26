// ReSharper disable CppInconsistentNaming
#pragma once

#include "Asset.h"

struct Skeleton;

struct aiNode;
struct aiAnimation;

struct PositionKey
{
	const Vector3D m_position;
	const float m_time;
};

struct RotationKey
{
	const Quaternion m_rotation;
	const float m_time;
};

struct ScaleKey
{
	const Vector3D m_scale;
	const float m_time;
};

struct AnimationNode
{
	AnimationNode() = default;

	AnimationNode(std::string&& nodeName,
				  Transform4D&& baseTransform,
				  std::vector<PositionKey>&& positionKeys,
				  std::vector<RotationKey>&& rotationKeys,
				  std::vector<ScaleKey>&& scaleKeys,
				  std::vector<AnimationNode*>&& children) :
		m_nodeName(std::move(nodeName)),
		m_nodeNameHash(std::hash<std::string>()(m_nodeName)),
		m_baseTransform(std::move(baseTransform)),
		m_positionKeys(std::move(positionKeys)),
		m_rotationKeys(std::move(rotationKeys)),
		m_scaleKeys(std::move(scaleKeys)),
		m_children(std::move(children)) {}

	std::string m_nodeName;
	uint64_t m_nodeNameHash;

	Transform4D m_baseTransform = {};

	std::vector<PositionKey> m_positionKeys;
	std::vector<RotationKey> m_rotationKeys;
	std::vector<ScaleKey> m_scaleKeys;

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
	friend void ImportAnimation(Animation& animation, const aiAnimation* aiAnimation, const aiNode* rootNode);

public:

	Animation() = default;

	[[nodiscard]] const std::vector<AnimationNode>& GetNodes() const { return m_nodes; }
	[[nodiscard]] const AnimationNode* GetRootNode() const { return m_rootNode; }
	[[nodiscard]] float GetDuration() const { return m_duration; }

protected:

	bool Load(const std::string& path) override;

private:

	float m_duration = 0.0f;

private:

	std::vector<AnimationNode> m_nodes;
	AnimationNode* m_rootNode;
};
