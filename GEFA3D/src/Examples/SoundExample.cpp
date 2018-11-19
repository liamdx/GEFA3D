
#include "SoundExample.h"

//namespace example {



SoundTest::SoundTest(GLFWwindow* newWindow)
	: window(newWindow)
{
	
}
SoundTest::~SoundTest()
{

}

void SoundTest::init()
{
	mySound.create("res/sounds/clap.wav");
}

void SoundTest::start()
{
	
}

void SoundTest::earlyUpdate(float deltaTime)
{


}

void SoundTest::update(float deltaTime)
{

	if (counter >= 1.0f)
	{
		mySound.play();
		counter = 0.0f;
	}
	else
	{
		counter += 1 * deltaTime;
	}

}


void SoundTest::UI()
{
	
}
//}