#pragma once

#include "Common.h"

//IMPORTANT!!!! The 0.041666666667 float in these functions is equal to
//    1.0 / 24.0, which is how many frames there are in a second, as the software
//    I am using (Blender 3D), plays animations at 24 frames per second.

//This first function converts from a time passed in (in seconds) to the number
//of frames to which it corresponds.
static float TimeToFrame(float time)
{
	float frame = 0.041666666667;
	return time / frame;
}

//This next function converts from a start frame and an end frame passed in to
//a start time and an end time, both of which are stored in a glm::vec2.
static glm::vec2 FramesToTime(glm::vec2 frames)
{
	float frame = 0.041666666667;
	return frames * frame;
}

class Animation
{
public:
	std::string name;
	float start_time;
	float end_time;
	int priority;

	Animation() { name = "", start_time = 0.0f, end_time = 0.0f, priority = 10; }
	Animation(std::string in_name, glm::vec2 times, int in_priority)
	{
		name = in_name;
		start_time = times.x;
		end_time = times.y;
		priority = in_priority;

	}
	~Animation() {} 




};