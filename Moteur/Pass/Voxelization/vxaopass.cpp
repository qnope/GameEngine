#include "VXAOPass.h"

VXAOPass::VXAOPass(vk::Device device, BufferFactory & bufferFactory, const Buffer & perspectiveViewMatriceBuffer, RenderDeferredPasses & renderDeferredPass, VoxelizationPass & voxelizationPass, vk::Extent2D extent) :
    mDevice(device),
    mRenderFullScreenPass(device, vk::Format::eR8Unorm, vk::ImageLayout::eUndefined, vk::ImageLayout::eShaderReadOnlyOptimal),
    mPerspectiveViewMatriceBuffer(perspectiveViewMatriceBuffer),
    mCubeVoxelizationInfoBuffer(voxelizationPass.getCubeVoxelizationInfoBuffer()),
    mVXAOParameterStagingBuffer(bufferFactory.createEmptyBuffer(sizeof(VXAOParameter), vk::BufferUsageFlagBits::eTransferSrc, false)),
    mVXAOParameterBuffer(bufferFactory.createEmptyBuffer(sizeof(VXAOParameter), vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eUniformBuffer, true)),
    mDepthMap(renderDeferredPass.getDepthMap()),
    mTangentMap(renderDeferredPass.getTangentMap()),
    mNormalMap(renderDeferredPass.getNormalMap()),
    mVoxelGrid(voxelizationPass.getVoxelGrid())
{
    mRenderFullScreenPass.addUniformBuffer(vk::DescriptorBufferInfo(mPerspectiveViewMatriceBuffer, 0, VK_WHOLE_SIZE), 0);
    mRenderFullScreenPass.addUniformBuffer(vk::DescriptorBufferInfo(mCubeVoxelizationInfoBuffer, 0, VK_WHOLE_SIZE), 1);
    mRenderFullScreenPass.addUniformBuffer(vk::DescriptorBufferInfo(mVXAOParameterBuffer, 0, VK_WHOLE_SIZE), 2);

    mRenderFullScreenPass.addSampler(vk::DescriptorImageInfo(mDepthMap, mDepthMap, vk::ImageLayout::eDepthStencilReadOnlyOptimal), 3);
    mRenderFullScreenPass.addSampler(vk::DescriptorImageInfo(mTangentMap, mTangentMap, vk::ImageLayout::eShaderReadOnlyOptimal), 4);
    mRenderFullScreenPass.addSampler(vk::DescriptorImageInfo(mNormalMap, mNormalMap, vk::ImageLayout::eShaderReadOnlyOptimal), 5);
    mRenderFullScreenPass.addSampler(vk::DescriptorImageInfo(m3DSampler, mVoxelGrid, vk::ImageLayout::eShaderReadOnlyOptimal), 6);

    mRenderFullScreenPass.setFragmentStage("../Shaders/Voxelization/vxao.frag");

    mRenderFullScreenPass.create(extent);
}

void VXAOPass::setVXAOParameters(VXAOParameter parameters)
{
    mVXAOParameterStagingBuffer.resetOffset();
    mVXAOParameterStagingBuffer.push_data(parameters);
}

void VXAOPass::execute(vk::CommandBuffer cmd, vk::Framebuffer framebuffer)
{
    auto postTransferBarrier = MemoryBarrier::buffer(mVXAOParameterBuffer, vk::AccessFlagBits::eTransferWrite, vk::AccessFlags()); // visibility done in the renderpass 

    StructHelper::transfer(mVXAOParameterStagingBuffer, mVXAOParameterBuffer, cmd);

    MemoryBarrier::barrier(postTransferBarrier, cmd, vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eBottomOfPipe);

    mRenderFullScreenPass.execute(cmd, framebuffer);
}

vk::RenderPass VXAOPass::getRenderPass() const {
    return mRenderFullScreenPass.getRenderPass();
}

vk::Extent2D VXAOPass::getExtent() const
{
    return mRenderFullScreenPass.getExtent();
}
