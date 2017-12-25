#include "DescriptorPool.h"
#include "Tools/stream.h"

DescriptorPool::DescriptorPool(vk::Device device) : mDevice(device)
{
}

void DescriptorPool::addPoolSize(vk::DescriptorPoolSize size)
{
    mPoolSizes << size;
}

void DescriptorPool::create(uint32_t maxSet)
{
    vk::DescriptorPoolCreateInfo info;
    info.maxSets = maxSet;
    info.poolSizeCount = (uint32_t)mPoolSizes.size();
    info.pPoolSizes = mPoolSizes.data();
    static_cast<vk::UniqueDescriptorPool&>(*this) = mDevice.createDescriptorPoolUnique(info);
    mMaximumNumberSets = maxSet;
}

vk::DescriptorSet DescriptorPool::allocate(vk::DescriptorSetLayout layout)
{
    return allocate(std::vector<vk::DescriptorSetLayout>({ { layout } }))[0];
}

std::vector<vk::DescriptorSet> DescriptorPool::allocate(vk::DescriptorSetLayout layout, uint32_t number)
{
    return allocate(std::vector<vk::DescriptorSetLayout>(number, layout));
}

std::vector<vk::DescriptorSet> DescriptorPool::allocate(const std::vector<vk::DescriptorSetLayout>& layouts)
{
    vk::DescriptorSetAllocateInfo info;

    info.descriptorPool = *this;
    info.descriptorSetCount = (uint32_t)layouts.size();
    info.pSetLayouts = layouts.data();

    mNumberAllocatedDescriptorSets += (uint32_t)layouts.size();

    assert(mNumberAllocatedDescriptorSets <= mMaximumNumberSets);

    return mDevice.allocateDescriptorSets(info);
}
