#include "ShaderGenerator.h"
#include <sstream>
#include <string>
#include <algorithm>
#include "Tools/string_tools.h"

ShaderGenerator::ShaderGenerator(int version)
{
    std::ostringstream stream;
    mSource.reserve(1024);
    stream << "#version " << version << "\n";
    mSource += stream.str();

    addExtension("GL_ARB_separate_shader_objects");
}

void ShaderGenerator::addExtension(const std::string &extension) {
    mSource += "#extension ";
    mSource += extension;
    mSource += " : enable\n";
}

void ShaderGenerator::setCurrentSet(std::size_t currentSet) {
    mCurrentSet = currentSet;
}

void ShaderGenerator::resetBinding() {
    mBinding = 0;
}

void ShaderGenerator::writeLayoutSetBinding(std::ostringstream &stream, const std::string &qualifier) {
    stream << "layout(set = " << mCurrentSet << "," << " binding = " << mBinding;

    if(!qualifier.empty())
        stream << ", " << qualifier << ") ";

    else
        stream << ") ";
}

void ShaderGenerator::writeVaryingLocation(std::ostringstream &stream, InOutQualifier qualifier, int size) {
    stream << "layout(location = ";

    if(qualifier == InOutQualifier::in) {
        stream << mInLocation << ") in ";
        mInLocation += size;
    }

    else {
        stream << mOutLocation << ") out ";
        mOutLocation += size;
    }
}

void ShaderGenerator::writeBlock(std::ostringstream &stream, const Block &block, const std::string &name, bool isStruct) {
    std::string nameWithCase = name;
    if(isStruct) {
        stream << "struct ";
        std::transform(nameWithCase.begin(), nameWithCase.end(), nameWithCase.begin(), ::toupper);
    }

    stream << nameWithCase << " { ";

    for(const auto &line : block) {
        stream << line.first << " " << line.second << ";\n";
    }

    stream << " }";

    if(isStruct) {
        std::transform(nameWithCase.begin(), nameWithCase.end(), nameWithCase.begin(), ::tolower);
        stream << " " << nameWithCase;
    }
    stream << ";\n";
}

void ShaderGenerator::addSampler2D(const std::string &name) {
    std::ostringstream stream;
    writeLayoutSetBinding(stream);
    stream << "uniform sampler2D " << name << ";\n";
    mSource += stream.str();
}


void ShaderGenerator::addInLocation(const type_name &typeName, int size) {
    std::ostringstream stream;

    writeVaryingLocation(stream, InOutQualifier::in, size);
    stream << typeName.first << " " << typeName.second << ";\n";

    mSource += stream.str();
}

void ShaderGenerator::addOutLocation(const type_name &typeName, int size) {
    std::ostringstream stream;

    writeVaryingLocation(stream, InOutQualifier::out, size);
    stream << typeName.first << " " << typeName.second << ";\n";

    mSource += stream.str();
}

void ShaderGenerator::addInLocation(const Block &block, const std::string &name) {
    std::ostringstream stream;
    writeVaryingLocation(stream, InOutQualifier::in, 1);
    writeBlock(stream, block, name, true);
    mSource += stream.str();
}

void ShaderGenerator::addOutLocation(const Block &block, const std::string &name) {
    std::ostringstream stream;
    writeVaryingLocation(stream, InOutQualifier::out, 1);
    writeBlock(stream, block, name, true);
    mSource += stream.str();
}

void ShaderGenerator::addSourceFromSource(const std::string &source) {
    mSource += source;
}

void ShaderGenerator::addSourceFromFile(const std::string &fileName) {
    addSourceFromSource(readFile(fileName));
}

std::string ShaderGenerator::getSource() const {
    return mSource;
}
