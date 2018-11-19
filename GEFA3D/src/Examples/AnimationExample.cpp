
#include "AnimationExample.h"

//namespace example {

float CAM_RADIUS = 5.0f;
Camera BoneCam(glm::vec3(1.0, 6.0, -2.0));
glm::vec3 CHAR_LOCATION = glm::vec3(0.0, 4.0, 2.0);
float Speed = 10.0f;


AnimationTest::~AnimationTest()
{

}

void AnimationTest::init()
{
	

	
}

void AnimationTest::start()
{
	projection = glm::perspectiveFov(90.0f, 1600.0f, 900.0f, 0.1f, 200.0f);
	
	anim.LoadMesh("res/models/boblampclean.md5mesh");

	BoneCam.MovementSpeed = Speed;
	gBoneLocations.clear();

	for (int i = 0; i < anim.NumBones(); i++)
	{
		std::stringstream currentBoneLocation;
		currentBoneLocation << "gBones[" << i << "]";
		gBoneLocations.push_back(boneShader.getMat4Location(currentBoneLocation.str()));
		currentBoneLocation.clear();
	}
}
void AnimationTest::earlyUpdate(float deltaTime)
{

	if (input.GetKeyW())
	{
		BoneCam.ProcessKeyboard(FORWARD, deltaTime);
		
	}

	if (input.GetKeyS())
	{
		BoneCam.ProcessKeyboard(BACKWARD, deltaTime);
	}

	if (input.GetKeyA())
	{
		BoneCam.ProcessKeyboard(LEFT, deltaTime);
	}

	if (input.GetKeyD())
	{
		BoneCam.ProcessKeyboard(RIGHT, deltaTime);
	}

	if (input.GetRightClick())
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		BoneCam.canMove = true;
		input.GetMouseMovement();
		float deltaX = input.xpos - lastX;
		float deltaY = input.ypos - lastY;
		BoneCam.ProcessMouseMovement(input.xpos, -input.ypos);
	}

	else
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
	
		view = BoneCam.GetViewMatrix();

}


void AnimationTest::update(float deltaTime)
{
	boneShader.use();

	runningTime += 1 * deltaTime;

	if (runningTime >= anim.currentAnimationLengthInSeconds)
	{
		runningTime = 0.0f;
	}

	anim.BoneTransform(runningTime, transforms);

	for (int i = 0; i < anim.NumBones() ; i++)
	{
		SetBoneTransformId(boneShader, gBoneLocations[i], transforms[i]);
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	model = glm::mat4(1.0f);
	model = glm::translate(model, CHAR_LOCATION);
	model = glm::scale(model,glm::vec3(0.3));
	boneShader.setMat4("model", model);
	boneShader.setMat4("view", view);
	boneShader.setMat4("projection", projection);
	

	//glActiveTexture(GL_TEXTURE0);
	//boneShader.setInt("texUnit", 0);
	//glBindTexture(GL_TEXTURE_2D, textureId);

	anim.Draw(boneShader);

	transforms.clear();

	/*anim2.BoneTransform(runningTime, transforms);
	model = glm::mat4(1.0f);
	model = glm::translate(model, CHAR_LOCATION + glm::vec3(-15.0, 2.0, 5.0));
	model = glm::scale(model, glm::vec3(0.3));
	boneShader.setMat4("model", model);

	for (int i = 0; i < anim2.NumBones(); i++)
	{
		SetBoneTransform(boneShader, i, transforms[i]);
	}

	anim2.Draw(boneShader);
	transforms.clear();

	anim3.BoneTransform(runningTime, transforms);
	model = glm::mat4(1.0f);
	model = glm::translate(model, CHAR_LOCATION + glm::vec3(15.0, 2.0, -5.0));
	model = glm::scale(model, glm::vec3(0.3));
	boneShader.setMat4("model", model);

	for (int i = 0; i < anim3.NumBones(); i++)
	{
		SetBoneTransform(boneShader, i, transforms[i]);
	}

	anim3.Draw(boneShader);
	transforms.clear();*/


	//glBindTexture(GL_TEXTURE_2D, 0);
	

	lastX = input.xpos;
	lastY = input.ypos;

	skyboxShader.use();
	skyboxShader.setMat4("projection", projection);
	skyboxShader.setMat4("view", view);
	skyboxShader.setInt("cubemap", 0);
	mcSkybox.Draw(skyboxShader, view, projection);

	
 }


void AnimationTest::UI()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{

		}
		if (ImGui::BeginMenu("Edit"))
		{
			if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
			if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
			ImGui::Separator();
			if (ImGui::MenuItem("Cut", "CTRL+X")) {}
			if (ImGui::MenuItem("Copy", "CTRL+C")) {}
			if (ImGui::MenuItem("Paste", "CTRL+V")) {}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	if (ImGui::Begin("AnimationTest Example")) {

		static bool openModelDialog = false;
		static bool openDiffuseDialog = false;
		static int diffuseCounter;

		if (ImGui::Button("Load Model"))
		{
			openModelDialog = true;
		}

		static std::string filePathName = "";
		static std::string path = "";
		static std::string fileName = "";
		static std::string filter = "";

		if (openModelDialog)
		{
			//if (ImGuiFileDialog::Instance()->FileDialog("Choose File", ".fbx\0.FBX\0.dae\0\0", ".", ""))
			if (ImGuiFileDialog::Instance()->FileDialog("Choose File"))
			{
				if (ImGuiFileDialog::Instance()->IsOk == true)
				{
					filePathName = ImGuiFileDialog::Instance()->GetFilepathName();
					path = ImGuiFileDialog::Instance()->GetCurrentPath();
					fileName = ImGuiFileDialog::Instance()->GetCurrentFileName();
					filter = ImGuiFileDialog::Instance()->GetCurrentFilter();
				}
				else
				{
					filePathName = "";
					path = "";
					fileName = "";
					filter = "";
				}
				openModelDialog = false;

				anim.LoadMesh(filePathName);
				gBoneLocations.clear();
				for (int i = 0; i < anim.NumBones(); i++)
				{
					std::stringstream currentBoneLocation;
					currentBoneLocation << "gBones[" << i << "]";
					gBoneLocations.push_back(boneShader.getMat4Location(currentBoneLocation.str()));
					currentBoneLocation.clear();
				}

				filePathName = "";
				path = "";
				fileName = "";
				filter = "";
			}
		}

		if (ImGui::BeginMenu("Model Options"))
		{
			for (unsigned int i = 0; i < anim.m_Entries.size(); i++)
			{

			std::stringstream na;
			na << "Mesh " << i;
			if (ImGui::BeginMenu(na.str().c_str()))
				{
					if (ImGui::Button("Load Diffuse"))
					{
						diffuseCounter = i;
						openDiffuseDialog = true;

					} 

					
					ImGui::EndMenu();
				}

				

			na.clear();
			}
			
			ImGui::EndMenu();
		}

		if (openDiffuseDialog == true)
		{
			if (ImGuiFileDialog::Instance()->FileDialog("Choose File"))
			{
				if (ImGuiFileDialog::Instance()->IsOk == true)
				{
					filePathName = ImGuiFileDialog::Instance()->GetFilepathName();
					path = ImGuiFileDialog::Instance()->GetCurrentPath();
					fileName = ImGuiFileDialog::Instance()->GetCurrentFileName();
					filter = ImGuiFileDialog::Instance()->GetCurrentFilter();

					Texture t;
					unsigned int id = Model::CommonTextureLoad(filePathName);
					t.t_Id = id;
					t.t_Path = filePathName;
					t.t_Type = "diffuse";
					anim.LoadTextureToMesh(diffuseCounter, t);
				}
				else
				{
					filePathName = "";
					path = "";
					fileName = "";
					filter = "";
				}





				filePathName = "";
				path = "";
				fileName = "";
				filter = "";
				openDiffuseDialog = false;
			}

		}


		if (ImGui::Button("Play Anim 1"))
		{
			anim.SetAnimation(0);
		}

		if (ImGui::Button("Play Anim 2"))
		{
			anim.SetAnimation(1);
		}

		if (ImGui::Button("Play Anim 3"))
		{
			anim.SetAnimation(2);
		}


		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		
	}
	ImGui::End();

}

	
	

