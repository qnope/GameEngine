#pragma once

#include "Vulkan/DeviceAllocator.h"
#include "Vulkan/StructHelper.h"
#include "Vulkan/CommandBufferSubmitter.h"
#include "Vulkan/Fence.h"
#include "Vulkan/CommandPool.h"

class BufferFactory {
public:
    BufferFactory(Device &device);

    template<typename T>
    std::enable_if_t<is_iterable_v<T>, Buffer> create(const T &v, vk::BufferUsageFlags usage, bool shouldBeDeviceLocal) {
        std::size_t size = sizeof(typename T::value_type) * v.size();
        if (shouldBeDeviceLocal) {
            Buffer bufferCPU(mDevice, vk::BufferUsageFlagBits::eTransferSrc, size, mDeviceAllocator, false);
            Buffer bufferGPU(mDevice, usage | vk::BufferUsageFlagBits::eTransferDst, size, mDeviceAllocator, true);

            bufferCPU.push_data(v);

            mCommandBuffer->begin(StructHelper::beginCommand(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
            StructHelper::transfer(bufferCPU, bufferGPU, mCommandBuffer);
            MemoryBarrier::barrier(MemoryBarrier::global(), mCommandBuffer);

            mCommandBuffer->end();

            mCommandBufferSubmitter.addCommandBuffer(mCommandBuffer);
            mCommandBufferSubmitter.submit(mQueue, mFence);
            mFence.wait();
            mFence.reset();

            return bufferGPU;
        }

        Buffer buffer(mDevice, usage, size, mDeviceAllocator, false);
        buffer.push_data(v);
        return buffer;
    }

    template<typename T>
    std::enable_if_t<std::is_trivially_copyable_v<T> && !is_iterable_v<T>, Buffer> create(const T &v, vk::BufferUsageFlags usage, bool shouldBeDeviceLocal) {
        auto a = { v };
        return create(a, usage, shouldBeDeviceLocal);
    }

    Buffer createEmptyBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, bool shouldBeDeviceLocal);

    void transfer(Buffer &src, Buffer &dst, vk::DeviceSize srcOffset = 0, vk::DeviceSize dstOffset = 0, vk::DeviceSize size = VK_WHOLE_SIZE);

private:
    Device &mDevice;
    DeviceAllocator mDeviceAllocator;
    vk::Queue mQueue;
    CommandPool mCommandPool;
    CommandBufferSubmitter mCommandBufferSubmitter;
    Fence mFence;
    vk::UniqueCommandBuffer mCommandBuffer;
};
