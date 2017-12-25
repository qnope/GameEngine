#pragma once

#include "Device.h"

int findMemoryType(uint32_t memoryTypeBits,
    vk::PhysicalDeviceMemoryProperties const &properties,
    bool shouldBeDeviceLocal);


struct MemoryBlock {
    vk::DeviceMemory memory;
    vk::DeviceSize offset{ 0 };
    vk::DeviceSize size{ 0 };
    bool free = false;
    void *ptr = nullptr; // Useless if it is a GPU allocation

    bool operator ==(MemoryBlock const &block);
};

std::ostream &operator<<(std::ostream &os, const MemoryBlock &block);

