#include "FrameBufferBuilder.h"

Framebuffer FramebufferBuilder::gBuffer(vk::Device device, ImageFactory & imageFactory, vk::Extent2D extent, vk::RenderPass renderPass)
{
    auto imageColor = Image::simple2DGPU(extent.width, extent.height, false, vk::Format::eR8G8B8A8Unorm, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eInputAttachment | vk::ImageUsageFlagBits::eSampled);
    auto imageNormal = Image::simple2DGPU(extent.width, extent.height, false, vk::Format::eR32G32B32A32Sfloat, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eInputAttachment | vk::ImageUsageFlagBits::eSampled);
    auto imageRoughnessMetallic = Image::simple2DGPU(extent.width, extent.height, false, vk::Format::eR8G8Unorm, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eInputAttachment | vk::ImageUsageFlagBits::eSampled);
    auto imageDepth = Image::simple2DGPU(extent.width, extent.height, false, vk::Format::eD32Sfloat, vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eInputAttachment | vk::ImageUsageFlagBits::eSampled);

    auto imageViewColor = ImageView::color2D(vk::Image(), vk::Format::eR8G8B8A8Unorm);
    auto imageViewNormal = ImageView::color2D(vk::Image(), vk::Format::eR32G32B32A32Sfloat);
    auto imageViewRoughnessMetallic = ImageView::color2D(vk::Image(), vk::Format::eR8G8Unorm);
    auto imageViewDepth = ImageView::depth2D(vk::Image(), vk::Format::eD32Sfloat);

    std::vector<CombinedImage> combinedImages;

    combinedImages.emplace_back(imageFactory.createEmptyImage(imageColor, imageViewColor)); // Diffuse map
    combinedImages.emplace_back(imageFactory.createEmptyImage(imageNormal, imageViewNormal)); // Tangeant map
    combinedImages.emplace_back(imageFactory.createEmptyImage(imageNormal, imageViewNormal)); // Normal map
    combinedImages.emplace_back(imageFactory.createEmptyImage(imageRoughnessMetallic, imageViewRoughnessMetallic));
    combinedImages.emplace_back(imageFactory.createEmptyImage(imageDepth, imageViewDepth)); // Depth map

    return Framebuffer{ device, renderPass, std::move(combinedImages) };
}

Framebuffer FramebufferBuilder::emptyFrameBuffer(vk::Device device, vk::Extent2D extent, vk::RenderPass renderPass)
{
    return Framebuffer{ device, renderPass, extent, std::vector<vk::ImageView>{} };
}

Framebuffer FramebufferBuilder::fullscreen(vk::Device device, ImageFactory & imageFactory, vk::Format format, vk::Extent2D extent, vk::RenderPass renderPass)
{
    auto image = Image::simple2DGPU(extent.width, extent.height, false, format, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled);
    auto imageView = ImageView::color2D(vk::Image(), format);

    std::vector<CombinedImage> v;
    v.emplace_back(imageFactory.createEmptyImage(image, imageView));

    return Framebuffer{ device, renderPass, std::move(v) };
}
