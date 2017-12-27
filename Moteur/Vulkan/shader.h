#pragma once

#include "Device.h"
#include "ShaderCompiler.h"

struct source_tag{};
struct file_tag{};

class Shader : public vk::UniqueShaderModule {
public:
    Shader(vk::Device device, std::string fileName, EShLanguage stage, file_tag);
    Shader(vk::Device device, std::string source, EShLanguage stage, source_tag);
private:
};
