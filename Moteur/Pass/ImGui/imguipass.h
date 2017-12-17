#pragma once

#include "../../imgui/imguiinstance.h"
#include "../../Vulkan/framebuffer.h"
#include "../renderpassbuilder.h"
#include "imguisubpass.h"

class ImGUIPass {
public:
    ImGUIPass(ImGUIInstance &imGUIInstance, Device &device, vk::Format format, vk::Extent2D extent, BufferFactory &bufferFactory);

    void newFrame();
    vk::RenderPass getRenderPass() const;

    vk::CommandBuffer execute(const Framebuffer &framebuffer);

private:
    ImGUIInstance &mImGUIInstance;
    vk::Device mDevice;
    RenderPass mRenderPass;
    std::shared_ptr<ImGUISubpass> mSubpass;
    CommandPool mTransientCommandPool;
    vk::UniqueCommandBuffer mCommandBuffer;
};
