#pragma once

#include "Common.h"



// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class OrbitCamera
{
public:
	// Camera Attributes
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;

	glm::vec3 Target;

	// Euler Angles
	float Yaw, Pitch;
	float oldX, oldY;
	float theta, phi;

	// Camera options
	float MovementSpeed;
	float MouseSensitivity;
	float Zoom;
	float upOffset;

	float radius = 10.0f;
	bool canMove = false;
	float heightOffset = 0.0f;

	// Constructor with vectors
	OrbitCamera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		Position = position;
		WorldUp = up;
		Yaw = yaw;
		Pitch = pitch;
		Target = glm::vec3(1.0);
		updateCameraVectors();
	}
	// Constructor with scalar values
	OrbitCamera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		Position = glm::vec3(posX, posY, posZ);
		WorldUp = glm::vec3(upX, upY, upZ);
		Yaw = yaw;
		Pitch = pitch;
		Target = glm::vec3(1.0);
		updateCameraVectors();
	}

	// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
	glm::mat4 GetViewMatrix()
	{
		Target.y += heightOffset;

		
			/*Position.x = radius * sinf(theta * (M_PI / 180)) * acosf((phi) * (M_PI / 180));

			float yCalc = acosf((phi) * (M_PI / 180));

			Position.y = radius * yCalc;
			Position.z = radius * -cosf(theta * (M_PI / 180)) * acosf((phi) * (M_PI / 180));*/

			
			Position.x = radius * sin(phi) * cos(theta);
			Position.y = radius * cos(phi);
			Position.z = radius * sin(phi) * sin(theta);
		
		
		
		return glm::lookAt(Target + Position, Target + Up * upOffset, Up);
	}

	// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
	void ProcessKeyboard(Camera_Movement direction, float deltaTime)
	{
		

	}

	// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
	void ProcessMouseMovement(float xoffset, float yoffset, float deltaTime, GLboolean constrainPitch = true)
	{
		xoffset *= MouseSensitivity;
		yoffset *= MouseSensitivity;


		if (canMove)
		{
			

			theta += (xoffset - oldX) * 0.01f *deltaTime;
			phi += (yoffset - oldY) * 0.01f * deltaTime;

			if (theta >= 3.125f)
			{
				theta = -3.125f;
			}
			else if (theta <= -3.125f)
			{
				theta = 3.125f;
			}
			

			if (phi > 2.99)
			{
				phi = 2.999f;
			}
			else if (phi <= 0.05f)
			{
				phi = 0.05f;
			}
			


			oldX = theta;
			oldY = phi;

			// Update Front, Right and Up Vectors using the updated Euler angles
			updateCameraVectors();
		}
		else
		{

			if (theta <= 0.05f)
			{
				theta += oldX * -xoffset * 0.02f *deltaTime;
				phi += oldY * yoffset * 0.02f * deltaTime;
			}
			else
			{
				theta += oldX * xoffset * 0.02f *deltaTime;
				phi += oldY * yoffset * 0.02f * deltaTime;
			}


			oldX = oldX * 0.85f ;
			oldY = oldY * 0.89f ;

			updateCameraVectors();
		}
			

			

			
		

	}

	// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
	void ProcessMouseScroll(float yoffset)
	{
		if (canMove)
		{
			if (Zoom >= 1.0f && Zoom <= 45.0f)
				Zoom -= yoffset;
			if (Zoom <= 1.0f)
				Zoom = 1.0f;
			if (Zoom >= 45.0f)
				Zoom = 45.0f;
		}

	}



	// Don't forget to replace glm::lookAt with your own version
	// view = glm::lookAt(glm::vec3(camX, 0.0f, camZ), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	//view = calculate_lookAt_matrix(glm::vec3(camX, 0.0f, camZ), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

private:
	// Calculates the front vector from the Camera's (updated) Euler Angles
	void updateCameraVectors()
	{
		// Calculate the new Front vector
		glm::vec3 front;
		front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		front.y = sin(glm::radians(Pitch));
		front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		Front = glm::normalize(front);
		// Also re-calculate the Right and Up vector
		Right = glm::normalize(glm::cross(Front, WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		Up = glm::normalize(glm::cross(Right, Front));
	}

	float M_PI = 3.14159;
};
