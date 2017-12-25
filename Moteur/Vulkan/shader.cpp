#include "Shader.h"
#include <fstream>
#include "Tools/Logger.h"

Shader::Shader(vk::Device device, std::string filename, EShLanguage stage) {
    ShaderCompiler compiler;

    log("Try to Compile shader : " + filename);

    try {
        auto code = compiler.compileFromFile(filename, stage, false, false);

        vk::ShaderModuleCreateInfo info;
        info.codeSize = code.size() * sizeof(decltype(code)::value_type);
        info.pCode = reinterpret_cast<const uint32_t*>(code.data());
        static_cast<vk::UniqueShaderModule&>(*this) = device.createShaderModuleUnique(info);
    }

    catch(std::runtime_error &error) {
        log(error);
        throw;
    }

    log("Success");
}
