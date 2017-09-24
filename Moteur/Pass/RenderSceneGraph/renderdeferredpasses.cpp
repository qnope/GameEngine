#include "renderdeferredpasses.h"
#include "../renderpassbuilder.h"
#include "../framebufferbuilder.h"
#include "../../Descriptor/descriptorsetlayoutbuilder.h"


RenderDeferredPasses::RenderDeferredPasses(Device & device, vk::Extent2D extent, ImageFactory & imageFactory, SceneGraph & sceneGraph, const Buffer & perspectiveViewMatrixBuffer, const Buffer & modelMatricesBuffer, const Buffer & indirectBuffer) :
	mDevice(device),
	mExtent(extent)
{
	auto matrixInfo = vk::DescriptorBufferInfo(perspectiveViewMatrixBuffer, 0, VK_WHOLE_SIZE);
	auto writeMatrix = StructHelper::writeDescriptorSet(mMatrixDescriptorSet, 0, &matrixInfo, vk::DescriptorType::eUniformBuffer);

	device->updateDescriptorSets(writeMatrix, {});

	std::tie(mRenderpass, std::ignore) = RenderPassBuilder::relatedSceneGraph(mDevice, sceneGraph, mMatrixDescriptorSetLayout, mMatrixDescriptorSet, extent, modelMatricesBuffer, indirectBuffer);
	mFramebuffer = FramebufferBuilder::gBuffer(device, imageFactory, extent, *mRenderpass);
}

void RenderDeferredPasses::execute(vk::CommandBuffer cmd)
{
	mRenderpass.execute(cmd, *mFramebuffer, mExtent);
}

const CombinedImage & RenderDeferredPasses::getAlbedoMap() const
{
	return mFramebuffer.getCombinedImage(0);
}

const CombinedImage & RenderDeferredPasses::getTangentMap() const
{
	return mFramebuffer.getCombinedImage(1);
}

const CombinedImage & RenderDeferredPasses::getNormalMap() const
{
	return mFramebuffer.getCombinedImage(2);
}

const CombinedImage & RenderDeferredPasses::getRoughnessMetallicMap() const
{
	return mFramebuffer.getCombinedImage(3);
}

const CombinedImage & RenderDeferredPasses::getDepthMap() const
{
	return mFramebuffer.getCombinedImage(4);
}


