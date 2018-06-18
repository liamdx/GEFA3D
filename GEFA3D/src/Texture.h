#pragma once
#include "Common.h"
class Texture {
public:
	unsigned int t_Id;
	unsigned int t_Slot;
	unsigned int slot;

	Texture(const char* filepath, unsigned int slot);
	~Texture();

	void Use();
};