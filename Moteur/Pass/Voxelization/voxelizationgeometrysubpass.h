#pragma once

#include "Pass/Subpass.h"
#include "SceneGraph/SceneGraph.h"
#include "Pass/PipelineBuilder.h"
#include "Transfer/ImageFactory.h"

class VoxelizationGeometrySubpass : public Subpass {
public:
    VoxelizationGeometrySubpass(vk::Device device, uint32_t resolution, uint32_t clipMapNumber, SceneGraph & sceneGraph, vk::PipelineLayout pipelineLayout, vk::DescriptorSet cubeVoxelizationInfoImageDescriptorSet, const Buffer &cubeVoxelizationInfoBuffer, const Buffer & modelMatricesBuffer, const Buffer & indirectBuffer, std::unique_ptr<CombinedImage> & combinedImage, ImageFactory & imageFactory);

private:
    void create(vk::RenderPass renderPass, vk::Extent2D extent, uint32_t indexPass) override;

    void execute(vk::CommandBuffer cmd) override;

    vk::Device mDevice;

    SceneGraph &mSceneGraph;

    uint32_t mResolution;

    vk::PipelineLayout mPipelineLayout;
    vk::DescriptorSet mCubeVoxelizationInfoImageDescriptorSet;

    Pipeline mPipeline;

    const Buffer &mModelMatricesBuffer;
    const Buffer &mIndirectBuffer;
};
