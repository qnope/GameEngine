#include "input.h"
#include <cassert>

static void cursor_position_callback(GLFWwindow*, double xpos, double ypos)
{
    Input::instance().mouseXPos = xpos;
    Input::instance().mouseYPos = ypos;
}

static void mouse_button_callback(GLFWwindow*, int button, int action, int mods)
{
    Input::instance().mouseButtonPressed[button] = (action == GLFW_PRESS);
}

static void scroll_callback(GLFWwindow*, double xoffset, double yoffset)
{
    Input::instance().scrollXOffset += xoffset;
    Input::instance().scrollYOffset += yoffset;
}

static void key_callback(GLFWwindow*, int key, int, int action, int)
{
    if (action == GLFW_PRESS)
        Input::instance().keyPressed[key] = true;

    else if (action == GLFW_RELEASE)
        Input::instance().keyPressed[key] = false;
}

Input & Input::instance(GLFWwindow * window)
{
    static Input input;

    if (window == nullptr)
        assert(input.mWindow != nullptr);

    else {
        input.mWindow = window;
        glfwSetInputMode(window, GLFW_STICKY_KEYS, 1);
        glfwSetCursorPosCallback(window, cursor_position_callback);
        glfwSetMouseButtonCallback(window, mouse_button_callback);
        glfwSetScrollCallback(window, scroll_callback);
        glfwSetKeyCallback(window, key_callback);
    }

    return input;
}
