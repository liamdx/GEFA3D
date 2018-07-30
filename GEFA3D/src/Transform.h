#pragma once

#include "Common.h"
#include "EngineComponent.h"
class Transform : public EngineComponent
{
public: 
	Transform() : EngineComponent("Transform")
	{
		position = glm::vec3(0, 0, 0 );
		rotationAngle = 0.0f;
		rotationVector = glm::vec3(0, 0, 0);
		scale = glm::vec3(1, 1, 1);
	}

	Transform(Transform& parent) : EngineComponent("Transform")
	{
		position = glm::vec3(0, 0, 0);
		rotationAngle = 0.0f;
		rotationVector = glm::vec3(0, 0, 0);
		scale = glm::vec3(1, 1, 1);
		parentTransform = &parent;
	}

	~Transform() override
	{
		
	}

	//Transform(const Transform* parent);

	glm::vec3 position;
	float rotationAngle;
	glm::vec3 rotationVector;
	glm::vec3 scale;

	glm::mat4 getModelMatrix()
	{
		glm::mat4 model = glm::mat4(1.0);
		model = glm::translate(model, position);
		model = glm::rotate(model, rotationAngle, rotationVector);
		model = glm::scale(model, scale);

		if (parentTransform == nullptr)
		{
			glm::mat4 model = glm::mat4(1.0);
			model = glm::translate(model, position);
			model = glm::rotate(model, rotationAngle, rotationVector);
			model = glm::scale(model, scale);
			return model;
		}
		else	
		{
			glm::mat4 model = glm::mat4(1.0);
			glm::mat4 parentModel = parentTransform->getModelMatrix();
			model = glm::translate(model, position);
			model = glm::rotate(model, rotationAngle, rotationVector);
			model = glm::scale(model, scale);

			return parentModel * model;
		}
	}
	inline void setPosition(glm::vec3 newPosition) { position = newPosition; }
	inline void setRotation(glm::vec3 newRotationVector, float angle) { rotationVector = newRotationVector; rotationAngle = angle; }
	inline void setScale(glm::vec3 newScale) { position = newScale; }
	inline void setParent(Transform* parent) { parentTransform = parent; }
	inline void identity() { position = glm::vec3(0.0f); rotationAngle = 0.0f; rotationVector = glm::vec3(0.0f); scale = glm::vec3(1.0f); }

	void updateBehaviour() override
	{
		std::cout << "Object scale - X: " << scale.x << " Y: " << scale.y << " Z: " << scale.z << std::endl;
		std::cout << "Object position - X: " << position.x << " Y: " << position.y << " Z: " << position.z << std::endl;
	}
private:
	Transform * parentTransform;
	

};