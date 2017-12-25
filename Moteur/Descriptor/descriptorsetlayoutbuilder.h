#pragma once

#include "Vulkan/Descriptorsetlayout.h"
#include "Vulkan/StructHelper.h"
#include "Tools/variadics.h"

struct DescriptorSetLayoutBuilder {
private:
    template<typename ...Bindings>
    static DescriptorSetLayout build(vk::Device device, Bindings ...bindings) {
        DescriptorSetLayout layout(device);
        EXPAND_FUNCTION(layout.addBinding(bindings));
        layout.create();
        return layout;
    }

public:
    static DescriptorSetLayout monoUniformBuffer(vk::Device device, vk::ShaderStageFlags flags) {
        return build(device, LayoutBinding::uniformBuffer(0, 1, flags));
    }

    static DescriptorSetLayout monoUniformBufferDynamic(vk::Device device, vk::ShaderStageFlags flags) {
        return build(device, LayoutBinding::uniformBufferDynamic(0, 1, flags));
    }

    static DescriptorSetLayout monoCombinedImage(vk::Device device, vk::ShaderStageFlags flags) {
        return build(device, LayoutBinding::combinedImage(0, 1, flags));
    }

    static DescriptorSetLayout monoUniformBufferMonoStorageImage(vk::Device device, vk::ShaderStageFlags flagUniformBuffer, vk::ShaderStageFlags flagStorageImage) {
        return build(device, LayoutBinding::uniformBuffer(0, 1, flagUniformBuffer), LayoutBinding::storageImage(1, 1, flagStorageImage));
    }

    static DescriptorSetLayout voxelDownsampler(vk::Device device) {
        return build(device, LayoutBinding::uniformBuffer(0, 1, vk::ShaderStageFlagBits::eCompute), LayoutBinding::combinedImage(1, 1, vk::ShaderStageFlagBits::eCompute), LayoutBinding::storageImage(2, 1, vk::ShaderStageFlagBits::eCompute));
    }

    static DescriptorSetLayout normalPBRTexture(vk::Device device, vk::ShaderStageFlags flags) {
        return build(device, LayoutBinding::combinedImage(0, 1, flags), LayoutBinding::combinedImage(1, 1, flags), LayoutBinding::combinedImage(2, 1, flags), LayoutBinding::combinedImage(3, 1, flags));
    }
};
