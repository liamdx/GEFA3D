
#include "PBRTest.h"

//namespace example {


OrbitCamera TestCam(glm::vec3(1.0, 6.0, -2.0));

glm::vec3 cubeLocation = glm::vec3(0, 4.0, 2);

glm::vec3 pointLightPositions[4];
glm::vec3 pointLightColors[4];
ImVec4 colors[4];
ImVec4 positions[4];
ImVec4 scale = ImVec4(0.03,0.03,0.03,1.0);
ImVec4 Offset;
float camRadius = 12.0f;
float OrbitSpeed = 0.5f;


PBRTest::~PBRTest()
{

}

void PBRTest::init()
{

}

void PBRTest::start()
{

	projection = glm::perspective(90.0f, (float)1280 / 720, 0.1f, 100.0f);
	//albedo = Model::CommonTextureLoad("res/textures/gun/Cerberus_A.tga");
	//normal = Model::CommonTextureLoad("res/textures/gun/Cerberus_N.tga");
	//metallic = Model::CommonTextureLoad("res/textures/gun/Cerberus_M.tga");
	//roughness = Model::CommonTextureLoad("res/textures/gun/Cerberus_R.tga");
	//ao = Model::CommonTextureLoad("res/textures/gun/Cerberus_AO.tga");
	positions[0] = ImVec4(0, 9, 4, 0);
	positions[1] = ImVec4(0, 10, 14, 0);
	positions[2] = ImVec4(7, 10, -7, 0);
	positions[3] = ImVec4(-7, 13, -4, 0);
	colors[0] = ImVec4(7, 10, 7, 0);
	colors[1] = ImVec4(12, 7, 6, 0);
	colors[2] = ImVec4(7, 7, 17, 0);
	colors[3] = ImVec4(25, 7, 7, 0);
	/*albedo = Model::CommonTextureLoad("res/models/Lantern/textures/lantern_Base_Color.jpg");
	normal = Model::CommonTextureLoad("res/models/Lantern/textures/lantern_Normal_OpenGL.jpg");
	metallic = Model::CommonTextureLoad("res/models/Lantern/textures/lantern_Metallic.jpg");
	roughness = Model::CommonTextureLoad("res/models/Lantern/textures/lantern_Roughness.jpg");
	ao = Model::CommonTextureLoad("res/models/Lantern/textures/lantern_Mixed_AO.jpg");*/

	albedo = Model::CommonTextureLoad("res/models/xwing/Diffuse.png");
	normal = Model::CommonTextureLoad("res/models/xwing/NormalMap.png");
	metallic = Model::CommonTextureLoad("res/models/xwing/METALLIC.png");
	roughness = Model::CommonTextureLoad("res/models/xwing/Roughness.png");
	ao = Model::CommonTextureLoad("res/models/xwing/AO.png");

	PBRShader.use();
	PBRShader.setMat4("projection", projection);
	
	PBRShader.setInt("albedoMap", 0);
	PBRShader.setInt("normalMap", 1);
	PBRShader.setInt("metallicMap", 2);
	PBRShader.setInt("roughnessMap", 3);
	PBRShader.setInt("aoMap", 4); 


}


void PBRTest::earlyUpdate(float deltaTime)
{
	glm::vec3 cOffset = glm::vec3(Offset.x, Offset.y, Offset.z);
	TestCam.Target = cubeLocation + cOffset;
	TestCam.radius = camRadius;
	

	if (input.GetRightClick())
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		TestCam.canMove = true;
		input.GetMouseMovement();

		float deltaX = input.xpos - lastX ;
		float deltaY = input.ypos - lastY ;

		//UPOFFSET = 5 + TestCam.phi;

		TestCam.upOffset = 1.0f;

		TestCam.ProcessMouseMovement(input.xpos * OrbitSpeed, -input.ypos * OrbitSpeed, deltaTime);

	}
	else {
		TestCam.canMove = false;
		TestCam.ProcessMouseMovement(input.xpos * OrbitSpeed, -input.ypos * OrbitSpeed, deltaTime);
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		lastX = 0.0f;
		lastY = 0.0f;
	}
	

	
	view = TestCam.GetViewMatrix();

	

}

void PBRTest::update(float deltaTime)
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	skyboxShader.use();
	skyboxShader.setMat4("projection", projection);
	skyboxShader.setMat4("view", view);
	skyboxShader.setInt("cubemap", 0);



	for (unsigned int i = 0; i < 4; i++)
	{
		std::ostringstream oss;
		std::ostringstream osb;

		oss << "lightPositions[" << i << "]";
		std::string currentLightUniform = oss.str();

		oss.clear();

		osb << "lightColors[" << i << "]";
		std::string currentColorUniform = osb.str();
		osb.clear();


		PBRShader.use();
		pointLightPositions[i] = glm::vec3(positions[i].x, positions[i].y, positions[i].z);
		pointLightColors[i] = glm::vec3(colors[i].x, colors[i].y, colors[i].z);

		PBRShader.setVec3(currentLightUniform, pointLightPositions[i]);
		PBRShader.setVec3(currentColorUniform, pointLightColors[i]);

		currentLightUniform.clear();
		currentColorUniform.clear();

	}


	PBRShader.use();

	PBRShader.setMat4("view", view);
	PBRShader.setVec3("camPos", TestCam.Position);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, albedo);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normal);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, metallic);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, roughness);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, ao);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(cubeLocation.x, cubeLocation.y, cubeLocation.z));
	model = glm::scale(model, glm::vec3(scale.x, scale.y, scale.z));
	//model = glm::rotate(model, 90.0f, glm::vec3(0, 0, 1));
	PBRShader.setMat4("model", model);

	cyborg.TestDraw(PBRShader);

	skybox.Draw(skyboxShader, view, projection);

	lastX = input.xpos;
	lastY = input.ypos;
}


void PBRTest::UI()
{
	if (ImGui::Begin("PBRTest Example")) {

		if (ImGui::BeginMenu("Example Options"))
		{
			ImGui::DragFloat3("Model Position: ", &cubeLocation.x, 0.1f, -50, 50);

			ImGui::DragFloat3("Point Light Color1", &colors[0].x, 0.1f, 0, 255);
			ImGui::DragFloat3("Point Light Color2", &colors[1].x, 0.1f, 0, 255);
			ImGui::DragFloat3("Point Light Color3", &colors[2].x, 0.1f, 0, 255);
			ImGui::DragFloat3("Point Light Color4", &colors[3].x, 0.1f, 0, 255);

			ImGui::DragFloat3("Point Light Position1", &positions[0].x, 0.1f, -50, 50);
			ImGui::DragFloat3("Point Light Position2", &positions[1].x, 0.1f, -50, 50);
			ImGui::DragFloat3("Point Light Position3", &positions[2].x, 0.1f, -50, 50);
			ImGui::DragFloat3("Point Light Position4", &positions[3].x, 0.1f, -50, 50);
		
			ImGui::DragFloat3("Model scale", &scale.x, 0.1f, -50, 50);
			ImGui::DragFloat3("Cam Target offset", &Offset.x, 0.1f, -50, 50);
			ImGui::SliderFloat("Radius", &camRadius, 2, 24);
			ImGui::SliderFloat("Orbit Speed", &OrbitSpeed, -25, 25);

			ImGui::EndMenu();
		}

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		ImGui::End();
	}

}
//}