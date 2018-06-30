#pragma once
#include "Common.h"
#include "Model.h"
#include "MeshCollider.h"
#include "Transform.h"


class GameObject
{
public:
	//basic
	Shader& g_Shader;
	Model g_Model;
	Transform g_Transform;
	std::vector<Transform&> children;

	//physics
	b3BodyDef bodyDef;
	const b3Body* g_Body;

	//Constructors
	GameObject(Model& model, Shader& shader, b3World* physicsWorld);
	GameObject(std::string const &path, Shader& shader, b3World* physicsWorld);
	
	GameObject(const GameObject& copy);
	
	// Member functions
	void updateBehaviour();
	void Draw();
	void addChild(Transform child);
	~GameObject()
	{
		
	}

private:
	
};