#include "ShaderCompiler.h"
#include "ResourceLimits.h"
#include <fstream>
#include <SPIRV/GlslangToSpv.h>
#include "Tools/string_tools.h"

ShaderCompiler::ShaderCompiler() {
    glslang::InitializeProcess();
}

std::vector<unsigned> ShaderCompiler::compileFromSource(std::string source, EShLanguage stage, bool debug, bool invertY, const char *fileName) {
    EShMessages messages = (EShMessages)(EShMsgSpvRules | EShMsgVulkanRules | EShMsgDefault);

    glslang::TProgram program;
    glslang::TShader shader(stage);

    if (debug)
        messages = (EShMessages)(messages | EShMsgDebugInfo);
    const char *cSource = source.c_str();
    shader.setStringsWithLengthsAndNames(&cSource, nullptr, &fileName, 1);

    if (invertY)
        shader.setInvertY(true);

    shader.setEnvInput(glslang::EShSourceGlsl, stage, glslang::EShClientVulkan, 100);
    shader.setEnvClient(glslang::EShClientVulkan, 100);
    shader.setEnvTarget(glslang::EshTargetSpv, 0x1000);

    const int defaultVersion = 450;
    const TBuiltInResource Resources = glslang::DefaultTBuiltInResource;

    auto throwError = [](std::string fileName, auto &sourceOfBug) {
        throw std::runtime_error(fileName + sourceOfBug.getInfoLog() + sourceOfBug.getInfoDebugLog());
    };

    if (!shader.parse(&Resources, defaultVersion, false, messages))
        throwError(fileName ? fileName : "", shader);

    program.addShader(&shader);

    // Link
    if (!program.link(messages))
        throwError(fileName ? fileName : "", program);

    std::vector<unsigned int> spirv;
    spv::SpvBuildLogger logger;
    glslang::SpvOptions spvOptions;

    if (debug) {
        spvOptions.disableOptimizer = true;
        spvOptions.optimizeSize = false;
        spvOptions.generateDebugInfo = true;
    }

    else {
        spvOptions.generateDebugInfo = false;
        spvOptions.optimizeSize = true;
        spvOptions.disableOptimizer = false;
    }

    glslang::GlslangToSpv(*program.getIntermediate((EShLanguage)stage), spirv, &logger, &spvOptions);
    return spirv;
}

std::vector<unsigned> ShaderCompiler::compileFromFile(std::string pathFile, EShLanguage stage, bool debug, bool invertY) {
    return compileFromSource(readFile(pathFile), stage, debug, invertY, pathFile.c_str());
}

ShaderCompiler::~ShaderCompiler() {
    glslang::FinalizeProcess();
}
