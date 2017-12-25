#include "CommandPool.h"

CommandPool::CommandPool(vk::Device device, uint32_t queueFamily, bool resetable, bool transient) : mDevice(device)
{
    vk::CommandPoolCreateInfo info;
    info.flags = ((resetable) ? vk::CommandPoolCreateFlagBits::eResetCommandBuffer : vk::CommandPoolCreateFlags()) | ((transient) ? vk::CommandPoolCreateFlagBits::eTransient : vk::CommandPoolCreateFlags());
    info.queueFamilyIndex = queueFamily;

    static_cast<vk::UniqueCommandPool&>(*this) = device.createCommandPoolUnique(info);
}

std::vector<vk::UniqueCommandBuffer> CommandPool::allocate(vk::CommandBufferLevel level, uint32_t number)
{
    vk::CommandBufferAllocateInfo info;
    info.commandBufferCount = number;
    info.commandPool = *this;
    info.level = level;

    return mDevice.allocateCommandBuffersUnique(info);
}

void CommandPool::reset(bool releaseResources)
{
    vk::CommandPoolResetFlags flags = (releaseResources) ? vk::CommandPoolResetFlagBits::eReleaseResources : vk::CommandPoolResetFlags();
    mDevice.resetCommandPool(*this, flags);
}
