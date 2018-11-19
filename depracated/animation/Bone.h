#pragma once

#include "Common.h"
#include "animation\AnimUtils.h"
#include "animation\Animation.h"

class Skeleton;

class Bone
{
public:

	/*Bone() {
		name = "";
		id = -2;
		mesh = 0;
	}*/
	//Bone(AnimatedMesh* in_mesh, unsigned int in_id, std::string in_name, aiMatrix4x4 in_ai_mat);
	
	Bone( unsigned int in_id, std::string in_name, glm::mat4 in_glm_mat);


	unsigned int id;
	std::string name;
	//AnimatedMesh* mesh;
	aiNode* node;
	aiNodeAnim* animNode;
	Bone* parent_bone;
	glm::mat4 parent_transforms;
	glm::mat4 offset_matrix;

	Skeleton* parent_skeleton;

	//Keyframe data
	glm::vec3 pos;
	glm::quat rot;
	glm::vec3 scale;
	glm::vec3 p1;
	glm::vec3 p2;

	
	glm::mat4 GetParentTransforms();
	unsigned int FindPosition(float time);
	glm::vec3 CalcInterpolatedPosition(float time);
	unsigned int FindRotation(float time);
	glm::quat CalcInterpolatedRotation(float time);
	void UpdateKeyframeTransform(float time);

};

class Skeleton
{

public:
	std::vector<Bone> bones;
	glm::mat4 globalInverseTransform;
	std::vector<glm::mat4> boneMats;

	float time;

	float start_time;
	float end_time;

	Animation* active_animation;
	Animation* idle_animation;

	bool anim_play;
	bool anim_loop;

	Skeleton()
	{
		time = start_time = end_time = 0;
		active_animation = nullptr;

		anim_loop = false;
	}

	Skeleton(std::vector<Bone> in_bones, glm::mat4 in_globalInverseTransform)
	{
		Init(in_bones, in_globalInverseTransform);
	}

	~Skeleton() {}


	void Init(std::vector<Bone> in_bones, glm::mat4 in_globalInverseTransform);
	Bone* FindBone(std::string name);
	void UpdateBoneMatsVector();
	void updateBehaviour(float deltaTime);
	void PlayAnimation(Animation& anim, bool loop, bool reset_to_start);

	//This function stops animating

	void StopAnimating() { time = end_time; active_animation = nullptr; anim_play = false; }

	//This function sets the idle animation
	//(An idle animation, for those who don't know, is an animation
	//that plays when no other animations are playing)

	void SetIdleAnimation(Animation* in_anim) { idle_animation = in_anim; }
};

