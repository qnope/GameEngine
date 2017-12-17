#include <utility>
#include "chunkallocator.h"

vk::DeviceSize nextPowerOfTwo(vk::DeviceSize size) {
    vk::DeviceSize power = (vk::DeviceSize)(std::log2((long double)size) + 1);
    return (vk::DeviceSize)1 << power;
}

bool isPowerOfTwo(vk::DeviceSize size) {
    vk::DeviceSize mask = 0;
    vk::DeviceSize power = (vk::DeviceSize)std::log2((long double)size);

    for (vk::DeviceSize i = 0; i < power; ++i)
        mask += (vk::DeviceSize)1 << i;

    return !(size & mask);
}

ChunkAllocator::ChunkAllocator(Device &device, vk::DeviceSize size) :
    mDevice(device),
    mSize(size) {
    assert(isPowerOfTwo(size));
}

std::unique_ptr<MemoryChunk> ChunkAllocator::allocate(vk::DeviceSize size, int memoryTypeIndex) {
    size = (size > mSize) ? nextPowerOfTwo(size) : mSize;

    return std::make_unique<MemoryChunk>(mDevice, size, memoryTypeIndex);
}