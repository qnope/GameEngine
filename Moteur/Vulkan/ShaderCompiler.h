#pragma once
#include <glslang/Public/ShaderLang.h>

class ShaderCompiler {
public:
    ShaderCompiler();
    std::vector<unsigned> compileFromSource(std::string source, EShLanguage stage, bool debug, bool invertY, const char *fileName = nullptr);
    std::vector<unsigned> compileFromFile(std::string pathFile, EShLanguage stage, bool debug, bool invertY);
    ~ShaderCompiler();

private:
};