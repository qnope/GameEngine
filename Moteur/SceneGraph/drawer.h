#pragma once

#include "Vulkan/vulkan.h"

struct Drawer {
    vk::Buffer vbo;
    vk::Buffer ibo;

    vk::Pipeline pipeline;
    vk::PipelineLayout pipelineLayout;

    vk::DescriptorSet materialSet;
    uint32_t offsetMaterialUbo = (uint32_t)~0u;
};
