#pragma once

#include <vector>
#include <string>
#include <utility>
#include <string_view>

enum class InOutQualifier{
    in, out
};

class ShaderGenerator
{
public:
    using type_name = std::pair<std::string, std::string>;
    using Block = std::vector<type_name>;

    ShaderGenerator(int version = 450);

    void addExtension(const std::string &extension);
    void addOutVertexShader(bool gl_Position);

    void setCurrentSet(std::size_t currentSet);

    void addSampler2D(const std::string &name);
    void resetBinding();

    void addInLocation(const type_name &typeName, int size = 1);
    void addInLocation(const Block &block, const std::string &name);

    void addOutLocation(const type_name &typeName, int size = 1);
    void addOutLocation(const Block &block, const std::string &name);

    void addSourceFromSource(const std::string &source);
    void addSourceFromFile(const std::string &fileName);

    std::string getSource() const;

private:
    void writeLayoutSetBinding(std::ostringstream &stream, const std::string &qualifier = "");
    void writeVaryingLocation(std::ostringstream &stream, InOutQualifier qualifier, int size);
    void writeBlock(std::ostringstream &stream, const Block &block, const std::string &name, bool isStruct);

private:
    std::string mSource;
    std::size_t mInLocation{0};
    std::size_t mOutLocation{0};
    std::size_t mCurrentSet{0};
    std::size_t mBinding{0};
};
