#include "image.h"

Image::Image(Device &device, DeviceAllocator & allocator, const vk::ImageCreateInfo & info) :
    mDevice(device), mAllocator(allocator), mInfos(info)
{
    static_cast<vk::UniqueImage&>(*this) = device.get().createImageUnique(info);

    auto requirements = device->getImageMemoryRequirements(*this);
    auto properties = device.getPhysicalDevice().getMemoryProperties();

    int memoryTypeIndex = findMemoryType(requirements.memoryTypeBits, properties, info.tiling == vk::ImageTiling::eOptimal);

    mMemoryBlock = allocator.allocate(requirements.size, requirements.alignment, memoryTypeIndex);
    device->bindImageMemory(*this, mMemoryBlock.memory, mMemoryBlock.offset);

    if (info.tiling == vk::ImageTiling::eLinear)
        mPtr = mMemoryBlock.ptr;
}

vk::ImageUsageFlags Image::getUsage() const
{
    return mInfos.usage;
}

vk::Extent3D Image::getSize() const
{
    return mInfos.extent;
}

vk::Extent2D Image::getSize2D() const
{
    return vk::Extent2D{ mInfos.extent.width, mInfos.extent.height };
}

vk::Format Image::getFormat() const
{
    return mInfos.format;
}

uint32_t Image::getMipmapLevels() const
{
    return mInfos.mipLevels;
}

vk::DeviceSize Image::getRowPitch(vk::ImageAspectFlagBits aspect, uint32_t mipLevel, uint32_t arrayLayer) const
{
    vk::SubresourceLayout layout;
    vk::ImageSubresource subresource(aspect, mipLevel, arrayLayer);
    mDevice.getImageSubresourceLayout(*this, &subresource, &layout);
    return layout.rowPitch;
}

void * Image::getPtr()
{
    return mPtr;
}

Image::~Image() {
    if (get() != vk::Image())
        mAllocator.deallocate(mMemoryBlock);
}

vk::ImageCreateInfo Image::simple2DCPU(uint32_t width, uint32_t height, bool mip, vk::Format format, vk::ImageUsageFlags usages)
{
    uint32_t mipLevels = (uint32_t)(std::floor(std::log2(std::max(width, height))) + 1);
    vk::ImageCreateInfo info;

    info.arrayLayers = 1;
    info.extent = vk::Extent3D(width, height, 1);
    info.format = format;
    info.imageType = vk::ImageType::e2D;
    info.initialLayout = vk::ImageLayout::ePreinitialized;
    info.mipLevels = mip ? mipLevels : 1;
    info.samples = vk::SampleCountFlagBits::e1;
    info.sharingMode = vk::SharingMode::eExclusive;
    info.usage = usages;
    info.tiling = vk::ImageTiling::eLinear;
    return info;
}

vk::ImageCreateInfo Image::simple2DGPU(uint32_t width, uint32_t height, bool mip, vk::Format format, vk::ImageUsageFlags usages)
{
    uint32_t mipLevels = (uint32_t)(std::floor(std::log2(std::max(width, height))) + 1);

    vk::ImageCreateInfo info;
    info.arrayLayers = 1;
    info.extent = vk::Extent3D(width, height, 1);
    info.format = format;
    info.imageType = vk::ImageType::e2D;
    info.initialLayout = vk::ImageLayout::eUndefined;
    info.mipLevels = mip ? mipLevels : 1;
    info.samples = vk::SampleCountFlagBits::e1;
    info.sharingMode = vk::SharingMode::eExclusive;
    info.usage = usages;
    info.tiling = vk::ImageTiling::eOptimal;
    return info;
}

vk::ImageCreateInfo Image::simple3DGPU(uint32_t width, uint32_t height, uint32_t depth, bool mip, vk::Format format, vk::ImageUsageFlags usages)
{
    uint32_t mipLevels = (uint32_t)(std::floor(std::log2(std::max(std::max(depth, width), height))) + 1);

    vk::ImageCreateInfo info;
    info.arrayLayers = 1;
    info.extent = vk::Extent3D(width, height, depth);
    info.format = format;
    info.imageType = vk::ImageType::e3D;
    info.initialLayout = vk::ImageLayout::eUndefined;
    info.mipLevels = mip ? mipLevels : 1;
    info.samples = vk::SampleCountFlagBits::e1;
    info.sharingMode = vk::SharingMode::eExclusive;
    info.usage = usages;
    info.tiling = vk::ImageTiling::eOptimal;
    return info;
}

vk::ImageCreateInfo Image::anisotropicOpacityVoxel(uint32_t resolution, uint32_t nClipMap)
{
    resolution += 2; // little border
    vk::ImageCreateInfo info;
    info.arrayLayers = 1;
    info.extent = vk::Extent3D{ resolution * 3, resolution * nClipMap, resolution }; // 3 directions for anisotropic voxel are enough
    info.format = vk::Format::eR8Unorm;
    info.imageType = vk::ImageType::e3D;
    info.initialLayout = vk::ImageLayout::eUndefined;
    info.mipLevels = 1;
    info.samples = vk::SampleCountFlagBits::e1;
    info.sharingMode = vk::SharingMode::eExclusive;
    info.usage = vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eStorage;
    info.tiling = vk::ImageTiling::eOptimal;
    return info;
}
