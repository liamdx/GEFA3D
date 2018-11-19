#pragma once
#include "Common.h"

class InputManager {

public:

	InputManager(GLFWwindow* newWindow) : window(newWindow) {};

	~InputManager() {} ;

	void initialize()
	{
		glfwSetCursorPosCallback(window, mouse_callback);
	}

	bool GetRightClick ()
	{ 
		if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
		{
			return true;
		}
		else
		{
			return false;
		}
	 
	};

	bool GetLeftClick ()
	{
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		{
			return true;
		}
		else
		{
			return false;
		}
	};
	bool GetKeyW () 
	{
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		{
			return true;
		}
		else
		{
			return false;
		}
	};
	bool GetKeyS () 
	{
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		{
			return true;
		}
		else
		{
			return false;
		}
	};
	bool GetKeyA () 
	{
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		{
			return true;
		}
		else
		{
			return false;
		}
	};
	bool GetKeyD () 
	{
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		{
			return true;
		}
		else
		{
			return false;
		}
	};
	bool GetKeyEsc  () 
	{
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		{
			return true;
		}
		else
		{
			return false;
		}
	};


	GLFWwindow* window;

	double xpos, ypos;
	

	static void mouse_callback(GLFWwindow* window, double xpos, double ypos)
	{

	}

	void GetMouseMovement()
	{
		glfwGetCursorPos(window, &xpos, &ypos);
	}

	void GetMouseScroll()
	{
		//glfwGetScro
	}
};