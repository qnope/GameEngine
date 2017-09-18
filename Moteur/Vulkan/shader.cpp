#include "shader.h"
#include <fstream>

static std::vector<char> readFile(const std::string &filename) {
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file)
		throw std::runtime_error("Failed to OpenFile : " + filename);

	std::size_t fileSize = (size_t)file.tellg();
	file.seekg(0);

	std::vector<char> buffer(fileSize);
	file.read(buffer.data(), fileSize);
	return buffer;
}

Shader::Shader(vk::Device device, std::string filename) {
	auto code = readFile(filename);

	vk::ShaderModuleCreateInfo info;
	info.codeSize = code.size();
	info.pCode = reinterpret_cast<const uint32_t*>(code.data());

	static_cast<vk::UniqueShaderModule&>(*this) = device.createShaderModuleUnique(info);
}