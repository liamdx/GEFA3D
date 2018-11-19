
#include "ShadowExample.h"

OrbitCamera Cam(glm::vec3(1.0, 6.0, -2.0));

float ccradius = 25.0f;


void ShadowTest::init()
{

}

void ShadowTest::start()
{
	projection = glm::perspective(90.0f, (float)1280 / 720, near_plane, far_plane);
	glGenFramebuffers(1, &depthMapFBO);
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
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

	//Point light cubemap complete
	
}

void ShadowTest::earlyUpdate(float deltaTime)
{
	Cam.Target = glm::vec3(position.x,position.y,position.z);
	Cam.radius = ccradius;

	if (input.GetRightClick())
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		Cam.canMove = true;
		input.GetMouseMovement();

		float deltaX = input.xpos - lastX;
		float deltaY = input.ypos - lastY;

		//UPOFFSET = 5 + TestCam.phi;

		Cam.upOffset = 1.0f;

		Cam.ProcessMouseMovement(input.xpos * OrbitSpeed, -input.ypos * OrbitSpeed, deltaTime);

	}
	else {
		Cam.canMove = false;
		Cam.ProcessMouseMovement(input.xpos * OrbitSpeed, -input.ypos * OrbitSpeed, deltaTime);
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		lastX = 0.0f;
		lastY = 0.0f;
	}



	view = Cam.GetViewMatrix();

	// Directional Shadow map
	//Configure shadow matrix and shader stuff
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::vec3 currentCameraLocation = glm::vec3(cameraLocation.x, cameraLocation.y, cameraLocation.z);
	Cam.Position = currentCameraLocation;
	view = Cam.GetViewMatrix();
	glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
	glm::mat4 lightView = glm::lookAt(glm::vec3(lightposition.x, lightposition.y, lightposition.z),
		glm::vec3(lightcenter.x, lightcenter.y, lightcenter.z),
		glm::vec3(0.0f, 1.0f, 0.0f));
	lightSpaceMatrix = lightProjection * lightView;

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



	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(position.x, position.y, position.z));
	shadowMapDepthShader.setMat4("model", model);
	cyborg.Draw(shadowMapDepthShader);
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
	cubemapShadowDepthShader.setMat4("model", model);
	cyborg.Draw(cubemapShadowDepthShader);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	


}

void ShadowTest::update(float deltaTime)
{

	//point light shadow map complete
	shadowShader.use();
	shadowShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
	shadowShader.setVec3("lightPosition", glm::vec3(lightposition.x, lightposition.y, lightposition.z));
	shadowShader.setVec3("viewPosition", Cam.Position);
	shadowShader.setVec3("pointLightPosition", pointLightPosition);
	shadowShader.setFloat("near_plane", near_plane);
	shadowShader.setFloat("far_plane", far_plane);




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

	glActiveTexture(GL_TEXTURE0);

	//Draw with shadow shader
	
	model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(0.0f, 3.5f, 0.0f));
	model = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f));
	shadowShader.use();
	shadowShader.setMat4("model", model);
	shadowShader.setMat4("view", view);
	shadowShader.setMat4("projection", projection);
	shadowShader.setVec3("viewPosition", Cam.Position);
	terrain.Draw(shadowShader);


	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(position.x, position.y, position.z));
	shadowShader.use();
	shadowShader.setMat4("model", model);
	shadowShader.setMat4("view", view);
	shadowShader.setMat4("projection", projection);
	shadowShader.setVec3("viewPosition", Cam.Position);

	cyborg.Draw(shadowShader);


}


void ShadowTest::UI()
{
	if (ImGui::Begin("Shadow Example")) {

		if (ImGui::BeginMenu("Example Options"))
		{
			ImGui::DragFloat3("Cyborg Position: ", &position.x,0.1f, -20, 20);
			ImGui::DragFloat3("Camera Position: ", &cameraLocation.x, 0.1f, -20, 20);
			ImGui::DragFloat3("DirLight Position", &lightposition.x, 0.1f, -20, 20);
			ImGui::DragFloat3("DirLight Center", &lightcenter.x, 0.1f, -20, 20);
			ImGui::DragFloat3("PointLight position", &plposition.x, 0.1f, -20, 20);
			ImGui::DragFloat("Cam Radius", &ccradius, 0.1f, 5.0f, 30.0f);
			ImGui::EndMenu();
		}
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		ImGui::End();
	}

	

}
//}