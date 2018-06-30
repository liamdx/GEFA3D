#include "GameObject.h"

GameObject::GameObject(Model& model, Shader& shader, b3World* physicsWorld)
	: g_Shader(shader), g_Model(model)
{
	g_Body = physicsWorld->CreateBody(bodyDef);
	for (unsigned int i = 0; i < g_Model.meshes.size(); i++)
	{
		g_Model.meshes[i].transform.setParent(g_Transform);
	}
}

GameObject::GameObject(std::string const &path, Shader& shader, b3World* physicsWorld)
	:g_Shader(shader), g_Model(Model(path))
{
	g_Body = physicsWorld->CreateBody(bodyDef);
	for (unsigned int i = 0; i < g_Model.meshes.size(); i++)
	{
		g_Model.meshes[i].transform.setParent(g_Transform);
	}
}

GameObject::GameObject(const GameObject& copy)
	:g_Model(copy.g_Model), g_Shader(copy.g_Shader)
{
	g_Body = copy.g_Body;

}
void GameObject::updateBehaviour()
{
	
}

void GameObject::Draw()
{
	g_Shader.use();
	g_Model.Draw(g_Shader);
	
	
}

