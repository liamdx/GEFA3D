#pragma once

#include "Common.h"

class Keyframe
{

public:

	unsigned int index;
	float animationLength;
	float timeStamp;
	aiMatrix4x4 m_GlobalInverseTransform;

	std::map<std::string, aiVector3D> trans;
	std::map<std::string, aiVector3D> scales;
	std::map<std::string, aiQuaternion> rots;


	Keyframe() {};

	void LoadData(aiMatrix4x4 GlobalInverseTransform,
		std::map<std::string, aiVector3D> Translations,
		std::map<std::string, aiVector3D> Scales,
		std::map<std::string, aiQuaternion> Rotations,
		unsigned int position,
		unsigned int boneCount,
		float inTimeStamp);

	Keyframe(aiMatrix4x4 GlobalInverseTransform,
		std::map<std::string, aiVector3D>  Translations,
		std::map<std::string, aiVector3D>  Scales,
		std::map<std::string, aiQuaternion>  Rotations,
		unsigned int position,
		unsigned int boneCount,
		float inTimeStamp);


	

};