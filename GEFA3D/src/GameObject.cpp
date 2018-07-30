#include "GameObject.h"

GameObject::GameObject(Model& model, b3World* physicsWorld)
	:  g_Model(model)
{
	g_Transform = new Transform();
	g_Body = physicsWorld->CreateBody(bodyDef);
	for (unsigned int i = 0; i < g_Model.meshes.size(); i++)
	{
		g_Model.meshes[i].transform->setParent(g_Transform);
	}
}

GameObject::GameObject(std::string const &path, b3World* physicsWorld)
	: g_Model(Model(path))
{
	g_Transform = new Transform();
	g_Body = physicsWorld->CreateBody(bodyDef);
	for (unsigned int i = 0; i < g_Model.meshes.size(); i++)
	{
		g_Model.meshes[i].transform->setParent(g_Transform);
	}
}

GameObject::GameObject(const GameObject& copy)
	:g_Model(copy.g_Model)
{
	g_Body = copy.g_Body;

}
void GameObject::updateBehaviour()
{
	
}

void GameObject::addChild(Transform* child)
{
	children.push_back(child);
}

void GameObject::Draw(Shader shader)
{
	
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(g_Transform->position));
	model = glm::rotate(model, float(g_Transform->rotationAngle), glm::vec3(this->g_Transform->rotationVector));
	model = glm::scale(model, glm::vec3(g_Transform->scale));
	shader.setMat4("model", model);
	g_Model.Draw(shader);
	
	
}

