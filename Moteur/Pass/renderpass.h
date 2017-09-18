#pragma once

#include "../Vulkan/device.h"
#include "../Vulkan/structhelper.h"
#include "subpass.h"

class RenderPass : public vk::UniqueRenderPass {
public:
	RenderPass() = default;
	RenderPass(vk::Device device);

	std::shared_ptr<Subpass> getFirstSubpass() const;
	std::shared_ptr<Subpass> getLastSubpass() const;

	void addDependency(std::shared_ptr<Subpass> src, std::shared_ptr<Subpass> dst,
		vk::PipelineStageFlags srcStage = vk::PipelineStageFlagBits::eAllCommands, vk::PipelineStageFlags dstStage = vk::PipelineStageFlagBits::eAllCommands,
		vk::AccessFlags srcAccess = Access::allBits, vk::AccessFlags dstAccess = Access::allBits,
		vk::DependencyFlags dependencyFlags = vk::DependencyFlags());

	void addSubpass(std::shared_ptr<Subpass> subpass, bool secondaryCommandBuffers);

	void addAttachment(vk::AttachmentDescription attachmentDescription, vk::ClearValue value);

	void create(vk::Extent2D extent);

	void execute(vk::CommandBuffer cmd, vk::Framebuffer framebuffer, vk::Extent2D extent);

private:
	vk::Device mDevice;
	
	std::vector<vk::AttachmentDescription> mAttachmentDescriptions;
	std::vector<vk::ClearValue> mClearValues;

	std::vector<vk::SubpassDependency> mDependencies;

	std::vector<std::shared_ptr<Subpass>> mSubpasses;
	std::vector<vk::SubpassContents> mSubpassesContent;

	uint32_t subpassIndex(std::shared_ptr<Subpass> pass) const;
};
