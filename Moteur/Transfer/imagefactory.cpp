#include "Vulkan/Buffer.h"
#include "ImageFactory.h"

#include <algorithm>

ImageFactory::ImageFactory(Device & device) :
    mDevice(device), mDeviceAllocator(mDevice, 1 < 26), mQueue(device.getTransferQueue()),
    mCommandPool(device, device.getQueueFamilyIndices().transferFamily, true, true),
    mCommandBufferSubmitter(),
    mFence(device, false),
    mBufferFactory(device)
{
    mCommandBuffer = std::move(mCommandPool.allocate(vk::CommandBufferLevel::ePrimary, 1)[0]);
}

CombinedImage ImageFactory::loadImage(const std::vector<unsigned char>& pixels, uint32_t width, uint32_t height, uint32_t rowPitch, vk::Format format, bool mipmap, bool shouldBeDeviceLocal)
{
    Sampler sampler(mDevice, Sampler::simple());

    vk::ImageUsageFlags usages = vk::ImageUsageFlagBits::eSampled;
    if (mipmap)
        usages |= vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eTransferSrc;

    if (shouldBeDeviceLocal) {
        usages |= vk::ImageUsageFlagBits::eTransferDst;
        Buffer buffer(mDevice, vk::BufferUsageFlagBits::eTransferSrc, rowPitch * height, mDeviceAllocator, false);

        buffer.push_data(pixels);

        Image img(mDevice, mDeviceAllocator, Image::simple2DGPU(width, height, mipmap, format, usages));

        mCommandBuffer->begin(StructHelper::beginCommand(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
        MemoryBarrier::transition(img, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal, mCommandBuffer);
        StructHelper::transfer(buffer, img, mCommandBuffer);

        if (mipmap) {
            MemoryBarrier::transition(img, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eTransferSrcOptimal, mCommandBuffer);
            buildMipmap(mCommandBuffer, img, false);
            MemoryBarrier::transition(img, vk::ImageLayout::eTransferSrcOptimal, vk::ImageLayout::eShaderReadOnlyOptimal, mCommandBuffer);
        }

        else
            MemoryBarrier::transition(img, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal, mCommandBuffer);

        mCommandBuffer->end();

        mCommandBufferSubmitter.addCommandBuffer(mCommandBuffer);
        mCommandBufferSubmitter.submit(mQueue, mFence);
        mFence.wait();
        mFence.reset();

        ImageView imgView(mDevice, ImageView::color2D(img, format));
        return CombinedImage(img, imgView, sampler);
    }

    Image img(mDevice, mDeviceAllocator, Image::simple2DCPU(width, height, mipmap, format, usages));

    for (uint32_t i = 0; i < height; ++i)
        memcpy((char*)img.getPtr() + i * img.getRowPitch(vk::ImageAspectFlagBits::eColor, 0, 0),
        (char*)pixels.data() + i* rowPitch,
            width * 4);

    ImageView imgView(mDevice, ImageView::color2D(img, format));

    mCommandBuffer.get().begin(StructHelper::beginCommand(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));


    if (mipmap) {
        MemoryBarrier::transition(img, vk::ImageLayout::ePreinitialized, vk::ImageLayout::eTransferSrcOptimal, mCommandBuffer);
        buildMipmap(mCommandBuffer, img, false);
        MemoryBarrier::transition(img, vk::ImageLayout::eTransferSrcOptimal, vk::ImageLayout::eShaderReadOnlyOptimal, mCommandBuffer);
    }

    else
        MemoryBarrier::transition(img, vk::ImageLayout::ePreinitialized, vk::ImageLayout::eShaderReadOnlyOptimal, mCommandBuffer);

    mCommandBuffer.get().end();

    mCommandBufferSubmitter.addCommandBuffer(mCommandBuffer);
    mCommandBufferSubmitter.submit(mQueue, mFence);
    mFence.wait();
    mFence.reset();

    return CombinedImage(img, imgView, sampler);
}

CombinedImage ImageFactory::loadImage(std::string filename, bool mipmap, bool shouldBeDeviceLocal)
{
    ImageLoader imgLoader(filename);
    auto size = imgLoader.getHeight() * imgLoader.getRowPitch();
    std::vector<unsigned char> pixels(size);
    std::copy(imgLoader.getPixel(), imgLoader.getPixel() + size, pixels.begin());
    return loadImage(pixels, imgLoader.getWidth(), imgLoader.getHeight(), imgLoader.getRowPitch(), imgLoader.getFormat(), mipmap, shouldBeDeviceLocal);
}

CombinedImage ImageFactory::createEmptyImage(vk::ImageCreateInfo imageInfo, vk::ImageViewCreateInfo imageViewInfo)
{
    Sampler sampler(mDevice, Sampler::simple());
    Image image(mDevice, mDeviceAllocator, imageInfo);
    imageViewInfo.image = image;
    ImageView imageView(mDevice, imageViewInfo);

    return CombinedImage(image, imageView, sampler);
}

void ImageFactory::buildMipmap(vk::CommandBuffer cmd, Image & image, bool stopEarly)
{
    assert((image.getUsage() & vk::ImageUsageFlagBits::eTransferDst) == vk::ImageUsageFlagBits::eTransferDst);
    assert((image.getUsage() & vk::ImageUsageFlagBits::eTransferSrc) == vk::ImageUsageFlagBits::eTransferSrc);

    for (uint32_t i = 1; i < image.getMipmapLevels(); ++i) {
        vk::ImageBlit blit;
        blit.srcSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
        blit.srcSubresource.baseArrayLayer = 0;
        blit.srcSubresource.layerCount = 1;
        blit.srcSubresource.mipLevel = i - 1;
        blit.dstSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
        blit.dstSubresource.baseArrayLayer = 0;
        blit.dstSubresource.layerCount = 1;
        blit.dstSubresource.mipLevel = i;

        // each mipmap is the size divided by two
        blit.srcOffsets[1] = vk::Offset3D(std::max(1u, image.getSize().width >> (i - 1)),
            std::max(1u, image.getSize().height >> (i - 1)),
            std::max(1u, image.getSize().depth >> (i - 1)));

        blit.dstOffsets[1] = vk::Offset3D(std::max(1u, image.getSize().width >> i),
            std::max(1u, image.getSize().height >> i),
            std::max(1u, image.getSize().depth >> i));

        auto range = ImageSubresource::rangeColor(i, 1);

        MemoryBarrier::transition(image, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal,
            cmd, range, vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eTransfer);

        cmd.blitImage(image, vk::ImageLayout::eTransferSrcOptimal, image, vk::ImageLayout::eTransferDstOptimal, blit, vk::Filter::eLinear);

        MemoryBarrier::transition(image, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eTransferSrcOptimal, cmd,
            range, vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTransfer);

        if (stopEarly)
            if (blit.srcOffsets[1].x / 2 == 0 || blit.srcOffsets[1].y / 2 == 0 || blit.srcOffsets[1].z / 2 == 0)
                break;
    }
}
