#pragma once

#include <tuple>
#include "Vulkan/Image.h"
#include "Vulkan/ImageView.h"
#include "Vulkan/Sampler.h"
#include "BufferFactory.h"
#include "Vulkan/ImageLoader.h"

struct CombinedImage {
    CombinedImage(Image &img, ImageView &imageView, Sampler &sampler) : image(std::move(img)), imageView(std::move(imageView)), sampler(std::move(sampler)) {

    }

    operator vk::ImageView() const {
        return imageView;
    }

    operator vk::Image() const {
        return image;
    }

    operator vk::Sampler() const {
        return sampler;
    }

    operator Image&() {
        return image;
    }

    vk::Extent3D getSize() const {
        return image.getSize();
    }

    CombinedImage(CombinedImage &&img) :
        image(std::move(img.image)), imageView(std::move(img.imageView)), sampler(std::move(img.sampler)) {}

public:
    Image image;
    ImageView imageView;
    Sampler sampler;
};

class ImageFactory {
public:
    ImageFactory(Device &device);

    CombinedImage loadImage(const std::vector<unsigned char> &pixels, uint32_t width, uint32_t height, uint32_t rowPitch, vk::Format format, bool mipmap, bool shouldBeDeviceLocal);

    CombinedImage loadImage(std::string filename, bool mipmap, bool shouldBeDeviceLocal);

    CombinedImage createEmptyImage(vk::ImageCreateInfo imageInfo, vk::ImageViewCreateInfo imageViewInfo);

    static void buildMipmap(vk::CommandBuffer cmd, Image &image, bool stopEarly);

private:
    Device &mDevice;
    DeviceAllocator mDeviceAllocator;
    vk::Queue mQueue;
    CommandPool mCommandPool;
    CommandBufferSubmitter mCommandBufferSubmitter;
    Fence mFence;
    vk::UniqueCommandBuffer mCommandBuffer;
    BufferFactory mBufferFactory;
};
