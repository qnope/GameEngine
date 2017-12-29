#pragma once

#include "Descriptor/DescriptorPoolWrapper.h"
#include "Vulkan/Descriptorsetlayout.h"
#include "SceneGraph/ModelImporter/Material.h"
#include "SceneGraph/Drawer.h"

class AbstractUniqueMaterialManager {
public:
    struct MaterialPointer {
        AbstractUniqueMaterialManager *ptr;
        uint32_t index;
    };

    AbstractUniqueMaterialManager(vk::Device device, std::string pathToMaterialFunctionDefinition);

    void changePipeline(vk::Pipeline pipeline);
    void changePipelineLayout(vk::PipelineLayout pipelineLayout);

    std::string getFunctionDefinition() const;

    vk::DescriptorSetLayout getDescriptorSetLayout() const;

    virtual bool isAccepted(Material material) const = 0;
    MaterialPointer addMaterial(Material material);
    void getDrawerMaterialValues(Drawer &drawer, const MaterialPointer &ptr) const;

    virtual ~AbstractUniqueMaterialManager() = default;

protected:
    virtual void registerMaterial(Material material) = 0;
    virtual void getDrawerDescriptorSet(Drawer &drawer, const MaterialPointer &ptr) const = 0;

protected:
    vk::Device mDevice;

    vk::Pipeline mPipeline; /// Associated pipeline to this material
    vk::PipelineLayout mPipelineLayout; /// Associated pipelineLayout to this material
    std::unique_ptr<DescriptorPoolWrapper> mDescriptorPool;
    std::unique_ptr<DescriptorSetLayout> mDescriptorSetLayout;
    std::string mMaterialFunctionDefinition;

    uint32_t mNumberMaterialRegistered{0};
};
