#pragma once
#include "../Vulkan/descriptorpool.h"

class DescriptorPoolWrapper {
    friend struct DescriptorPoolBuilder;
public:
    DescriptorPoolWrapper(vk::Device device, uint32_t numberSetByPool);

    std::vector<vk::DescriptorSet> allocate(vk::DescriptorSetLayout layout, uint32_t number);
    std::vector<vk::DescriptorSet> allocate(const std::vector<vk::DescriptorSetLayout> &layouts);
    vk::DescriptorSet allocate(vk::DescriptorSetLayout layout);

private:
    void addPoolSize(std::tuple<vk::DescriptorType, uint32_t> poolSize);
    void addPoolSize(vk::DescriptorPoolSize poolSize);

private:
    vk::Device mDevice;
    uint32_t mNumberSetByPool;
    uint32_t mNumberAllocatedDescriptorSets{ 0 };
    std::vector<vk::DescriptorPoolSize> mDescriptorPoolSizes;
    std::vector<DescriptorPool> mDescriptorPools;
};