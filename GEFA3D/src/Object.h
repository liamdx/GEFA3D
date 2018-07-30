#pragma once

#include "Model.h";
#include "Common.h"
#include "Transform.h"
#include "EngineComponent.h"


class Object {
private:
	std::vector<std::unique_ptr<EngineComponent>> components;
public:
	Object(std::string name);
	
	Transform transform;
	std::string name;

	inline void addComponent(EngineComponent* newComponent) { components.emplace_back(newComponent); }
	
	// getComponent NOT tested, need to check if component is moved to new  memory location, or pointer will affect component stored in Object array.
	inline EngineComponent* getComponent(const char* componentName) { for (int i = 0; i < components.size(); i++) { if (components[i]->componentName == componentName) { return components[i].get(); } } }

	//Transform related
	inline void setPosition(glm::vec3 newPosition) { transform.position = newPosition; }
	inline glm::vec3 getPosition() { return transform.position; }
	inline void setScale(glm::vec3 newScale) { transform.scale = newScale; }
	inline glm::vec3 getScale() { return transform.scale; }
	inline void setRotation(glm::vec3 newRotationVector, float newRotationAngle) { transform.rotationAngle = newRotationAngle; transform.rotationVector = newRotationVector; }
	virtual void updateBehaviour();
};

class GameObject : Object
{
private:
	std::vector<std::unique_ptr<EngineComponent>> components;
public:
	GameObject(std::string name, Shader shader) : Object(name), shader(shader) {};

	std::string name;
	Shader shader;

	virtual void updateBehaviour() override;
	void renderBehaviour();
};

