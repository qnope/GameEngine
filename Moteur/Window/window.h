#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>

class Window
{
public:
	Window(int width, int height, std::string title);

	GLFWwindow *getWindow();

	operator GLFWwindow*();

	int getWidth() const;
	int getHeight() const;

	bool isRunning() const;

	~Window();

private:

	GLFWwindow *mWindow;
};

