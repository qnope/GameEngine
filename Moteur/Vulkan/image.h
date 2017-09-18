#pragma once

#include "deviceallocator.h"

class Image : public vk::UniqueImage {
public:
	Image() = default;
	Image(Device &device, DeviceAllocator &allocator, const vk::ImageCreateInfo &info);

	Image(Image &&image) = default;
	Image(Image const &image) = delete;
	Image &operator=(Image &&image) = default;
	Image &operator=(Image const &image) = delete;

	vk::ImageUsageFlags getUsage() const;
	vk::Extent3D getSize() const;
	vk::Extent2D getSize2D() const;
	vk::Format getFormat() const;

	uint32_t getMipmapLevels() const;

	vk::DeviceSize getRowPitch(vk::ImageAspectFlagBits aspect, uint32_t mipLevel, uint32_t arrayLayer) const;

	void *getPtr();

	static vk::ImageCreateInfo simple2DCPU(uint32_t width, uint32_t height, bool mip, vk::Format format, vk::ImageUsageFlags usages);
	static vk::ImageCreateInfo simple2DGPU(uint32_t width, uint32_t height, bool mip, vk::Format format, vk::ImageUsageFlags usages);
	static vk::ImageCreateInfo simple3DGPU(uint32_t width, uint32_t height, uint32_t depth, bool mip, vk::Format format, vk::ImageUsageFlags usages);

	static vk::ImageCreateInfo anisotropicOpacityVoxel(uint32_t resolution, uint32_t nClipMap);

	~Image();

private:
	vk::Device mDevice;
	DeviceAllocator &mAllocator;
	MemoryBlock mMemoryBlock;
	vk::ImageCreateInfo mInfos;
	void *mPtr;
};
