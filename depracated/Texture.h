#pragma once
#include "Common.h"
class Texture {
public:
	unsigned int t_Id;
	unsigned int t_Slot;
	unsigned int slot;
	std::string t_Type;

	Texture(const char* filepath, unsigned int slot);
	~Texture();

	void Use();
	inline void SetType(std::string type) { t_Type = type; }
};