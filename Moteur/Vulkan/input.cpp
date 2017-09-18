#include "input.h"
#include <cassert>

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	Input::instance().mouseXPos = xpos;
	Input::instance().mouseYPos = ypos;
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	Input::instance().mouseButtonPressed[button] = (action == GLFW_PRESS);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	Input::instance().scrollXOffset += xoffset;
	Input::instance().scrollYOffset += yoffset;
}

Input & Input::instance(GLFWwindow * window)
{
	static Input input;

	if (window == nullptr)
		assert(input.mWindow != nullptr);

	else {
		input.mWindow = window;
		glfwSetCursorPosCallback(window, cursor_position_callback);
		glfwSetMouseButtonCallback(window, mouse_button_callback);
	}

	return input;
}
