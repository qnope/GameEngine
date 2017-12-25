#pragma once

#include "Device.h"

class Fence : public vk::UniqueFence {
public:
    Fence() = default;
    Fence(vk::Device device, bool signaled = false);

    void reset();
    void wait();
private:
    vk::Device mDevice;
};
