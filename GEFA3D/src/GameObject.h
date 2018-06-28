#pragma once
#include "Common.h"
#include "Model.h"
#include "MeshCollider.h"

class GameObject 
{

	

public:
	Model* model;
	Shader* shader;
	PhysicsType pType;
	unsigned int numberOfMeshes;
	GameObject(std::string const &modelPath, const char* vertexPath, const char* fragPath, b3World physicsWorld,PhysicsType type); 

	void updateBehaviour();
	void setPosition(b3Vec3 newPosition, unsigned int meshIndex);
	
private:
	std::vector<b3Body*> bodies;
	unsigned int numberOfBodies;
	glm::vec3 position;
	glm::vec3 scale;
};