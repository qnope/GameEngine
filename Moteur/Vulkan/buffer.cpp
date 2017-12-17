#include "buffer.h"

Buffer::Buffer(Device &device, vk::BufferUsageFlags usage, vk::DeviceSize size,
    DeviceAllocator &allocator, bool shouldBeDeviceLocal) :
    mAllocator(allocator),
    mSize(size),
    mUsage(usage),
    mIsDeviceLocal(shouldBeDeviceLocal) {
    vk::BufferCreateInfo createInfo(vk::BufferCreateFlags(),
        mSize,
        mUsage,
        vk::SharingMode::eExclusive);

    static_cast<vk::UniqueBuffer&>(*this) = device.get().createBufferUnique(createInfo);
    auto requirements = device.get().getBufferMemoryRequirements(*this);
    auto properties = device.getPhysicalDevice().getMemoryProperties();

    int memoryTypeIndex = findMemoryType(requirements.memoryTypeBits, properties, shouldBeDeviceLocal);

    mBlock = mAllocator.allocate(requirements.size, requirements.alignment, memoryTypeIndex);
    device->bindBufferMemory(*this, mBlock.memory, mBlock.offset);

    // if host_visible, we can map it
    if (!shouldBeDeviceLocal)
        mPtr = mBlock.ptr;
}

vk::DeviceSize Buffer::getSize() const {
    return mSize;
}

vk::BufferUsageFlags Buffer::getUsage() const {
    return mUsage;
}

bool Buffer::isDeviceLocal() const {
    return mIsDeviceLocal;
}

void *Buffer::getPtr() {
    return mPtr;
}

DeviceAllocator &Buffer::getAllocator() {
    return mAllocator;
}

vk::DeviceSize Buffer::getOffset() const
{
    return mOffset;
}

void Buffer::push_data(const void * data, std::size_t size)
{
    assert(mIsDeviceLocal == false);
    memcpy((char*)mPtr + mOffset, data, size);
    mOffset += size;
}

void Buffer::resetOffset()
{
    mOffset = 0;
}

Buffer::~Buffer() {
    if (get() != vk::Buffer())
        mAllocator.deallocate(mBlock);
}