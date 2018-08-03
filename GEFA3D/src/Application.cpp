#define _CRT_SECURE_NO_WARNINGS

//Libs, helper functions and commonly used code
#include "Common.h"
#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "MeshCollider.h"
#include "Object.h"
#include "primitives\Cube.h"
#include "primitives\Quad.h"
#include "lighting\DirectionalLight.h"
#include "lighting\PointLight.h"
#include "lighting\SpotLight.h"
#include "ext/imgui/imgui.h"
#include "ext/imgui/imgui_impl_glfw_gl3.h"
#include "DebugEngineComponent.h"
#include "Transform.h"
#include "Cubemap.h"

//Engine Time
float deltaTime = 0.0f;
float lastFrame = 0.0f;
const float timeStep = 1 / 120.0f;

//camera;
float lastX;
float lastY;
bool firstMouse = true;
Camera cam(glm::vec3(0.0f, 0.0f, 3.0f));

//Forward Decalrations
void processInput(GLFWwindow *window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

//lights
std::vector<PointLight> pointLights;
std::vector<SpotLight> spotLights;


const unsigned int SCREEN_WIDTH = 1280;
const unsigned int SCREEN_HEIGHT = 720;

// We don't care for a profiler. This definition does nothing.
void b3BeginProfileScope(const char* name)
{

}

// We don't care for a profiler. This definition does nothing.
void b3EndProfileScope()
{

}


int main(void)
{
	//DEBUG
	int  success;
	char infoLog[512];
	//GL initialisation
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "GEFA3D", NULL, NULL);

	//Does nothing? what needs to be set up / is fxaa a better solution?
	//glfwWindowHint(GLFW_SAMPLES, 4);
	//glEnable(GL_MULTISAMPLE);

	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK)
	{
		std::cout << "failed to initalise glew" << std::endl;
	}

	ImGui::CreateContext();
	ImGui_ImplGlfwGL3_Init(window, true);
	ImGui::StyleColorsDark();

	//Mouse input handle

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	glfwSetCursorPosCallback(window, mouse_callback);

	//Enable depth
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	cam.MovementSpeed = 10.0f;
	//Physics
	//init
	// The world gravity.
	const b3Vec3 gravity(0.0f, -9.8f, 0.0f);
	// The fixed time step size.
	const float32 timeStep = 1.0f / 60.0f;
	// Number of iterations for the velocity constraint solver.
	const u32 velocityIterations = 8;
	// Number of iterations for the position constraint solver.
	const u32 positionIterations = 2;

	//Create Physics world
	// The world-> We allocate it using the heap but you can to it 
	// on the stack if the stack is sufficiently large.
	b3World* world = new b3World();
	world->SetGravity(gravity);

	// Create a static ground body at the world origin.
	b3BodyDef groundDef;
	b3Body* ground = world->CreateBody(groundDef);
	// Create a box positioned at the world origin and 
	// aligned with the world frame.
	b3BoxHull groundBox;
	// Set the ground box dimensions using a linear scale transform.
	b3Transform scale;
	scale.position.SetZero();
	scale.rotation = b3Diagonal(100.0f, 1.0f, 100.0f);
	groundBox.SetTransform(scale);
	// Create the box physics wrapper.
	b3HullShape groundShape;
	groundShape.m_hull = &groundBox;
	// Add the box to the ground body.
	b3ShapeDef groundBoxDef;
	groundBoxDef.shape = &groundShape;
	ground->CreateShape(groundBoxDef);

	// Create a dynamic body.
	b3BodyDef bodyDef;
	bodyDef.type = e_dynamicBody;
	bodyDef.fixedRotationX = true;
	bodyDef.fixedRotationZ = true;
	// Position the body 10 meters high from the world origin.
	bodyDef.position.Set(0.0f, 100.0f, 0.0f);
	// Set the initial angular velocity to pi radians (180 degrees) per second.
	bodyDef.angularVelocity.Set(0.0f, B3_PI, 0.0f);
	b3Body* body = world->CreateBody(bodyDef);
	// Create a unit box positioned at the world origin and 
	// aligned with the world frame.
	b3BoxHull bodyBox;
	bodyBox.SetIdentity();
	// Create the box physics wrapper.
	b3HullShape bodyShape;
	bodyShape.m_hull = &bodyBox;

	std::string nanoPath = "res/models/nanosuit_reflection/nanosuit.obj";
	Model nano(nanoPath);
	//std::string terrainPath = "res/models/sponza_obj/sponza.obj";
	std::string terrainPath = "res/models/sponza_obj/sponza.obj";
	Model terrain(terrainPath);


	b3Capsule capsule = b3Capsule_identity;
	capsule.radius = 2.0f;
	b3CapsuleShape capsuleShape;
	capsuleShape.m_centers[0].Set(0.0f, 0.5f, 0.0f);
	capsuleShape.m_centers[1].Set(0.0f, -0.5f, 0.0f);
	capsuleShape.m_radius = 2.0f;

	// Add the box to the body.
	b3ShapeDef bodyBoxDef;
	bodyBoxDef.shape = &capsuleShape;
	bodyBoxDef.density = 10.0f;
	body->CreateShape(bodyBoxDef);

	//Test objects

	Shader defaultShader("res/shaders/Default.vert", "res/shaders/Default.frag");
	Shader lampShader("res/shaders/lamp.vert", "res/shaders/lamp.frag");
	Shader outlineShader("res/shaders/default.vert", "res/shaders/outline.frag");
	Shader grassShader("res/shaders/default.vert", "res/shaders/grass.frag");
	Shader fbShader("res/shaders/FrameBuffer.vert", "res/shaders/FrameBuffer.frag");
	Shader cubemapShader("res/shaders/Cubemap.vert", "res/shaders/Cubemap.frag");
	Shader reflectionMapShader("res/shaders/DefaultReflective.vert", "res/shaders/DefaultReflective.frag");
	Shader shadowMapDepthShader("res/shaders/Shadowmap.vert", "res/shaders/Shadowmap.frag");
	Shader cubemapShadowDepthShader("res/shaders/CubeDepthMap.vert", "res/shaders/CubeDepthMap.geo", "res/shaders/CubeDepthMap.frag");
	Shader shadowShader("res/shaders/Shadow.vert", "res/shaders/Shadow.frag");
	Shader normalShader("res/shaders/NormalMap.vert", "res/shaders/NormalMap.frag");
	Shader DebugDepthShader("res/shaders/DepthMapDebug.vert", "res/shaders/DepthMapDebug.frag");

	Cube cube("res/textures/green.jpg");
	Cube reflectiveCube;
	Quad quad("res/textures/grass.png");
	Quad shadowMapQuad;
	screenQuad renderQuad;



	std::vector<std::string> faces
	{
		"res/textures/violentdays/violentdays_rt.tga",
		"res/textures/violentdays/violentdays_lf.tga",
		"res/textures/violentdays/violentdays_up.tga",
		"res/textures/violentdays/violentdays_dn.tga",
		"res/textures/violentdays/violentdays_bk.tga",
		"res/textures/violentdays/violentdays_ft.tga"
	};
	Cubemap cubeMap(faces);
	cubeMap.Bind(reflectionMapShader);


	//mvp

	glm::mat4 view = glm::mat4(1.0f);
	view = cam.GetViewMatrix();
	//model
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	//projection
	glm::mat4 projection = glm::mat4(1.0f);
	projection = glm::perspective(cam.Zoom, (float)SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, 100.0f);

	glm::mat4 mvp = glm::mat4(1.0f);
	mvp = projection * view * model;




	float timeValue;
	float offsetValue;

	glm::vec3 cubePositions[10]{
		glm::vec3(0.0f,10.0f,0.0f),
		glm::vec3(1.0f,12.0f,-3.0f),
		glm::vec3(-2.0f,14.0f,-6.0f),
		glm::vec3(5.0f,14.6f,-9.0f),
		glm::vec3(3.0f,15.0f,-12.0f),
		glm::vec3(6.0f,17.0f,0.0f),
		glm::vec3(3.0f,16.0f,-7.70f),
		glm::vec3(-5.0f,14.0f,-6.0f),
		glm::vec3(7.0f,10.6f,-9.0f),
		glm::vec3(-3.0f,10.0f,-15.0f)
	};


	std::vector<glm::vec3> pointLightPositions = {
		glm::vec3(8.7f,  3.52f,  2.0f),
		glm::vec3(12.3f, 3.3f, 4.0f),
		glm::vec3(-4.0f,  3.0f, -2.0f),
		glm::vec3(0.0f,  4.0f, 3.0f)
	};


	DirectionalLight dirLight(glm::vec3(-0.2f, -1.0f, -0.3f), glm::vec3(0.15f, 0.08f,0.061f), glm::vec3(0.96f, 0.48f, 0.27f), glm::vec3(0.91f, 0.31f, 0.101f));
	//DirectionalLight dirLight(glm::vec3(-0.2f, -1.0f, -0.3f), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0));

	//create pointlights
	for (unsigned int i = 0; i < pointLightPositions.size(); i++)
	{
		pointLights.push_back(PointLight(pointLightPositions[i], glm::vec3(0.01, 0.01, 0.01), glm::vec3(1.0, 0.4, 0.2), glm::vec3(1.0, 0.5, 0.2), 0.65f, 1.0f));

	}

	//Imgui debug values
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	ImVec4 lightposition = ImVec4(-2.0f, 4.0f, -1.0f, 1.00f);
	ImVec4 lightambient = ImVec4(0.5f, 0.5f, 0.5f, 1.00f);
	ImVec4 lightcenter = ImVec4(0.0f, 3.5f, 0.0f, 1.00f);
	ImVec4 lightspecular = ImVec4(3.429f, 2.0f, 3.429f, 1.00f);
	ImVec4 lightattenuation = ImVec4(1.0, 0.22f, 0.20f, 1.00f);
	ImVec4 plposition = ImVec4(1.0, 5.0, 2.0, 1.0);
	float lightintensity = 1.0f;
	ImVec4 testposition = ImVec4(0.45f, 2.0f, 0.60f, 1.00f);
	ImVec4 framebufferPosition = ImVec4(0.0f,0.0f,0.0f,0.0f);
	ImVec4 framebufferViewPosition = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	cam.Position = glm::vec3(0.0, 9.0, 10.0);
	float gamma = 1.0f;
	float reflectionIntensity = 1.0f;

	//Framebuffer
	//Frame buffer set up
	unsigned int fbo, framebufferTexture;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glGenTextures(1, &framebufferTexture);
	glBindTexture(GL_TEXTURE_2D, framebufferTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebufferTexture, 0);

	//render buffer object
	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCREEN_WIDTH, SCREEN_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "error with framebuffer" << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Directional Shadow mapping
	unsigned int depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);
	const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
	unsigned int depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	
	//Point light shadowmapping
	//use cubemap to store depth information
	unsigned int depthCubemap, depthCubemapFBO;
	glGenFramebuffers(1, &depthCubemapFBO);
	glGenTextures(1, &depthCubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
	for (unsigned int i = 0; i < 6; i++)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
			SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, depthCubemapFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//Debug pointlight position
	glm::vec3 pointLightPosition;

	//debug omni shadowmaps

	unsigned int debugOmniMap, debugOmniMapFBO;
	glGenFramebuffers(1, &debugOmniMapFBO);
	glGenTextures(1, &debugOmniMap);
	glBindTexture(GL_TEXTURE_2D, debugOmniMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindFramebuffer(GL_FRAMEBUFFER, debugOmniMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, debugOmniMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	/*
	//GameObject stuff
	Object testobject("testObject");
	testobject.addComponent(new DebugEngineComponent());
	testobject.addComponent(new Transform());
	Mesh testMesh = nano.meshes[0];
	testobject.addComponent(new Mesh(testMesh.vertices, testMesh.indices, testMesh.textures));
	*/

	// Loop until the user closes the window 

		while (!glfwWindowShouldClose(window))
		{

			
			//Engine time
			float currentFrame = glfwGetTime();
			deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;

			//Refresh input
			processInput(window);

			//Refresh ui window
			ImGui_ImplGlfwGL3_NewFrame();

			// Perform a time step of the world in this frame.
			world->Step(deltaTime, velocityIterations, positionIterations);
			
			//Clear buffers
			glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
			
			// Directional Shadow map
			//Configure shadow matrix and shader stuff
			float near_plane = 0.1f, far_plane = 100.0f;
			glm::mat4 lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, near_plane, far_plane);
			glm::mat4 lightView = glm::lookAt(glm::vec3(lightposition.x, lightposition.y, lightposition.z),
				glm::vec3(lightcenter.x, lightcenter.y, lightcenter.z),
				glm::vec3(0.0f, 1.0f, 0.0f));
			glm::mat4 lightSpaceMatrix = lightProjection * lightView;

			shadowMapDepthShader.use();
			shadowMapDepthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
			glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
			glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
			glClear(GL_DEPTH_BUFFER_BIT);
			glCullFace(GL_FRONT);

			//Render the scene using the directional map shader
			model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3(0.0f, 3.5f, 0.0f));
			model = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f));
			shadowMapDepthShader.setMat4("model", model);
			terrain.Draw(shadowMapDepthShader);

			//use Bounce to calculate rib's position;
			b3Vec3 position = body->GetPosition();
			b3Quat orientation = body->GetOrientation();
			b3Vec3 axis;
			float32 angle;
			orientation.GetAxisAngle(&axis, &angle);


			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(position.x, position.y, position.z));
			model = glm::rotate(model, angle, glm::vec3(axis.x, axis.y, axis.z));
			model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
			shadowMapDepthShader.setMat4("model", model);
			nano.Draw(shadowMapDepthShader);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			//Directional shadow map complete

			//Omni pointlight shadowmap
			pointLightPosition = glm::vec3(plposition.x, plposition.y, plposition.z);
			glBindFramebuffer(GL_FRAMEBUFFER, depthCubemapFBO);
			glClear(GL_DEPTH_BUFFER_BIT);

			float aspect = (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT;
			glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, near_plane, far_plane);
			
			//depth map for each face of the cubemap texture
			std::vector<glm::mat4> shadowTransforms;
			shadowTransforms.push_back(shadowProj *	glm::lookAt(pointLightPosition, pointLightPosition + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
			shadowTransforms.push_back(shadowProj *	glm::lookAt(pointLightPosition, pointLightPosition + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
			shadowTransforms.push_back(shadowProj *	glm::lookAt(pointLightPosition, pointLightPosition + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
			shadowTransforms.push_back(shadowProj *	glm::lookAt(pointLightPosition, pointLightPosition + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
			shadowTransforms.push_back(shadowProj *	glm::lookAt(pointLightPosition, pointLightPosition + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
			shadowTransforms.push_back(shadowProj *	glm::lookAt(pointLightPosition, pointLightPosition + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));


			//Setup cubemap shadowmap shader
			cubemapShadowDepthShader.use();
			cubemapShadowDepthShader.setMat4("shadowMatrices[0]", shadowTransforms[0]);
			cubemapShadowDepthShader.setMat4("shadowMatrices[1]", shadowTransforms[1]);
			cubemapShadowDepthShader.setMat4("shadowMatrices[2]", shadowTransforms[2]);
			cubemapShadowDepthShader.setMat4("shadowMatrices[3]", shadowTransforms[3]);
			cubemapShadowDepthShader.setMat4("shadowMatrices[4]", shadowTransforms[4]);
			cubemapShadowDepthShader.setMat4("shadowMatrices[5]", shadowTransforms[5]);
			cubemapShadowDepthShader.setFloat("far_plane", far_plane);
			cubemapShadowDepthShader.setVec3("lightPos", pointLightPosition);

			//Draw scene using Cubemap shadowmap shader
			model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3(0.0f, 3.5f, 0.0f));
			model = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f));
			cubemapShadowDepthShader.setMat4("model", model);
			terrain.Draw(cubemapShadowDepthShader);

			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(position.x, position.y, position.z));
			model = glm::rotate(model, angle, glm::vec3(axis.x, axis.y, axis.z));
			model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
			cubemapShadowDepthShader.setMat4("model", model);
			nano.Draw(cubemapShadowDepthShader);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			//point light shadow map complete


			//Render scene normally
			glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glCullFace(GL_BACK);

			//Framebuffer first pass
			glBindFramebuffer(GL_FRAMEBUFFER, fbo);
			glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // | GL_STENCIL_BUFFER_BIT included
			
			defaultShader.use();
			defaultShader.setInt("NOPL", 4);
			defaultShader.setInt("NOSL", 0);
			defaultShader.setVec3("viewPosition", cam.Position);
			defaultShader.setFloat("mat.m_Shininess", 64.0f);

			shadowShader.use();
			shadowShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
			shadowShader.setVec3("lightPosition", glm::vec3(lightposition.x, lightposition.y, lightposition.z));
			shadowShader.setVec3("viewPosition", cam.Position);
			shadowShader.setVec3("pointLightPosition", pointLightPosition);
			shadowShader.setFloat("near_plane", near_plane);
			shadowShader.setFloat("far_plane", far_plane);

			normalShader.use();
			normalShader.setVec3("lightPosition", glm::vec3(lightposition.x, lightposition.y, lightposition.z));
			normalShader.setVec3("viewPosition", cam.Position);
			normalShader.setVec3("pointLightPosition", pointLightPosition);



			//bind directional and cubemap shadow maps to shadow shader
			glActiveTexture(GL_TEXTURE15);
			shadowShader.setInt("shadowMap", 15);
			glBindTexture(GL_TEXTURE_2D, depthMap);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			glActiveTexture(GL_TEXTURE14);
			shadowShader.setInt("pointShadowMap", 14);
			glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);


			reflectionMapShader.use();
			reflectionMapShader.setInt("NOPL", 4);
			reflectionMapShader.setInt("NOSL", 0);
			reflectionMapShader.setFloat("mat.m_Shininess", 64.0f);
			reflectionMapShader.setFloat("reflectionIntensity", reflectionIntensity);
			
			cubeMap.Bind(reflectionMapShader);

			view = cam.GetViewMatrix();
			defaultShader.use();
			dirLight.Bind(defaultShader);

			reflectionMapShader.use();
			dirLight.Bind(reflectionMapShader);

			
			for (unsigned int i = 0; i < pointLights.size(); i++)
			{
				reflectionMapShader.use();
				pointLights[i].Bind(reflectionMapShader, i);
				defaultShader.use();
				pointLights[i].Bind(defaultShader, i);
			}

			// Draw with reflection shader
			/*
			model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3(0.0f, 3.5f, 5.0f));
			model = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f));
			reflectionMapShader.use();
			reflectionMapShader.setMat4("model", model);
			reflectionMapShader.setMat4("view", view);
			reflectionMapShader.setMat4("projection", projection);
			reflectionMapShader.setVec3("viewPosition", cam.Position);
			terrain.Draw(reflectionMapShader);


			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(position.x, position.y, position.z));
			model = glm::rotate(model, angle, glm::vec3(axis.x, axis.y, axis.z));
			model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
			reflectionMapShader.use();
			reflectionMapShader.setMat4("model", model);
			reflectionMapShader.setMat4("view", view);
			reflectionMapShader.setMat4("projection", projection);
			reflectionMapShader.setVec3("viewPosition", cam.Position);
			nano.Draw(reflectionMapShader);

			*/
			
			//Draw with shadow shader
			/*
			model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3(0.0f, 3.5f, 0.0f));
			model = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f));
			shadowShader.use();
			shadowShader.setMat4("model", model);
			shadowShader.setMat4("view", view);
			shadowShader.setMat4("projection", projection);
			terrain.Draw(shadowShader);


			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(position.x, position.y, position.z));
			model = glm::rotate(model, angle, glm::vec3(axis.x, axis.y, axis.z));
			model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
			shadowShader.use();
			shadowShader.setMat4("model", model);
			shadowShader.setMat4("view", view);
			shadowShader.setMat4("projection", projection);
			
			nano.Draw(shadowShader);
			
			*/

			model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3(0.0f, 3.5f, 0.0f));
			model = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f));
			normalShader.use();
			normalShader.setMat4("model", model);
			normalShader.setMat4("view", view);
			normalShader.setMat4("projection", projection);
			terrain.Draw(normalShader);


			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(position.x, position.y, position.z));
			model = glm::rotate(model, angle, glm::vec3(axis.x, axis.y, axis.z));
			model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
			normalShader.use();
			normalShader.setMat4("model", model);
			normalShader.setMat4("view", view);
			normalShader.setMat4("projection", projection);

			nano.Draw(normalShader);

			lampShader.use();
			model = glm::mat4(1.0);
			model = glm::translate(model, pointLightPosition);
			model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
			lampShader.setMat4("model", model);
			lampShader.setMat4("view", view);
			lampShader.setMat4("projection", projection);
			reflectiveCube.Draw(lampShader);

			cubemapShader.use();
			cubeMap.Draw(cubemapShader, view, projection);



			//Framebuffer time
			// second pass
			glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
			glDisable(GL_DEPTH_TEST);
			glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

			fbShader.use();
			fbShader.setFloat("gamma", gamma);
			renderQuad.Draw(fbShader, "screenTexture", framebufferTexture);
			glEnable(GL_DEPTH_TEST);

			glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
			glDisable(GL_DEPTH_TEST);
			glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

			DebugDepthShader.use();
			DebugDepthShader.setFloat("scaler", 0.25f);
			DebugDepthShader.setFloat("offsetX", -0.75f);
			DebugDepthShader.setFloat("offsetY", -0.75f);
			renderQuad.Draw(DebugDepthShader, "depthMap", depthMap);


			glEnable(GL_DEPTH_TEST);

			//ui stuff
			// 1. Show a simple window.
			// Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets automatically appears in a window called "Debug".
			//New menus can be created this way.
			{
				if (ImGui::Begin("STUFF")) {

					if (ImGui::BeginMenu("Debug Stuff"))
					{
						//std::cout << "menu working, fps wasted" << std::endl;

						static float f = 0.0f;
						static int counter = 0;
						ImGui::Text("Debug Menu");

						// Display some text (you can use a format string too)        // Edit 1 float using a slider from 0.0f to 1.0f    
						ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

						ImGui::SliderFloat("Gamma", &gamma, -2, 2);
						ImGui::SliderFloat("ReflectionIntensity", &reflectionIntensity, 0, 10);
						ImGui::SliderFloat3("DirLight Position", &lightposition.x, -40, 40);
						ImGui::SliderFloat3("DirLight Center", &lightcenter.x, -40, 40);
						ImGui::SliderFloat3("PointLight position", &plposition.x, -20, 20);

						ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

						if (ImGui::Button("addForce"))
						{
							body->ApplyForce(b3Vec3(20000, 100000.0f, 0), b3Vec3(1, 0, 0), true);
						}

						ImGui::EndMenu();
					}

					if (ImGui::BeginMenu("GameObjects"))
					{
						ImGui::EndMenu();
					}

					if (ImGui::BeginMenu("Lights"))
					{
						if (ImGui::BeginMenu("Point Lights"))
						{
							ImGui::EndMenu();
						}
						if (ImGui::BeginMenu("Spot Lights"))
						{
							ImGui::EndMenu();
						}
						ImGui::EndMenu();
					}

					if (ImGui::BeginMenu("Sound"))
					{
						if (ImGui::BeginMenu("Pure Data"))
						{
							ImGui::EndMenu();
						}
						if (ImGui::BeginMenu("Sample Based"))
						{
							ImGui::EndMenu();
						}
						ImGui::EndMenu();
					}

				}
				ImGui::End();

			}

			//ui render
			ImGui::Render();
			ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());



			/* Swap front and back buffers */
			glfwSwapBuffers(window);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

			/* Poll for and process events */
			glfwPollEvents();
		}

	// Cleanup
	ImGui_ImplGlfwGL3_Shutdown();
	ImGui::DestroyContext();

	glDeleteFramebuffers(1, &fbo);

	glfwTerminate();
	return 0;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cam.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cam.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cam.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cam.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
			cam.canMove = true;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE)
	{
		cam.canMove = false;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) 
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	cam.ProcessMouseMovement(xoffset, yoffset);
};

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	cam.ProcessMouseScroll(yoffset);
};
