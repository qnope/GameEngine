#pragma once

#include "Device.h"

class ImageView : public vk::UniqueImageView {
public:
    ImageView(Device &device, vk::ImageViewCreateInfo const &info);

    static vk::ImageViewCreateInfo depth2D(vk::Image image, vk::Format fmt);
    static vk::ImageViewCreateInfo color2D(vk::Image image, vk::Format fmt);

    static vk::ImageViewCreateInfo color3D(vk::Image image, vk::Format fmt);

    static vk::ImageViewCreateInfo colorMipmapSelection3D(vk::Image image, uint32_t mipmap, vk::Format fmt);

private:
};
