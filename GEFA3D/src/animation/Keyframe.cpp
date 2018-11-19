#include "animation\Keyframe.h"

Keyframe::Keyframe(aiMatrix4x4 GlobalInverseTransform,
	std::map<std::string, aiVector3D> Translations,
	std::map<std::string, aiVector3D> Scales,
	std::map<std::string, aiQuaternion>  Rotations,
	unsigned int position,
	unsigned int boneCount,
	float inTimeStamp)
{
	m_GlobalInverseTransform = GlobalInverseTransform;
	trans = Translations;
	scales = Scales;
	rots = Rotations;
	index = position;
	timeStamp = inTimeStamp;
}

void Keyframe::LoadData(aiMatrix4x4 GlobalInverseTransform,
	std::map<std::string, aiVector3D>Translations,
	std::map<std::string, aiVector3D> Scales,
	std::map<std::string, aiQuaternion>   Rotations,
	unsigned int position,
	unsigned int boneCount,
	float inTimeStamp)
{
	m_GlobalInverseTransform = GlobalInverseTransform;
	trans = Translations;
	scales = Scales;
	rots = Rotations;
	index = position;
	timeStamp = inTimeStamp;
}

