#include "BufferFactory.h"

BufferFactory::BufferFactory(Device & device) :
    mDevice(device), mDeviceAllocator(device, 1 << 20), mQueue(device.getTransferQueue()),
    mCommandPool(device, device.getQueueFamilyIndices().transferFamily, true, true),
    mCommandBufferSubmitter(),
    mFence(device, false)
{
    mCommandBuffer = std::move(mCommandPool.allocate(vk::CommandBufferLevel::ePrimary, 1)[0]);
}

Buffer BufferFactory::createEmptyBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, bool shouldBeDeviceLocal)
{
    Buffer buf = create(std::vector<char>(size), usage, shouldBeDeviceLocal);
    buf.resetOffset();
    return buf;
}

void BufferFactory::transfer(Buffer & src, Buffer & dst, vk::DeviceSize srcOffset, vk::DeviceSize dstOffset, vk::DeviceSize size)
{
    mCommandBuffer->begin(StructHelper::beginCommand(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
    StructHelper::transfer(src, dst, mCommandBuffer, srcOffset, dstOffset, size);
    MemoryBarrier::barrier(MemoryBarrier::buffer(dst, vk::AccessFlagBits::eTransferWrite, Access::allBits, dstOffset, size), mCommandBuffer);
    mCommandBuffer->end();

    mCommandBufferSubmitter.addCommandBuffer(mCommandBuffer);
    mCommandBufferSubmitter.submit(mQueue, mFence);
    mFence.wait();
    mFence.reset();
}
