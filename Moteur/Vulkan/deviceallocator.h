#pragma once

#include "ChunkAllocator.h"

class DeviceAllocator
{
public:
    DeviceAllocator(Device &device, vk::DeviceSize size);

    MemoryBlock allocate(vk::DeviceSize size, vk::DeviceSize alignment, int memoryTypeIndex);
    void deallocate(MemoryBlock &block);

private:
    ChunkAllocator mChunkAllocator;
    std::vector<std::unique_ptr<MemoryChunk>> mChunks;
};
