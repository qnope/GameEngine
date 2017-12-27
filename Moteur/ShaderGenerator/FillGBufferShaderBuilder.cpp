#include "FillGBufferShaderBuilder.h"
#include "ShaderGenerator.h"

std::string FillGBufferShaderBuilder::buildFragmentShader(const std::string &functionImplementation) const {
    ShaderGenerator generator;

    generator.addSourceFromSource("layout(early_fragment_tests) in;\n");
    generator.addSourceFromFile("../Shaders/FillGBuffer/FillGBuffer.frag");
    generator.addSourceFromSource(functionImplementation);

    return generator.getSource();
}
