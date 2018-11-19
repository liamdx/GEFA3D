#pragma once

#include "Keyframe.h"

class Animation
{

public:

	float timeInTicks, totalTicks, ticksPerSecond;
	float timeInSecs, totalSecs;
	unsigned int numBones;


	std::vector<aiMatrix4x4> GetCurrentPose(float animationTime);

	Keyframe GetCurrentKeyframe(float animationTime);

	Animation(std::vector<Keyframe> inKeyframes, std::map<std::string, unsigned int> m_BoneMapping, float timeInTicks, float TicksPerSecond, unsigned int NumBones);

private:

	std::vector<Keyframe> keyframes;
	std::vector<aiMatrix4x4> m_Transforms;
	std::map<std::string, unsigned int> m_BoneMapping; // maps a bone name to its index
	
};