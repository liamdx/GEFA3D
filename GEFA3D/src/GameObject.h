#pragma once
#include "Common.h"
#include "Model.h"
#include "MeshCollider.h"
#include "Transform.h"


class GameObject
{
public:
	//basic
	Model g_Model;
	Transform* g_Transform;
	std::vector<Transform*> children;

	//physics
	b3BodyDef bodyDef;
	const b3Body* g_Body;

	//Constructors
	GameObject(Model& model, b3World* physicsWorld);
	GameObject(std::string const &path,  b3World* physicsWorld);
	
	GameObject(const GameObject& copy);
	
	// Member functions
	void updateBehaviour();
	void Draw(Shader shader);
	void addChild(Transform* child);

	inline void setPosition(glm::vec3 position) { g_Transform->position = position; }

	~GameObject()
	{
		
	}

private:
	
};