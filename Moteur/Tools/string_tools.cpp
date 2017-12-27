#include <fstream>
#include "string_tools.h"

std::string readFile(std::string fileName)
{
    std::ifstream stream(fileName);
    std::string source;
    std::string line;

    if (!stream)
        throw std::runtime_error("Unable to open input file : " + fileName);

    while (std::getline(stream, line))
        source += line + "\n";

    return source;
}
