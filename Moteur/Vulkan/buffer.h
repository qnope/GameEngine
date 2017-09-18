#pragma once

#include "../Tools/stream.h"
#include "deviceallocator.h"

class Buffer : public vk::UniqueBuffer
{
public:
	Buffer() = default;

	Buffer(Device &device, vk::BufferUsageFlags usage, vk::DeviceSize size,
		   DeviceAllocator &allocator, bool shouldBeDeviceLocal);

	Buffer(Buffer &&buffer) = default;
	Buffer(Buffer const &buffer) = delete;
	Buffer &operator=(Buffer &&buffer) = default;
	Buffer &operator=(Buffer const &buffer) = delete;

	vk::DeviceSize getSize() const;
	vk::BufferUsageFlags getUsage() const;
	bool isDeviceLocal() const;
	void *getPtr();
	DeviceAllocator &getAllocator();

	
	template<typename T>
	std::enable_if_t<std::is_trivially_copyable_v<T> && !is_iterable_v<T>, void> push_data(T const &value) {
		assert(mIsDeviceLocal == false);
		assert(sizeof(T) + mOffset <= mSize);
		memcpy((char*)mPtr + mOffset, &value, sizeof(T));
		mOffset += sizeof(T);
	}

	template<typename T>
	std::enable_if_t<is_iterable_v<T>, void> push_data(T const &v) {
		for (const auto &e : v)
			push_data(e);
	}

	vk::DeviceSize getOffset() const;

	void push_data(const void *data, std::size_t size);

	void resetOffset();

	~Buffer();

private:
	DeviceAllocator &mAllocator;
	vk::DeviceSize mSize;
	vk::BufferUsageFlags mUsage;
	MemoryBlock mBlock;
	bool mIsDeviceLocal;
	vk::DeviceSize mOffset = 0;
	void *mPtr = nullptr;
};
