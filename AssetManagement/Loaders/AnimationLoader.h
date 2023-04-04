#pragma once
#include "assimp/matrix4x4.h"
#include "assimp/quaternion.h"
#include "assimp/vector3.h"
// ReSharper disable CppInconsistentNaming

struct aiNode;
struct aiAnimation;
class Animation;

Quaternion  AIQuaternionCast(const aiQuaternion& quaternion);
Vector3D    AIVectorCast(const aiVector3D& vector);
Transform4D AIMatrixCast(const aiMatrix4x4& matrix);

std::shared_ptr<Animation> ImportAnimation(const aiAnimation* aiAnimation, const aiNode* rootNode);
