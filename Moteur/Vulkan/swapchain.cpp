#include "Swapchain.h"
#include "Tools/stream.h"
#include "StructHelper.h"

vk::SurfaceFormatKHR getFormat(const std::vector<vk::SurfaceFormatKHR> &formats) {
    vk::SurfaceFormatKHR toReturn;

    if (formats.size() == 1 && formats[0].format == vk::Format::eUndefined) {
        toReturn.format = vk::Format::eB8G8R8A8Unorm;
        toReturn.colorSpace = vk::ColorSpaceKHR::eSrgbNonlinear;
        return toReturn;
    }

    for (const auto &format : formats)
        if (format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
            if (format.format == vk::Format::eB8G8R8A8Unorm)
                return format;
    throw std::runtime_error("Unable to find format");
}

vk::PresentModeKHR getPresentMode(const std::vector<vk::PresentModeKHR> &presentModes) {
    vk::PresentModeKHR best = vk::PresentModeKHR::eFifo;

    for (const auto &mode : presentModes) {
        if (mode == vk::PresentModeKHR::eMailbox)
            best = mode;

        else if (mode == vk::PresentModeKHR::eImmediate)
            best = mode;
    }

    return best;
}

vk::Extent2D getSwapExtent(const vk::SurfaceCapabilitiesKHR &capabilities, const Window &window) {
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
        return capabilities.currentExtent;

    vk::Extent2D extent;

    extent.height = std::max<uint32_t>(capabilities.minImageExtent.height, std::min<uint32_t>(capabilities.maxImageExtent.height, window.getHeight())),
    extent.width = std::max<uint32_t>(capabilities.minImageExtent.width, std::min<uint32_t>(capabilities.maxImageExtent.width, window.getWidth()));

    return extent;
}

Swapchain::Swapchain(Device & device, Swapchain *old) : mDevice(device)
{
    auto pd = device.getPhysicalDevice();
    //device.getInstance().createSurfaceKHR();
    auto surface = device.getInstance().getSurfaceKHR();

    mCapabilities = pd.getSurfaceCapabilitiesKHR(surface);
    mFormats = pd.getSurfaceFormatsKHR(surface);
    mPresentModes = pd.getSurfacePresentModesKHR(surface);

    auto format = ::getFormat(mFormats);
    auto presentMode = getPresentMode(mPresentModes);
    mExtent = getSwapExtent(mCapabilities, device.getInstance().getWindow());

    assert(mExtent.width > 0);
    assert(mExtent.height > 0);

    mImageCount = std::max<uint32_t>(mCapabilities.minImageCount, 3);

    if (mCapabilities.maxImageCount > 0 && mImageCount > mCapabilities.maxImageCount)
        mImageCount = mCapabilities.maxImageCount;

    vk::SwapchainCreateInfoKHR info;

    info.surface = surface;
    info.imageFormat = mFormat = format.format;
    info.imageColorSpace = format.colorSpace;
    info.imageExtent = mExtent;
    info.imageArrayLayers = 1;
    info.minImageCount = mImageCount;
    info.imageUsage = vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst;

    uint32_t gctp[] = { (uint32_t)device.getQueueFamilyIndices().graphicFamily, (uint32_t)device.getQueueFamilyIndices().computeFamily, (uint32_t)device.getQueueFamilyIndices().transferFamily, (uint32_t)device.getQueueFamilyIndices().presentFamily };

    if (gctp[0] == gctp[1] && gctp[1] == gctp[2] && gctp[2] == gctp[3])
        info.imageSharingMode = vk::SharingMode::eExclusive;

    else {
        info.imageSharingMode = vk::SharingMode::eConcurrent;
        info.queueFamilyIndexCount = 4;
        info.pQueueFamilyIndices = gctp;
    }

    info.preTransform = mCapabilities.currentTransform;
    info.clipped = true;
    info.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;

    info.presentMode = presentMode;

    if (old != nullptr)
        info.oldSwapchain = *old;

    static_cast<vk::UniqueSwapchainKHR&>(*this) = device->createSwapchainKHRUnique(info);

    mImages = device.get().getSwapchainImagesKHR(*this);
    mImageViews.reserve(mImages.size());
    mFrameBuffers.reserve(mImages.size());

    for (const auto &image : mImages)
        mImageViews.emplace_back(device, ImageView::color2D(image, mFormat));
}

const Framebuffer &Swapchain::createFramebuffer(vk::RenderPass renderpass) {
    return mFrameBuffers.emplace_back(mDevice, renderpass, mExtent, mImageViews[mFrameBuffers.size()]);
}

uint32_t Swapchain::getImageCount() const
{
    return (uint32_t)mImageViews.size();
}

const std::vector<Framebuffer>& Swapchain::getFramebuffers() const
{
    return mFrameBuffers;
}

vk::Format Swapchain::getFormat() const
{
    return mFormat;
}

vk::Extent2D Swapchain::getExtent() const
{
    return mExtent;
}
