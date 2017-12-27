#include "FillGBufferSubpass.h"
#include "Vulkan/StructHelper.h"

FillGBufferSubpass::FillGBufferSubpass(vk::Device device, SceneGraph &sceneGraph, vk::DescriptorSetLayout matrixDescriptorSetLayout, vk::DescriptorSet matrixDescriptorSet, const Buffer &modelMatricesBuffer, const Buffer &indirectBuffer) :
    mDevice(device),
    mSceneGraph(sceneGraph),
    mMatrixDescriptorSet(matrixDescriptorSet),
    mModelMatricesBuffer(modelMatricesBuffer),
    mIndirectBuffer(indirectBuffer)
{
    for (int i = 0; i < 4; ++i)
        addColorAttachment(i);

    setDepthStencilAttachment(4);

    mFunctionDefinitions = mSceneGraph.getMaterialsManager().getAllFunctionDescriptions();
    auto descriptorSetLayouts = mSceneGraph.getMaterialsManager().getAllDescriptorSetLayouts();

    for (auto &layout : descriptorSetLayouts)
        mPipelineLayouts.emplace_back(PipelineLayoutBuilder::build(mDevice, { matrixDescriptorSetLayout, layout }));
}

void FillGBufferSubpass::create(vk::RenderPass renderPass, vk::Extent2D extent, uint32_t indexPass)
{
    for (auto i(0u); i < mPipelineLayouts.size(); ++i)
        mPipelines.emplace_back(PipelineBuilder::buildFillGBufferPipeline(mDevice, renderPass, indexPass, extent, mPipelineLayouts[i], mFunctionDefinitions[i]));
}

void FillGBufferSubpass::execute(vk::CommandBuffer cmd)
{
    static std::vector<vk::Pipeline> pipelines;
    static std::vector<vk::PipelineLayout> pipelineLayouts;

    pipelines.clear();
    pipelineLayouts.clear();

    for (int i = 0; i < mPipelines.size(); ++i) {
        pipelines << *mPipelines[i];
        pipelineLayouts << mPipelineLayouts[i];
    }

    cmd.bindVertexBuffers(1, (vk::Buffer)mModelMatricesBuffer, { 0u });

    // matrix through pipelineLayout compatibilty
    cmd.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLayouts[0], 0, mMatrixDescriptorSet, {});

    mSceneGraph.prepareWithMaterials(pipelines, pipelineLayouts);
    mSceneGraph.compile(cmd, 1, mIndirectBuffer);
}
