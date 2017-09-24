#pragma once

#include "../../Vulkan/framebuffer.h"
#include "../renderpass.h"
#include "../../Descriptor/descriptorpoolbuilder.h"
#include "../../SceneGraph/scenegraph.h"

class RenderDeferredPasses {
public:
	RenderDeferredPasses(Device &device, vk::Extent2D extent, ImageFactory &imageFactory, SceneGraph &sceneGraph, const Buffer &perspectiveViewMatrixBuffer, const Buffer &modelMatricesBuffer, const Buffer &indirectBuffer);

	void execute(vk::CommandBuffer cmd);

	const CombinedImage &getAlbedoMap() const;
	const CombinedImage &getTangentMap() const;
	const CombinedImage &getNormalMap() const;
	const CombinedImage &getRoughnessMetallicMap() const;
	const CombinedImage &getDepthMap() const;

private:
	vk::Device mDevice;

	RenderPass mRenderpass;
	Framebuffer mFramebuffer;
	vk::Extent2D mExtent;

	DescriptorPoolWrapper mMatrixDescriptorPool{ DescriptorPoolBuilder::monoNonDynamicUniformBuffer(mDevice, 1) };
	DescriptorSetLayout mMatrixDescriptorSetLayout{ DescriptorSetLayoutBuilder::monoUniformBuffer(mDevice, vk::ShaderStageFlagBits::eVertex) };
	vk::DescriptorSet mMatrixDescriptorSet{ mMatrixDescriptorPool.allocate(mMatrixDescriptorSetLayout) };
};
