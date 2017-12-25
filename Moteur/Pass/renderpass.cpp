#include "RenderPass.h"

RenderPass::RenderPass(vk::Device device) :
    mDevice(device)
{
}

std::shared_ptr<Subpass> RenderPass::getFirstSubpass() const
{
    return mSubpasses.front();
}

std::shared_ptr<Subpass> RenderPass::getLastSubpass() const
{
    return mSubpasses.back();
}

uint32_t RenderPass::subpassIndex(std::shared_ptr<Subpass> pass) const
{
    if (pass == nullptr)
        return VK_SUBPASS_EXTERNAL;
    auto it = std::find(mSubpasses.begin(), mSubpasses.end(), pass);
    assert(it != mSubpasses.end());
    return (uint32_t)std::distance(mSubpasses.begin(), it);
}

void RenderPass::addDependency(std::shared_ptr<Subpass> src, std::shared_ptr<Subpass> dst, vk::PipelineStageFlags srcStage, vk::PipelineStageFlags dstStage, vk::AccessFlags srcAccess, vk::AccessFlags dstAccess, vk::DependencyFlags dependencyFlags)
{
    auto srcIndex = subpassIndex(src);
    auto dstIndex = subpassIndex(dst);

    mDependencies.emplace_back(srcIndex, dstIndex, srcStage, dstStage, srcAccess, dstAccess, dependencyFlags);
}

void RenderPass::addSubpass(std::shared_ptr<Subpass> subpass, bool secondaryCommandBuffers)
{
    mSubpasses.emplace_back(subpass);
    mSubpassesContent.emplace_back((!secondaryCommandBuffers) ? vk::SubpassContents::eInline : vk::SubpassContents::eSecondaryCommandBuffers);
}

void RenderPass::addAttachment(vk::AttachmentDescription attachmentDescription, vk::ClearValue value)
{
    mAttachmentDescriptions.emplace_back(attachmentDescription);
    mClearValues.emplace_back(value);
}

void RenderPass::create(vk::Extent2D extent)
{
    vk::RenderPassCreateInfo info;
    std::vector<vk::SubpassDescription> subpasses;

    for (const auto &s : mSubpasses)
        subpasses.emplace_back(s->getDescription());

    info.attachmentCount = (uint32_t)mAttachmentDescriptions.size();
    info.dependencyCount = (uint32_t)mDependencies.size();
    info.subpassCount = (uint32_t)subpasses.size();

    info.pAttachments = mAttachmentDescriptions.data();
    info.pDependencies = mDependencies.data();
    info.pSubpasses = subpasses.data();

    static_cast<vk::UniqueRenderPass&>(*this) = mDevice.createRenderPassUnique(info);
    unsigned int i = 0;
    for (auto subpass : mSubpasses)
        subpass->create(*this, extent, i++);
}

void RenderPass::execute(vk::CommandBuffer cmd, vk::Framebuffer framebuffer, vk::Extent2D extent)
{
    vk::RenderPassBeginInfo info;
    info.renderPass = *this;
    info.framebuffer = framebuffer;
    info.renderArea = vk::Rect2D({}, extent);
    info.clearValueCount = (uint32_t)mClearValues.size();
    info.pClearValues = mClearValues.data();

    cmd.beginRenderPass(info, mSubpassesContent[0]);

    for (uint32_t i = 0; i < mSubpasses.size(); ++i) {
        mSubpasses[i]->execute(cmd);
        if (i != mSubpasses.size() - 1)
            cmd.nextSubpass(mSubpassesContent[i + 1]);
    }

    cmd.endRenderPass();
}
