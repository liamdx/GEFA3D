#define _CRT_SECURE_NO_WARNINGS

//#include "Examples\ShadowExample.h"
//#include "Examples\NormalTestExample.h"
//#include "Examples\PBRTest.h"
//#include "Examples\SoundExample.h"
#include "Examples\AnimationExample.h"
//#include "Examples\ReflectionExample.h"

//Engine Time
float deltaTime = 0.0f;
float lastFrame = 0.0f;
const float timeStep = 1 / 60.0f;

//camera;
float lastX;
float lastY;
bool firstMouse = true;

//Forward Decalrations
void framebuffer_size_callback(GLFWwindow* window, int width, int height);


const unsigned int SCREEN_WIDTH = 1600;
const unsigned int SCREEN_HEIGHT = 900;
float gamma = 1.0f;

// We don't care for a profiler. This definition does nothing.
void b3BeginProfileScope(const char* name)
{

}

// We don't care for a profiler. This definition does nothing.
void b3EndProfileScope()
{

}

void SetImGuiStyle();

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

	SetImGuiStyle();

	ImGuiIO& io = ImGui::GetIO();
	ImFont* font = io.Fonts->AddFontFromFileTTF("res/font/Rubik-Light.ttf", 14.0f);

	YSE::System().init();

	//Enable depth
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glfwSwapInterval(0);
	Shader fbShader("res/shaders/FrameBuffer.vert", "res/shaders/FrameBuffer.frag");

	//Mouse input handle

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	//glfwSetCursorPosCallback(window, test.input.mouse_callback);


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

	

	//Cube reflectiveCube;
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
	
	/*
	std::vector<std::string> faces
	{
		"res/textures/starfield/starfield_rt.tga",
		"res/textures/starfield/starfield_lf.tga",
		"res/textures/starfield/starfield_up.tga",
		"res/textures/starfield/starfield_dn.tga",
		"res/textures/starfield/starfield_bk.tga",
		"res/textures/starfield/starfield_ft.tga"
	};
	*/



	// Tests
	AnimationTest test(window, faces);
	//ShadowTest test(window);
	//NormalTest test(window);
	//PBRTest test(window, faces);
	//SoundTest test(window);
	//ReflectionTest test(window, faces);




	float timeValue;
	float offsetValue;




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

	glm::vec3 pointLightPosition;


	/*
	//GameObject stuff
	Object testobject("testObject");
	testobject.addComponent(new DebugEngineComponent());
	testobject.addComponent(new Transform());
	Mesh testMesh = nano.meshes[0];
	testobject.addComponent(new Mesh(testMesh.vertices, testMesh.indices, testMesh.textures));
	*/

	// Loop until the user closes the window 

	test.init();

	test.start();

		while (!glfwWindowShouldClose(window))
		{
			
			//Engine time
			float currentFrame = glfwGetTime();
			deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;


			//Refresh ui window
			ImGui_ImplGlfwGL3_NewFrame();

			// Perform a time step of the world in this frame.
			world->Step(deltaTime, velocityIterations, positionIterations);
			
			//Clear buffers
			glClearColor(0.0, 0.0f,0.0f,0.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
			
			//use Bounce to calculate rib's position;
			b3Vec3 position = body->GetPosition();
			b3Quat orientation = body->GetOrientation();
			b3Vec3 axis;
			float32 angle;
			orientation.GetAxisAngle(&axis, &angle);
			

			test.earlyUpdate(deltaTime);


			//Render scene normally
			glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
			glCullFace(GL_BACK);

			
			//Framebuffer first pass
			glBindFramebuffer(GL_FRAMEBUFFER, fbo);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // | GL_STENCIL_BUFFER_BIT included

			test.update(deltaTime);

			


			

			//Framebuffer time
			// second pass
			glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
			glDisable(GL_DEPTH_TEST);
			glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

			fbShader.use();
			fbShader.setFloat("gamma", gamma);
			renderQuad.Draw(fbShader, "screenTexture", framebufferTexture);

			glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default

			glEnable(GL_DEPTH_TEST);


			test.UI();

			//ui render
			
			ImGui::Render();
			
			ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());


			YSE::System().update();
			/* Swap front and back buffers */
			glfwSwapBuffers(window);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

			/* Poll for and process events */
			glfwPollEvents();
		}

	// Cleanup
	ImGui_ImplGlfwGL3_Shutdown();
	ImGui::DestroyContext();
	YSE::System().close();
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

void SetImGuiStyle()
{
	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.44f, 0.44f, 0.44f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.11f, 0.11f, 0.11f, 0.94f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.38f, 0.38f, 0.38f, 0.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.28f, 0.28f, 0.28f, 0.94f);
	colors[ImGuiCol_Border] = ImVec4(1.00f, 0.72f, 0.72f, 0.50f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.41f, 0.41f, 0.41f, 0.54f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.00f, 0.00f, 0.00f, 0.40f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.00f, 0.00f, 0.00f, 0.67f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.00f, 0.62f, 1.00f, 1.00f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.38f, 0.94f, 0.99f, 1.00f);
	colors[ImGuiCol_Button] = ImVec4(0.41f, 0.87f, 1.00f, 0.40f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.38f, 0.52f, 1.00f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(1.00f, 0.57f, 0.57f, 0.31f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.98f, 0.26f, 0.26f, 0.80f);
	colors[ImGuiCol_HeaderActive] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_Separator] = ImVec4(1.00f, 1.00f, 1.00f, 0.50f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.91f, 0.88f, 0.16f, 0.25f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.67f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(1.00f, 0.91f, 0.46f, 0.95f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.35f);
	colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);


}