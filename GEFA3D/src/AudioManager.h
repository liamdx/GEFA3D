#pragma once
#include "Common.h"

struct AudioClip
{
	YSE::sound* clip;
	AudioProperties settings;
	
	void Play()
	{
		settings.setProperties(*this);
		clip->play();
	}
};

struct AudioProperties
{
	float volume = 0.8f;
	float radius = 5.0f;
	float speed = 1.0f;
	bool doppler = false;
	bool spatialized = true;
	bool loop = false;

	void setProperties(AudioClip clip)
	{
		clip.clip->setDoppler(doppler);
		clip.clip->setRelative(spatialized);
		clip.clip->setLooping(loop);
		clip.clip->setVolume(volume);
		clip.clip->setSize(radius);
		clip.clip->setSpeed(speed);
	}
};

class AudioManager
{
public:

	void PlayOneShot(AudioClip clip )
	{
		clip.Play();
	}

	AudioClip loadAudioClip(const char* path)
	{
		AudioClip ret;
		ret.clip = loadSound(path);

	}


private:

	YSE::sound* loadSound(const char * path)
	{
		YSE::sound* sound;
		sound->create(path);
		return sound;
	}

};