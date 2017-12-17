#include "voxelizationgeometrysubpass.h"

VoxelizationGeometrySubPass::VoxelizationGeometrySubPass(vk::Device device, uint32_t resolution, uint32_t clipMapNumber, SceneGraph & sceneGraph, vk::PipelineLayout pipelineLayout, vk::DescriptorSet cubeVoxelizationInfoImageDescriptorSet, const Buffer &cubeVoxelizationInfoBuffer, const Buffer & modelMatricesBuffer, const Buffer & indirectBuffer, std::unique_ptr<CombinedImage> &combinedImage, ImageFactory &imageFactory) :
    mDevice(device),
    mSceneGraph(sceneGraph),
    mResolution(resolution),
    mPipelineLayout(pipelineLayout),
    mCubeVoxelizationInfoImageDescriptorSet(cubeVoxelizationInfoImageDescriptorSet),
    mModelMatricesBuffer(modelMatricesBuffer),
    mIndirectBuffer(indirectBuffer)
{
    combinedImage = std::make_unique<CombinedImage>(imageFactory.createEmptyImage(
        Image::anisotropicOpacityVoxel(resolution, clipMapNumber),
        ImageView::color3D(vk::Image(), vk::Format::eR8Unorm)));

    vk::DescriptorBufferInfo bufferInfo{ cubeVoxelizationInfoBuffer, 0, VK_WHOLE_SIZE };
    vk::DescriptorImageInfo imageInfo{ vk::Sampler(), *combinedImage, vk::ImageLayout::eGeneral };
    std::vector<vk::WriteDescriptorSet> writeSet{ { StructHelper::writeDescriptorSet(cubeVoxelizationInfoImageDescriptorSet, 1, &imageInfo, vk::DescriptorType::eStorageImage) , StructHelper::writeDescriptorSet(cubeVoxelizationInfoImageDescriptorSet, 0, &bufferInfo, vk::DescriptorType::eUniformBuffer) } };
    device.updateDescriptorSets(writeSet, {});
}

void VoxelizationGeometrySubPass::create(vk::RenderPass renderPass, vk::Extent2D extent, uint32_t indexPass)
{
    mPipeline = PipelineBuilder::buildVoxelizationOnlyGeometryPipeline(mDevice, renderPass, mResolution, mPipelineLayout);
}


void VoxelizationGeometrySubPass::execute(vk::CommandBuffer cmd)
{
    cmd.bindVertexBuffers(1, (vk::Buffer)mModelMatricesBuffer, { 0u });

    cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, mPipeline);
    cmd.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, mPipelineLayout, 0, mCubeVoxelizationInfoImageDescriptorSet, {});

    mSceneGraph.prepareWithoutMaterials(mPipeline, mPipelineLayout);
    mSceneGraph.compile(cmd, 1, mIndirectBuffer);
}