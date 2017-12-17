#include "window.h"

Window::Window(int width, int height, std::string title)
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    mWindow = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
}

GLFWwindow * Window::getWindow()
{
    return mWindow;
}

Window::operator GLFWwindow*()
{
    return mWindow;
}

int Window::getWidth() const
{
    int width, height;
    glfwGetFramebufferSize(mWindow, &width, &height);
    return width;
}

int Window::getHeight() const
{
    int width, height;
    glfwGetFramebufferSize(mWindow, &width, &height);
    return height;
}

bool Window::isRunning() const
{
    glfwPollEvents();
    return !glfwWindowShouldClose(mWindow);
}


Window::~Window()
{
    glfwDestroyWindow(mWindow);
    glfwTerminate();
}
