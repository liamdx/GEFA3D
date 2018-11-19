
#include "AnimationExample.h"

//namespace example {

float CAM_RADIUS = 5.0f;

OrbitCamera BoneCam(glm::vec3(1.0, 6.0, -2.0));

glm::vec3 CHAR_LOCATION = glm::vec3(0, 4.0, 2);

float Speed = 5.0f;

static Animation Anim_Test_Walk("Walk", FramesToTime(glm::vec2(0,200)), 1);
static Animation Anim_Test_Idle("Idle", FramesToTime(glm::vec2(51, 110)), 3);

AnimationTest::~AnimationTest()
{

}

void AnimationTest::init()
{
	

	
}

void AnimationTest::start()
{
	projection = glm::perspective(90.0f, (float)1280 / 720, 0.1f, 100.0f);

	cyborg.AddAnimation(Anim_Test_Walk);
	cyborg.AddAnimation(Anim_Test_Idle);

	for (int i = 0; i < cyborg.skeleton.boneMats.size(); i++)
	{
		std::stringstream ss;
		boneShader.use();
		boneShader.setMat4(ss.str(), cyborg.skeleton.boneMats[i]);
		ss.clear();

	}

}

void AnimationTest::earlyUpdate(float deltaTime)
{
	BoneCam.heightOffset = 2.0f;
	
	
	
	if (input.GetKeyW())
	{
		CHAR_LOCATION += glm::vec3(0, 0, 1) *deltaTime * Speed;
		
	}

	if (input.GetKeyS())
	{
		CHAR_LOCATION -= glm::vec3(0, 0, 1) * deltaTime * Speed;
	}

	if (input.GetKeyA())
	{
		CHAR_LOCATION += glm::vec3(1, 0, 0) * deltaTime * Speed;
	}

	if (input.GetKeyD())
	{
		CHAR_LOCATION -= glm::vec3(1, 0, 0) * deltaTime * Speed;
	}

	if (input.GetRightClick())
	{
		BoneCam.canMove = true;
		input.GetMouseMovement();

		float deltaX = input.xpos - lastX;
		float deltaY = input.ypos - lastY;

		BoneCam.ProcessMouseMovement(input.xpos, -input.ypos);
		cyborg.PlayAnimation(cyborg.animations[0], true, true);

	}
	else
	{
		BoneCam.canMove = false;
	}

	if (input.GetLeftClick())
	{
		cyborg.PlayAnimation(cyborg.animations[1], true, true);

	}

	BoneCam.Target = CHAR_LOCATION;
	BoneCam.radius = CAM_RADIUS;
	view = BoneCam.GetViewMatrix();

}

void AnimationTest::update(float deltaTime)
{




	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::vec3 currentPointLightPosition = glm::vec3(pointLightPosition.x, pointLightPosition.y, pointLightPosition.z);


	boneShader.use();
	boneShader.setMat4("projection", projection);
	boneShader.setMat4("view", view);
	boneShader.setVec3("viewPosition", BoneCam.Position);
	boneShader.setVec3("pointLightPosition", currentPointLightPosition);
	
	for (int i = 0; i < cyborg.skeleton.boneMats.size(); i++)
	{
		std::stringstream ss;
		ss << "gBones[" << i << "]";
		boneShader.setMat4(ss.str(), cyborg.skeleton.boneMats[i]);
		ss.clear();

	}

	model = glm::mat4(1.0f);
	model = glm::translate(model, CHAR_LOCATION);
	boneShader.setMat4("model", model);
	cyborg.Draw(boneShader, deltaTime);



	lastX = input.xpos;
	lastY = input.ypos;
}


void AnimationTest::UI()
{
	if (ImGui::Begin("AnimationTest Example")) {

		if (ImGui::BeginMenu("Example Options"))
		{
			ImGui::DragFloat3("Point Light Position: ", &pointLightPosition.x, 0.1f, -40, 40);
			ImGui::DragFloat3("Cyborg Position: ", &cyborgLocation.x, 0.1f, -40, 40);
			ImGui::DragFloat3("Camera Position: ", &cameraLocation.x, 0.1f, -40, 40);
			ImGui::SliderFloat("Camera Radius", &CAM_RADIUS, 2.0, 15.0);
			
			if (ImGui::Button("Play walk animation", ImVec2(170, 30)))
			{
				cyborg.PlayAnimation(cyborg.animations[0], true, true);
			
			}

			if (ImGui::Button("Play idle animation", ImVec2(170, 30)))
			{
				cyborg.PlayAnimation(cyborg.animations[1], true, true);

			}

			ImGui::EndMenu();
		}

		ImGui::End();
	}
}
//}