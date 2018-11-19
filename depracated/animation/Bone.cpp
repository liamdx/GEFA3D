#include "animation\Bone.h"

//Bone::Bone(AnimatedMesh* in_mesh, unsigned int in_id, std::string in_name, aiMatrix4x4 in_ai_mat)
//	: mesh(in_mesh)
//{
//	id = in_id;
//	name = in_name;
//	offset_matrix = AiToGlmMat4(in_ai_mat);
//
//
//	parent_bone = nullptr;
//	node = nullptr;
//}

Bone::Bone(unsigned int in_id, std::string in_name, glm::mat4 in_glm_mat)
{
	id = in_id;
	name = in_name;
	offset_matrix = in_glm_mat;


	parent_bone = NULL;
	parent_skeleton = new Skeleton();
	node = NULL;
}

glm::mat4 Bone::GetParentTransforms()
{
	Bone* b = NULL;

	
	if (parent_bone != NULL)
	{
		b = parent_bone;
	}//In order to recursively concatenate the transforms,
							  //we first start with this bone's parent.
	std::vector<glm::mat4> mats;    //Where we'll store the concatenated transforms.

	while (b != NULL)    //As long as 'b' has a parent (see the end of the loop
	{                      //to avoid confusion).

		//error here
		aiMatrix4x4 currentNodeTransform = b->node->mTransformation;
		glm::mat4 tmp_mat = AiToGlmMat4(currentNodeTransform); //This bone's transformation.
		mats.push_back(tmp_mat);

		b = b->parent_bone;    //We set b to its own parent so the loop can continue.
	}

	glm::mat4 concatenated_transforms;
	//IMPORTANT!!!! This next loop must be done in reverse, 
	//as multiplication with matrices is not commutative.
	if (mats.size() > 0)
	{
		for (int i = mats.size() - 1; i >= 0; i--)
		{
			concatenated_transforms *= mats.at(i);
		}
	}
	else
	{
		concatenated_transforms = glm::mat4(1.0);
	}
	

		

	return concatenated_transforms;    //Finally, we return the concatenated transforms.
}

unsigned int Bone::FindPosition(float time)
{
	for (unsigned int i = 0; i < animNode->mNumPositionKeys - 1; i++)
	{
		//If the time passed in is less than the time of the next
		//keyframe, then this is the keyframe we want!
		if (time < (float)animNode->mPositionKeys[i + 1].mTime)
		{
			return i;
		}
	}
	return 0;
}

glm::vec3 Bone::CalcInterpolatedPosition(float time)
{
	//If there's only one keyframe for the position, we return it.
	//This avoids any possible out of range errors later on in the function
	//as we're dealing with an array.
	if (animNode->mNumPositionKeys == 1)
	{
		aiVector3D assimp_val = animNode->mPositionKeys[0].mValue;
		glm::vec3 val(assimp_val.x, assimp_val.y, assimp_val.z);
		return val;
	}

	//The index of our current position, as well as the index that follows.
	//This will allow us to interpolate between the two values.
	unsigned int PositionIndex = FindPosition(time);
	unsigned int NextPositionIndex = (PositionIndex + 1);
	//delta time between current two keyframes
	float AnimDeltaTime = animNode->mPositionKeys[NextPositionIndex].mTime - animNode->mPositionKeys[PositionIndex].mTime;
	//Time passed since last keyframe
	float Factor = (time - (float)animNode->mPositionKeys[PositionIndex].mTime) / AnimDeltaTime;
	//The start and end positions (the position values of each of the keyframes)
	const aiVector3D StartPosition = animNode->mPositionKeys[PositionIndex].mValue;
	const aiVector3D EndPosition = animNode->mPositionKeys[NextPositionIndex].mValue;

	glm::vec3 P1(StartPosition.x, StartPosition.y, StartPosition.z);
	glm::vec3 P2(EndPosition.x, EndPosition.y, EndPosition.z);

	glm::vec3 val = glm::mix(P1, P2, Factor);

	return val;



}

unsigned int Bone::FindRotation(float time)
{
	for (unsigned int i = 0; i < animNode->mNumRotationKeys - 1; i++)
	{
		//Same as with the position.
		if (time < (float)animNode->mRotationKeys[i + 1].mTime)
		{
			return i;
		}
	}
	return 0;
}

glm::quat Bone::CalcInterpolatedRotation(float time)
{
	if (animNode->mNumRotationKeys == 1)
	{
		aiQuaternion assimp_val = animNode->mRotationKeys[0].mValue;
		glm::quat val(assimp_val.w, assimp_val.x, assimp_val.y, assimp_val.z);
		return val;
	}

	unsigned int RotationIndex = FindRotation(time);
	unsigned int NextRotationIndex = (RotationIndex + 1);

	float AnimDeltaTime = animNode->mRotationKeys[NextRotationIndex].mTime - animNode->mRotationKeys[RotationIndex].mTime;
	float Factor = (time - (float)animNode->mRotationKeys[RotationIndex].mTime) / AnimDeltaTime;
	
	const aiQuaternion& StartRotationQ = animNode->mRotationKeys[RotationIndex].mValue;
	const aiQuaternion& EndRotationQ = animNode->mRotationKeys[NextRotationIndex].mValue;

	glm::quat R1(StartRotationQ.w, StartRotationQ.x, StartRotationQ.y, StartRotationQ.z);
	glm::quat R2(EndRotationQ.w, EndRotationQ.x, EndRotationQ.y, EndRotationQ.z);

	glm::quat val = glm::slerp(R1, R2, Factor);

	return val;

}

void Bone::UpdateKeyframeTransform(float time)
{
	if (animNode == nullptr)
		return;

	pos = CalcInterpolatedPosition(time);
	rot = CalcInterpolatedRotation(time);
	scale = glm::vec3(1.0);    //You could add support for scaling, but that's
							   //beyond the scope of this tutorial series.


	glm::mat4 mat = glm::mat4(1.0);
	mat *= glm::translate(mat, pos);
	mat *= glm::mat4_cast(rot);
	mat *= glm::scale(mat,scale);

	node->mTransformation = GLMMat4ToAi(mat);
}


//Skeleton stuff
void Skeleton::Init(std::vector<Bone> in_bones, glm::mat4 in_globalInverseTransform)
{
	for (int i = 0; i < in_bones.size(); i++)
	{
		bones.push_back(in_bones[i]);
	}
	
	globalInverseTransform = in_globalInverseTransform;

	time = start_time = end_time = 0;
	active_animation = nullptr;
	idle_animation = nullptr;

	anim_loop = false;


	for (unsigned int i = 0; i < bones.size(); i++)
	{
		bones.at(i).parent_skeleton = this;

	}
}

Bone* Skeleton::FindBone(std::string name)
{
	for (int i = 0; i < bones.size(); i++)
	{
		if (bones.at(i).name == name)
			return &bones.at(i);
	}
	return nullptr;
}

void Skeleton::UpdateBoneMatsVector()
{
	if (bones.size() == 0)
	{
		return;
	}

	boneMats.clear();

	for (int i = 0; i < bones.size(); i++) // 100 represents the maxbones per shader.
	{
		if (i > bones.size() - 1)
		{
			boneMats.push_back(glm::mat4(1.0));
		}
		else
		{
			aiMatrix4x4 currentNodeTransform;
			currentNodeTransform = bones.at(i).node->mTransformation;

			glm::mat4 concatenated_transformation = (bones.at(i).GetParentTransforms() * AiToGlmMat4(currentNodeTransform));
			boneMats.push_back(globalInverseTransform * concatenated_transformation * bones.at(i).offset_matrix);

		}
	}
}

void Skeleton::PlayAnimation(Animation& anim, bool loop, bool reset_to_start)
{
	//If there's an animation currently playing
	if (active_animation != nullptr)
	{
		//And this animation is more important than the current one
		if (anim.priority < active_animation->priority)
			//Set the current animation to the one passed in.
			active_animation = &anim;
		else
			//Do nothing.
			return;
	}
	else {
		//Set the current animation to the one passed in.

		active_animation = &anim;

		start_time = active_animation->start_time;
		end_time = active_animation->end_time;

		anim_play = true;
		anim_loop = loop;

		//The reset_to_start variable determines whether or not the animation
		//should restart upon playing.

		if (reset_to_start) {
			time = active_animation->start_time;
		}
	}
}


void Skeleton::updateBehaviour(float deltaTime)
{
	
	if (!anim_play)
	{
		//If we're not playing an animation, then just give up, do nothing.
		return;
	}
	//Update the time variable by adding the delta time of the last frame
	//It's * 0.001f because the delta time is in milliseconds, and we 
	//need it in seconds.
	time += deltaTime * 0.001f;

	//Make sure the time can't be less than our animation's start time.
	if (time < start_time)
		time = start_time;

	//Make sure the time can't be greater than our animation's end time.
	if (time > 2.0f)
	{
		if (anim_loop)
			//If looping is set, then loop!
			time = 0.0f;
		else
			//Else, give up.
			//StopAnimating();
			time = 0.0f;
	}

	for (int i = 0; i < bones.size(); i++)
	{
		bones.at(i).UpdateKeyframeTransform(time);

	}

	UpdateBoneMatsVector();

}
