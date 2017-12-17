#include "shader.h"
#include <fstream>

Shader::Shader(vk::Device device, std::string filename, EShLanguage stage) {
    ShaderCompiler compiler;
    auto code = compiler.compileFromFile(filename, stage, false, false);

    vk::ShaderModuleCreateInfo info;
    info.codeSize = code.size() * sizeof(decltype(code)::value_type);
    info.pCode = reinterpret_cast<const uint32_t*>(code.data());

    static_cast<vk::UniqueShaderModule&>(*this) = device.createShaderModuleUnique(info);
}