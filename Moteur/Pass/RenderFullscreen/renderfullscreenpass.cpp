#include "renderfullscreenpass.h"
#include "../renderpassbuilder.h"
#include "../../Vulkan/structhelper.h"
#include "../../Tools/stream.h"

RenderFullScreenPass::RenderFullScreenPass(vk::Device device, vk::Format format, vk::ImageLayout initialLayout, vk::ImageLayout finalLayout) {
    std::tie(mRenderPass, mSubPass) = RenderPassBuilder::fullScreen(device, format, initialLayout, finalLayout);
}

void RenderFullScreenPass::setFragmentStage(std::string filename)
{
    mSubPass->setFragmentStage(filename);
}

void RenderFullScreenPass::create(vk::Extent2D extent)
{
    mRenderPass.create(extent);
    mExtent = extent;
}

void RenderFullScreenPass::addUniformBuffer(vk::DescriptorBufferInfo info, uint32_t binding)
{
    mSubPass->addUniformBuffer(info, binding);
}

void RenderFullScreenPass::addSampler(vk::DescriptorImageInfo info, uint32_t binding)
{
    mSubPass->addSampler(info, binding);
}

void RenderFullScreenPass::addStorageImage(vk::DescriptorImageInfo info, uint32_t binding)
{
    mSubPass->addStorageImage(info, binding);
}

void RenderFullScreenPass::addPushConstant(const void *data, uint32_t size, uint32_t offset)
{
    mSubPass->addPushConstant(vk::PushConstantRange(vk::ShaderStageFlagBits::eFragment, 0, size), data);
}

void RenderFullScreenPass::execute(vk::CommandBuffer cmd, vk::Framebuffer framebuffer)
{
    mRenderPass.execute(cmd, framebuffer, mExtent);
}

vk::RenderPass RenderFullScreenPass::getRenderPass() const
{
    return mRenderPass;
}

vk::Extent2D RenderFullScreenPass::getExtent() const
{
    return mExtent;
}
