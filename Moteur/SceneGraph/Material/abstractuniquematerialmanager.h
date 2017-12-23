#pragma once

#include "../../Descriptor/descriptorpoolwrapper.h"
#include "../../Vulkan/descriptorsetlayout.h"
#include "../ModelImporter/material.h"
#include "../drawer.h"

class AbstractUniqueMaterialManager {
public:
    struct MaterialPointer {
        AbstractUniqueMaterialManager *ptr;
        uint32_t index;
    };

    AbstractUniqueMaterialManager(vk::Device device);

    void changePipeline(vk::Pipeline pipeline);
    void changePipelineLayout(vk::PipelineLayout pipelineLayout);

    vk::DescriptorSetLayout getDescriptorSetLayout() const;

    virtual bool isAccepted(Material material) const = 0;
    virtual MaterialPointer addMaterial(Material material) = 0;
    virtual void getDrawerMaterialValues(Drawer &drawer, const MaterialPointer &ptr) const;

    virtual ~AbstractUniqueMaterialManager() = default;

protected:
    vk::Device mDevice;

    vk::Pipeline mPipeline; /// Associated pipeline to this material
    vk::PipelineLayout mPipelineLayout; /// Associated pipelineLayout to this material
    std::unique_ptr<DescriptorPoolWrapper> mDescriptorPool;
    std::unique_ptr<DescriptorSetLayout> mDescriptorSetLayout;
};