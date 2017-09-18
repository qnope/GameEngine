#pragma once
#include "../RenderFullscreen/renderfullscreenpass.h"
#include "../RenderSceneGraph/renderdeferredpasses.h"
#include "voxelizationpass.h"

struct VXAOParameter {
	float offsetCone{ 3.0f };
	int numberCones{ 16 };
	float oversampling{ 1.0f };
	float distanceAttenuation{ 1.f };
	float force{ 1.0f };
	float maximumDistance{ 0.1f };
};

class VXAOPass {
public:
	VXAOPass(vk::Device device, BufferFactory &bufferFactory, const Buffer &perspectiveViewMatriceBuffer, RenderDeferredPasses &renderDeferredPass, VoxelizationPass &voxelizationPass, vk::Extent2D extent);

	void setVXAOParameters(VXAOParameter parameters);

	vk::RenderPass getRenderPass() const;
	vk::Extent2D getExtent() const;

	void execute(vk::CommandBuffer cmd, vk::Framebuffer framebuffer);

private:
	vk::Device mDevice;
	RenderFullScreenPass mRenderFullScreenPass;
	
	const Buffer &mPerspectiveViewMatriceBuffer;
	const Buffer &mCubeVoxelizationInfoBuffer;

	Buffer mVXAOParameterStagingBuffer;
	Buffer mVXAOParameterBuffer;

	const CombinedImage &mDepthMap;
	const CombinedImage &mTangentMap;
	const CombinedImage &mNormalMap;
	const CombinedImage &mVoxelGrid;

	Sampler m3DSampler{ mDevice, Sampler::simple(vk::Filter::eLinear, vk::SamplerMipmapMode::eLinear, vk::SamplerAddressMode::eClampToEdge) };
};