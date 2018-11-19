#pragma once
#include "Examples\EngineCore.h"

enum BodyShape {
	Sphere,
	Cube,
	Capsule,
	Mesh
};



class PhysicsManager 
{
public:

	

	struct Body
	{
		b3BodyDef bodyDef;
		b3ShapeDef shapeDef;
		b3Transform transform;
		BodyShape shape;
		b3Body* body;
		unsigned int index;
		unsigned int vao, vbo;



		void initDebugMesh()
		{
			glGenVertexArrays(1, &vao);
			glGenBuffers(1, &vbo);
			glBindVertexArray(vao);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
		}
	};

	PhysicsManager()
	{
		const b3Vec3 gravity(0.0f, -9.8f, 0.0f);
		// The fixed time step size.
		const float32 timeStep = 1.0f / frameRate ;
		// Number of iterations for the velocity constraint solver.
		const u32 velocityIterations = 8;
		// Number of iterations for the position constraint solver.
		const u32 positionIterations = 2;

		//Create Physics world
		// The world-> We allocate it using the heap but you can to it 
		// on the stack if the stack is sufficiently large.
		world = new b3World();
		world->SetGravity(gravity);
	};
	~PhysicsManager();

	//Create body
	int addBody(BodyShape shape)
	{
		unsigned int currentBodyCount = bodies.size();
		Body testbody;
		testbody.body = world->CreateBody(testbody.bodyDef);

	}
	
	void setGravity(glm::vec3 newGravity) { currentGravity = b3Vec3(newGravity.x, newGravity.y, newGravity.z) ; world->SetGravity(currentGravity); }
	b3Vec3 getGravity() { return currentGravity; }

	void DebugDrawBody(Shader shader, Body body)
	{
		shader.use();
		glEnable(GL_PROGRAM_POINT_SIZE);
		glPointSize(10.0);
		
	}

private:
	b3World * world;
	b3Vec3 currentGravity;
	float frameRate = 60.0f;
	std::vector<Body> bodies;
};