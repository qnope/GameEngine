#pragma once

#include "SceneGraph/SceneGraph.h"
#include "Pass/Subpass.h"
#include "Pass/PipelineLayoutBuilder.h"
#include "Pass/PipelineBuilder.h"

class FillGBufferSubpass : public Subpass {
public:
    FillGBufferSubpass(vk::Device device, SceneGraph &sceneGraph, vk::DescriptorSetLayout matrixDescriptorSetLayout, vk::DescriptorSet matrixDescriptorSet, const Buffer &modelMatricesBuffer, const Buffer &indirectBuffer);

private:
    void execute(vk::CommandBuffer cmd) override;

    void create(vk::RenderPass renderPass, vk::Extent2D extent, uint32_t indexPass) override;

    vk::Device mDevice;

    SceneGraph &mSceneGraph;

    vk::DescriptorSet mMatrixDescriptorSet;

    std::vector<PipelineLayout> mPipelineLayouts;
    std::vector<Pipeline> mPipelines;

    const Buffer &mModelMatricesBuffer;
    const Buffer &mIndirectBuffer;
};
