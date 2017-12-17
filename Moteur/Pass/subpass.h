#pragma once

#include "../Vulkan/device.h"

class Subpass {
    friend class RenderPass;
public:

    virtual ~Subpass() = 0;

protected:
    void addColorAttachment(uint32_t index);
    void addInputAttachment(uint32_t index);
    void setDepthStencilAttachment(uint32_t index);

private:
    bool mHasDepthStencil = false;
    std::vector<vk::AttachmentReference> mColorAttachments;
    std::vector<vk::AttachmentReference> mInputAttachments;
    vk::AttachmentReference mDepthStencilAttachment;
    virtual void execute(vk::CommandBuffer cmd) = 0;

    virtual void create(vk::RenderPass renderpass, vk::Extent2D extent, uint32_t indexPass) = 0;
    vk::SubpassDescription getDescription() const;
};
