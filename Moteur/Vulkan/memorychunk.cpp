#include "memorychunk.h"

MemoryChunk::MemoryChunk(Device &device, vk::DeviceSize size, int memoryTypeIndex) :
    mDevice(device),
    mSize(size),
    mMemoryTypeIndex(memoryTypeIndex) {
    vk::MemoryAllocateInfo allocateInfo(size, memoryTypeIndex);

    MemoryBlock block;
    block.free = true;
    block.offset = 0;
    block.size = size;
    mMemory = block.memory = device.get().allocateMemory(allocateInfo);

    if ((device.getPhysicalDevice().getMemoryProperties().memoryTypes[memoryTypeIndex].propertyFlags & vk::MemoryPropertyFlagBits::eHostVisible) == vk::MemoryPropertyFlagBits::eHostVisible)
        mPtr = device.get().mapMemory(mMemory, 0, VK_WHOLE_SIZE);

    mBlocks.emplace_back(block);
}

int MemoryChunk::memoryTypeIndex() const {
    return mMemoryTypeIndex;
}

bool MemoryChunk::isIn(const MemoryBlock &block) const {
    return block.memory == mMemory;
}

void MemoryChunk::deallocate(const MemoryBlock &block) {
    auto blockIt(std::find(mBlocks.begin(), mBlocks.end(), block));
    assert(blockIt != mBlocks.end());
    // Just put the block to free
    blockIt->free = true;

    // Gather contigu free blocks
    bool end = false;
    while (!end) {
        end = true;
        for (int i = 0; i < mBlocks.size() - 1; ++i) {
            if (mBlocks[i].free && mBlocks[i + 1].free) {
                mBlocks[i].size = mBlocks[i + 1].offset + mBlocks[i + 1].size - mBlocks[i].offset;
                mBlocks.erase(mBlocks.begin() + i + 1);
                end = false;
                break;
            }
        }
    }
}

bool MemoryChunk::allocate(vk::DeviceSize size, vk::DeviceSize alignment, MemoryBlock &block) {
    // if chunk is too small
    if (size > mSize)
        return false;

    for (uint32_t i = 0; i < mBlocks.size(); ++i) {
        if (mBlocks[i].free && mBlocks[i].size >= size) {
            // Compute one offset to add to the current offset for alignment care
            auto offsetDisplacement = (mBlocks[i].offset % alignment > 0) ? alignment - mBlocks[i].offset % alignment : 0;
            auto oldSizeAvailable = mBlocks[i].size - offsetDisplacement;

            // if match
            if (size + offsetDisplacement <= mBlocks[i].size) {
                mBlocks[i].offset += offsetDisplacement;
                mBlocks[i].size = size;
                mBlocks[i].free = false;

                // Compute the ptr address
                if (mPtr != nullptr)
                    mBlocks[i].ptr = static_cast<char*>(mPtr) + mBlocks[i].offset;

                block = mBlocks[i];

                MemoryBlock nextBlock;
                nextBlock.free = true;
                nextBlock.offset = mBlocks[i].offset + size;
                nextBlock.memory = mMemory;
                nextBlock.size = oldSizeAvailable - size;

                if (nextBlock.size > 0)
                    mBlocks.emplace(mBlocks.begin() + i + 1, nextBlock);

                return true;
            }
        }
    }

    return false;
}

MemoryChunk::~MemoryChunk() {
    mDevice.get().freeMemory(mMemory);
}