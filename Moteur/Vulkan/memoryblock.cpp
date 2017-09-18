#include "memoryblock.h"

int findMemoryType(uint32_t memoryTypeBits,
	vk::PhysicalDeviceMemoryProperties const &properties,
	bool shouldBeDeviceLocal) {

	auto lambdaGetMemoryType = [&](vk::MemoryPropertyFlags propertyFlags) -> int {
		for (uint32_t i = 0; i < properties.memoryTypeCount; ++i)
			if ((memoryTypeBits & (1 << i)) &&
				((properties.memoryTypes[i].propertyFlags & propertyFlags) == propertyFlags))
				return i;
		return -1;
	};

	if (!shouldBeDeviceLocal) {
		vk::MemoryPropertyFlags optimal = vk::MemoryPropertyFlagBits::eHostCached |
			vk::MemoryPropertyFlagBits::eHostCoherent |
			vk::MemoryPropertyFlagBits::eHostVisible;

		vk::MemoryPropertyFlags required = vk::MemoryPropertyFlagBits::eHostCoherent |
			vk::MemoryPropertyFlagBits::eHostVisible;

		int type = lambdaGetMemoryType(optimal);
		if (type == -1) {
			int result = lambdaGetMemoryType(required);
			if (result == -1)
				assert(!"Memory type does not find");
			return result;
		}
		return type;
	}

	else
		return lambdaGetMemoryType(vk::MemoryPropertyFlagBits::eDeviceLocal);
}

bool MemoryBlock::operator ==(MemoryBlock const &block) {
	if (memory == block.memory &&
		offset == block.offset &&
		size == block.size &&
		free == block.free &&
		ptr == block.ptr)
		return true;
	return false;
}

std::ostream &operator<<(std::ostream &os, const MemoryBlock &block) {
	os << "Memory : " << block.memory << std::endl;
	os << "Offset : " << block.offset << std::endl;
	os << "Size : " << block.size << std::endl;
	os << "free : " << block.free << std::endl;
	return os;
}