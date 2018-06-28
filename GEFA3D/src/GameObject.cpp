#include "GameObject.h"

GameObject::GameObject(std::string const &modelPath, const char* vertexPath, const char* fragPath, b3World physicsWorld, PhysicsType type)
{
	model = new Model(modelPath);
	shader= new Shader(vertexPath, fragPath);

	numberOfMeshes = model->meshes.size();
	if (type == PTStatic)
	{
		numberOfBodies = numberOfMeshes;
	}
	else if (type == PTDynamic)
	{
		numberOfBodies = 1;
	}
	
	if (type == PTStatic)
	{
		for (unsigned int i = 0; i < numberOfBodies; i++)
		{
			// Create a dynamic body.
			b3BodyDef bodyDef;
			bodyDef.type = e_staticBody;
			bodyDef.position.Set(0.0f, 0.0f, 0.0f);
			bodyDef.angularVelocity.Set(0.0f, B3_PI, 0.0f);
			bodies[i] = physicsWorld.CreateBody(bodyDef);

			MeshCollider collider(model->meshes[i]);

			b3ShapeDef bodyMeshDef;
			bodyMeshDef.shape = &collider.meshShape;
			bodyMeshDef.density = 1.0f;
			bodies[i]->CreateShape(bodyMeshDef);

		}
	}
	else if (type == PTDynamic)
	{
		b3BodyDef bodyDef;
		bodyDef.type = e_dynamicBody;
		// Position the body 10 meters high from the world origin.
		bodyDef.position.Set(0.0f, 0.0f, 0.0f);

		// Set the initial angular velocity to pi radians (180 degrees) per second.
		bodyDef.angularVelocity.Set(0.0f, B3_PI, 0.0f);
		bodies[0] = physicsWorld.CreateBody(bodyDef);

		b3CapsuleShape capsuleShape;
		capsuleShape.m_centers[0].Set(0.0f, 0.5f, 0.0f);
		capsuleShape.m_centers[1].Set(0.0f, -0.5f, 0.0f);
		capsuleShape.m_radius = 2.0f;

		// Add the box to the body.
		b3ShapeDef bodyBoxDef;
		bodyBoxDef.shape = &capsuleShape;
		bodyBoxDef.density = 1.0f;
		bodies[0]->CreateShape(bodyBoxDef);
	}
	
	

}

void GameObject::updateBehaviour()
{
	if (pType == PTDynamic) 
	{
		for (unsigned int i = 0; i < numberOfBodies; i++)
		{
			model->meshes[i].position = bodies[i]->GetPosition();
		}
	}
	else if (pType == PTStatic)
	{
		for (unsigned int i = 0; i < numberOfBodies; i++)
		{
			bodies[i]->SetTransform(model->meshes[i].position, b3Vec3(0, 0, 0), 0.0f);
		}
	}
	
}

void GameObject::setPosition(b3Vec3 newPosition, unsigned int meshIndex)
{
	
}