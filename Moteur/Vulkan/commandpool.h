#pragma once

#include "Device.h"

class CommandPool : public vk::UniqueCommandPool {
public:
    CommandPool(vk::Device device, uint32_t queueFamily, bool resetable, bool transient);

    std::vector<vk::UniqueCommandBuffer> allocate(vk::CommandBufferLevel level, uint32_t number);

    void reset(bool releaseResources);

private:
    vk::Device mDevice;
};
