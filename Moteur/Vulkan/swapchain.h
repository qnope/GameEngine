#pragma once

#include "vulkan.h"
#include "imageview.h"
#include "framebuffer.h"
#include "device.h"

class Swapchain : public vk::UniqueSwapchainKHR {
public:
    Swapchain(Device &device, Swapchain *old);

    vk::Format getFormat() const;
    vk::Extent2D getExtent() const;

    const Framebuffer &createFramebuffer(vk::RenderPass renderpass);

    uint32_t getImageCount() const;

    const std::vector<Framebuffer> &getFramebuffers() const;

private:
    Device &mDevice;
    vk::SurfaceCapabilitiesKHR mCapabilities;
    std::vector<vk::SurfaceFormatKHR> mFormats;
    std::vector<vk::PresentModeKHR> mPresentModes;
    std::vector<vk::Image> mImages;
    std::vector<ImageView> mImageViews;
    std::vector<Framebuffer> mFrameBuffers;

    vk::Format mFormat;
    vk::Extent2D mExtent;
    uint32_t mImageCount;
};