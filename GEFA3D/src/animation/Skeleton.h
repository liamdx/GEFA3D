#pragma once

#include "Common.h"


class Bone
{
public: 

	Bone(aiMatrix4x4 globalInverseTransform, aiMatrix4x4 localBindTransform);

	~Bone();

	aiMatrix4x4 m_GlobalInverseTransform;

	aiMatrix4x4 m_LocalBindTransform;

	aiMatrix4x4 m_FinalTransformation;

	unsigned int parentIndex;

	unsigned int index;

	std::vector<Bone> children;

};

class Skeleton
{
public:

	Bone rootBone;

	unsigned numBones;

};