#include "DeviceAllocator.h"

DeviceAllocator::DeviceAllocator(Device &device, vk::DeviceSize size) :
    mChunkAllocator(device, size) {

}

MemoryBlock DeviceAllocator::allocate(vk::DeviceSize size, vk::DeviceSize alignment, int memoryTypeIndex) {
    MemoryBlock block;
    // We search a "good" chunk
    for (auto &chunk : mChunks)
        if (chunk->memoryTypeIndex() == memoryTypeIndex)
            if (chunk->allocate(size, alignment, block))
                return block;

    mChunks.emplace_back(mChunkAllocator.allocate(size, memoryTypeIndex));
    if (mChunks.back()->allocate(size, alignment, block) == false)
        throw std::runtime_error("Unable to allocate the chunk");
    return block;
}

void DeviceAllocator::deallocate(MemoryBlock &block) {
    for (auto &chunk : mChunks) {
        if (chunk->isIn(block)) {
            chunk->deallocate(block);
            return;
        }
    }
    assert(!"unable to deallocate the block");
}
