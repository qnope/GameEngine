#pragma once

#include "Vulkan/Framebuffer.h"

struct FramebufferBuilder {
    static Framebuffer gBuffer(vk::Device device, ImageFactory &imageFactory, vk::Extent2D extent, vk::RenderPass renderPass);
    static Framebuffer emptyFrameBuffer(vk::Device device, vk::Extent2D extent, vk::RenderPass renderPass);
    static Framebuffer fullscreen(vk::Device device, ImageFactory &imageFactory, vk::Format format, vk::Extent2D extent, vk::RenderPass renderPass);
};
