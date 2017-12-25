#include "Subpass.h"

void Subpass::addColorAttachment(uint32_t index)
{
    mColorAttachments.emplace_back(index, vk::ImageLayout::eColorAttachmentOptimal);
}

void Subpass::addInputAttachment(uint32_t index)
{
    mInputAttachments.emplace_back(index, vk::ImageLayout::eShaderReadOnlyOptimal);
}

void Subpass::setDepthStencilAttachment(uint32_t index)
{
    mDepthStencilAttachment = vk::AttachmentReference(index, vk::ImageLayout::eDepthStencilAttachmentOptimal);
    mHasDepthStencil = true;
}

vk::SubpassDescription Subpass::getDescription() const
{
    return vk::SubpassDescription(vk::SubpassDescriptionFlags(), vk::PipelineBindPoint::eGraphics,
        (uint32_t)mInputAttachments.size(), mInputAttachments.data(),
        (uint32_t)mColorAttachments.size(), mColorAttachments.data(),
        nullptr,
        mHasDepthStencil ? &mDepthStencilAttachment : nullptr,
        0u, nullptr);
}
