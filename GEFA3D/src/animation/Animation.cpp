#include "animation\Animation.h"

Animation::Animation(std::vector<Keyframe> inKeyframes, std::map<std::string, unsigned int> BoneMapping, float timeInTicks, float TicksPerSecond, unsigned int NumBones)
{
	keyframes = inKeyframes;
	m_BoneMapping = BoneMapping;
	totalTicks = timeInTicks;
	ticksPerSecond = TicksPerSecond;
	totalSecs = totalTicks / ticksPerSecond;
	numBones = NumBones;
}

std::vector<aiMatrix4x4> Animation::GetCurrentPose(float animationTime)

{
	float animationTimeInTicks = animationTime / ticksPerSecond;
	Keyframe current = GetCurrentKeyframe(animationTime);
	Keyframe next = keyframes[current.index + 1];

	std::vector<glm::mat4> transforms;

	// float DeltaTime = (float)(pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime);
	// float Factor = (AnimationTime - (float)pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;

	float DeltaTime = next.timeStamp - current.timeStamp;
	float Factor = animationTime - current.timeStamp / DeltaTime;

	for (int i = 0; i < numBones; i++)
	{
		std::string currentBoneName = m_BoneMapping.at[i];

		aiVector3D aPos = current.trans[currentBoneName];
		aiVector3D bPos = next.trans[currentBoneName];

		aiVector3D aScl = current.scales[currentBoneName];
		aiVector3D bScl = next.scales[currentBoneName];

		aiQuaternion aRot = current.rots[currentBoneName];
		aiQuaternion bRot = next.rots[currentBoneName];

		aiVector3D positionDelta = bPos - aPos;
		aiVector3D finalPosition = aPos + Factor * positionDelta;

		aiVector3D scaleDelta = bScl - aScl;
		aiVector3D finalScale = aScl + Factor * scaleDelta;

		aiQuaternion finalRotation;
		aiQuaternion::Interpolate(finalRotation, aRot, bRot, Factor);
		finalRotation.Normalize();


		aiMatrix4x4 scaling_mat;
		aiMatrix4x4::Scaling(finalScale, scaling_mat);
		aiMatrix4x4 rotating_mat = aiMatrix4x4(finalRotation.GetMatrix());
		aiMatrix4x4 translate_mat;
		aiMatrix4x4::Translation(finalPosition, translate_mat);

		aiMatrix4x4 BoneTransform = translate_mat * rotating_mat * scaling_mat;
	}

}


Keyframe Animation::GetCurrentKeyframe(float animationTime)
{
	for (int i = 0; i < keyframes.size(); i++)
	{
		if (keyframes[i + 1].timeStamp > animationTime)
		{
			return keyframes[i];
		}
	}
}