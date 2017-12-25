#include "Framebuffer.h"

Framebuffer::Framebuffer(vk::Device device, vk::RenderPass renderpass, std::vector<CombinedImage> &&images) :
    mCombinedImages(std::move(images))
{
    std::vector<vk::ImageView> views{ mCombinedImages.begin(), mCombinedImages.end() };

    vk::FramebufferCreateInfo info;

    mExtent = vk::Extent2D{ mCombinedImages[0].getSize().width, mCombinedImages[0].getSize().height };

    info.attachmentCount = (uint32_t)views.size();
    info.renderPass = renderpass;
    info.pAttachments = views.data();
    info.width = mExtent.width;
    info.height = mExtent.height;
    info.layers = 1;

    static_cast<vk::UniqueFramebuffer&>(*this) = device.createFramebufferUnique(info);
}

Framebuffer::Framebuffer(vk::Device device, vk::RenderPass renderpass, vk::Extent2D extent, const std::vector<vk::ImageView>& imageViews) :
    mExtent(extent)
{
    vk::FramebufferCreateInfo info;

    info.attachmentCount = (uint32_t)imageViews.size();
    info.renderPass = renderpass;
    info.pAttachments = imageViews.data();
    info.width = extent.width;
    info.height = extent.height;
    info.layers = 1;

    static_cast<vk::UniqueFramebuffer&>(*this) = device.createFramebufferUnique(info);
}

Framebuffer::Framebuffer(vk::Device device, vk::RenderPass renderpass, vk::Extent2D extent, vk::ImageView imageView) :
    mExtent(extent)
{
    vk::FramebufferCreateInfo info;

    info.attachmentCount = 1;
    info.renderPass = renderpass;
    info.pAttachments = &imageView;
    info.width = extent.width;
    info.height = extent.height;
    info.layers = 1;

    static_cast<vk::UniqueFramebuffer&>(*this) = device.createFramebufferUnique(info);
}

const CombinedImage & Framebuffer::getCombinedImage(uint32_t index) const
{
    return mCombinedImages[index];
}

vk::Extent2D Framebuffer::getExtent() const
{
    return mExtent;
}
