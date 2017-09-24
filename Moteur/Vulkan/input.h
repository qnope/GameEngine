#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <array>

class Input {
public:
	double mouseXPos{ 0.0 };
	double mouseYPos{ 0.0 };

	double scrollXOffset{ 0.0 };
	double scrollYOffset{ 0.0 };

	std::array<bool, GLFW_MOUSE_BUTTON_LAST> mouseButtonPressed;
	std::array<bool, GLFW_KEY_LAST> keyPressed;

public:
	static Input &instance(GLFWwindow *window = nullptr);

	Input(Input &&) = delete;
	Input(const Input&) = delete;

	Input &operator=(Input &&) = delete;
	Input &operator=(const Input &) = delete;

private:
	Input() = default;

private:
	GLFWwindow *mWindow{ nullptr };
};
