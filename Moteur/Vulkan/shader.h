#pragma once

#include "device.h"

class Shader : public vk::UniqueShaderModule {
public:
	Shader() = default;
	Shader(vk::Device device, std::string filename);
private:
};
