#pragma once

#include "../renderpass.h"
#include "fullscreensubpass.h"
#include <tuple>

class RenderFullScreenPass {
public:
	RenderFullScreenPass(vk::Device device, vk::Format format, vk::ImageLayout initialLayout, vk::ImageLayout finalLayout);

	void setFragmentStage(std::string filename);
	void create(vk::Extent2D extent);

	void addUniformBuffer(vk::DescriptorBufferInfo info, uint32_t binding);
	void addSampler(vk::DescriptorImageInfo info, uint32_t binding);
	void addStorageImage(vk::DescriptorImageInfo info, uint32_t binding);

	void addPushConstant(const void *data, uint32_t size, uint32_t offset = 0u);

	void execute(vk::CommandBuffer cmd, vk::Framebuffer framebuffer);

	vk::RenderPass getRenderPass() const;

	vk::Extent2D getExtent() const;

private:
	RenderPass mRenderPass;
	std::shared_ptr<FullScreenSubPass> mSubPass;
	vk::Extent2D mExtent;
};
