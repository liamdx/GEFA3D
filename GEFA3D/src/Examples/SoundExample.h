#pragma once
#include "Examples\Example.h"
#include "Examples\EngineCore.h"


class SoundTest : Example
{
public:
	SoundTest(GLFWwindow* newWindow);
	~SoundTest();
	void init() override;
	void start() override;
	void earlyUpdate(float deltaTime) override;
	void update(float deltaTime) override;
	void UI() override;


	GLFWwindow* window;

	YSE::sound mySound;
	float counter;

};