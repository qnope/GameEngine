#pragma once

#include "device.h"

class DescriptorPool : public vk::UniqueDescriptorPool {
public:
	DescriptorPool(vk::Device device);

	void addPoolSize(vk::DescriptorPoolSize size);

	void create(uint32_t maxSet);

	vk::DescriptorSet allocate(vk::DescriptorSetLayout layout);
	std::vector<vk::DescriptorSet> allocate(vk::DescriptorSetLayout layout, uint32_t number);
	std::vector<vk::DescriptorSet> allocate(const std::vector<vk::DescriptorSetLayout> &layouts);

private:
	vk::Device mDevice;
	std::vector<vk::DescriptorPoolSize> mPoolSizes;
	uint32_t mMaximumNumberSets{ 0 };
	uint32_t mNumberAllocatedDescriptorSets{ 0 };
};
