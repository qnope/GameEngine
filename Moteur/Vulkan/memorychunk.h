#pragma once

#include "MemoryBlock.h"

class MemoryChunk {
public:
    MemoryChunk(Device &device, vk::DeviceSize size, int memoryTypeIndex);

    bool allocate(vk::DeviceSize size, vk::DeviceSize alignment, MemoryBlock &block);
    bool isIn(MemoryBlock const &block) const;
    void deallocate(MemoryBlock const &block);
    int memoryTypeIndex() const;

    ~MemoryChunk();

protected:
    Device &mDevice;
    vk::DeviceMemory mMemory = vk::DeviceMemory();
    vk::DeviceSize mSize;
    int mMemoryTypeIndex;
    std::vector<MemoryBlock> mBlocks;
    void *mPtr = nullptr;
};
