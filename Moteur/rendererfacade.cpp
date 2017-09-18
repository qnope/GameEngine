#include "rendererfacade.h"

RendererFacade::RendererFacade(Device & device, SceneGraph & sceneGraph, BufferFactory & bufferFactory, ImageFactory & imageFactory, ImGUIInstance & imguiInstance, Swapchain &swapchain, vk::Extent2D extent) :
	mDevice(device),
	mSceneGraph(sceneGraph),
	mBufferFactory(bufferFactory),
	mImageFactory(imageFactory),
	mImGUIInstance(imguiInstance),
	mExtent(extent)
{
	createSceneGraphBuffers();
	createDeferredPasses();
	createFinalRenderingPass(swapchain);

	createVoxelizationPass();

	createVXAOPass();

	createImGUIPass();
	mVoxelGridHasChanged = false;
}

void RendererFacade::setParameters(EngineParameter parameter)
{
	mEngineParameters = parameter;
	createVoxelizationPass();
	createVXAOPass();
	mVoxelGridHasChanged = false;
	//mCurrentType = -1;
}

void RendererFacade::newFrame()
{
	mImguiPass->newFrame();
}

vk::Semaphore RendererFacade::execute(vk::Semaphore waitImageAvailableSemaphore)
{
	if (mEngineParameters.voxelizationPassParameter.enable) {
		mFencePresentableImage.wait();
		mFencePresentableImage.reset();

		if (mVoxelizationPassAlreadySubmitted)
			mVoxelizationPassProfiling = mVoxelizationPass->getProfiling();

		else
			mVoxelizationPassProfiling.clear();

		auto sceneGraphBuffersComputing = mSceneGraphBuffers->execute();
		mVoxelizationPass->prepare();
		mSubmitter.addCommandBuffer(sceneGraphBuffersComputing);
		mSubmitter.addCommandBuffer(mVoxelizationPassCommandBuffer);

		mSubmitter.submit(mDevice.getGraphicQueue());
		mVoxelizationPassAlreadySubmitted = true;
	}

	if (!mEngineParameters.voxelizationPassParameter.enable) {
		mFencePresentableImage.wait();
		mFencePresentableImage.reset();
		auto sceneGraphBuffersComputing = mSceneGraphBuffers->execute();
		mSubmitter.addCommandBuffer(sceneGraphBuffersComputing);
	}

	auto imguiCommandBuffer = mImguiPass->execute(*mSwapchainFramebuffer);

	mSubmitter.addCommandBuffer(mRenderDeferredPassesCommandBuffer);

	if (mEngineParameters.ambientOcclusionParameter.vxao && mEngineParameters.voxelizationPassParameter.enable && mEngineParameters.presentationPassParameter.type > PURE_DEBUG) {
		mVxaoPass->setVXAOParameters(mEngineParameters.ambientOcclusionParameter.vxaoParameter);
		mSubmitter.addCommandBuffer(mVxaoPassCommandBuffer);
	}

	mSubmitter.addCommandBuffer(choseTheGoodPresentationPass());

	mSubmitter.addCommandBuffer(imguiCommandBuffer);

	// We wait at color attachment output : We must not write into the image if it is not available : imageAvailableSemaphore is not signaled
	mSubmitter.addWaitSemaphore(waitImageAvailableSemaphore);
	mSubmitter.addWaitingStage(vk::PipelineStageFlagBits::eColorAttachmentOutput);

	mSubmitter.addSignalSemaphore(mImagePresentable);

	mSubmitter.submit(mDevice.getGraphicQueue(), mFencePresentableImage);

	return mImagePresentable;
}

const std::vector<Profiling>* RendererFacade::getVoxelizationPassProfiling() const
{
	return (mVoxelizationPassProfiling.empty()) ? nullptr : &mVoxelizationPassProfiling;
}

void RendererFacade::createSceneGraphBuffers()
{
	mSceneGraphBuffers = std::make_unique<SceneGraphBuffer>(mDevice, mSceneGraph, mBufferFactory);
}

void RendererFacade::createDeferredPasses()
{
	mRenderDeferredPasses = std::make_unique<RenderDeferredPasses>(mDevice, mExtent, mImageFactory, mSceneGraph, mSceneGraphBuffers->getPerspectiveViewMatrixBuffer(), mSceneGraphBuffers->getModelMatricesBuffer(), mSceneGraphBuffers->getIndirectBuffer());

	mRenderDeferredPassesCommandBuffer = std::move(mCommandPool.allocate(vk::CommandBufferLevel::ePrimary, 1)[0]);

	mRenderDeferredPassesCommandBuffer->begin(StructHelper::beginCommand());
	mRenderDeferredPasses->execute(mRenderDeferredPassesCommandBuffer);
	mRenderDeferredPassesCommandBuffer->end();
}

void RendererFacade::createFinalRenderingPass(Swapchain & swapchain)
{
	mFinalRenderingPass = std::make_unique<RenderFullScreenPass>(mDevice, vk::Format::eB8G8R8A8Unorm, vk::ImageLayout::eUndefined, vk::ImageLayout::eShaderReadOnlyOptimal);
	mFinalRenderingPass->setFragmentStage("../Shaders/RenderFullScreen/finalrendering.spv");
	mFinalRenderingPass->addSampler(vk::DescriptorImageInfo(mRenderDeferredPasses->getAlbedoMap(), mRenderDeferredPasses->getAlbedoMap(), vk::ImageLayout::eShaderReadOnlyOptimal), 0);
	mFinalRenderingPass->create(mExtent);

	mSwapchainFramebuffer = &swapchain.createFramebuffer(mFinalRenderingPass->getRenderPass());

	mFinalRenderingPassCommandBuffer = std::move(mCommandPool.allocate(vk::CommandBufferLevel::ePrimary, 1)[0]);

	mFinalRenderingPassCommandBuffer->begin(StructHelper::beginCommand());
	mFinalRenderingPass->execute(mFinalRenderingPassCommandBuffer, *mSwapchainFramebuffer);
	mFinalRenderingPassCommandBuffer->end();
}

void RendererFacade::createVoxelizationPass()
{
	if (!mEngineParameters.voxelizationPassParameter.enable) {
		mFencePresentableImage.wait();
		mVoxelizationPass = nullptr;
		mVoxelizationPassCommandBuffer.reset(vk::CommandBuffer());
		mVoxelizationPassAlreadySubmitted = false;
		mVoxelizationPassProfiling.clear();
		return;
	}
	
	auto resolution = 1 << mEngineParameters.voxelizationPassParameter.resolutionInPowerOfTwo;

	if (mVoxelizationPass == nullptr || mVoxelizationPass->getVoxelGridResolution() != resolution) {
		mFencePresentableImage.wait();
		mCurrentType = -1;

		mVoxelizationPass = std::make_unique<VoxelizationPass>(mDevice, resolution, mEngineParameters.voxelizationPassParameter.clipMapNumber, mSceneGraph, mSceneGraphBuffers->getModelMatricesBuffer(), mSceneGraphBuffers->getIndirectBuffer(), mImageFactory, mBufferFactory);
		mVoxelizationPassCommandBuffer = std::move(mCommandPool.allocate(vk::CommandBufferLevel::ePrimary, 1)[0]);
		mVoxelizationPassCommandBuffer->begin(StructHelper::beginCommand());
		mVoxelizationPass->execute(mVoxelizationPassCommandBuffer);
		mVoxelizationPassCommandBuffer->end();
		mVoxelGridHasChanged = true;
		mVoxelizationPassAlreadySubmitted = false;
	}
}

void RendererFacade::createVXAOPass()
{
	if (!mEngineParameters.ambientOcclusionParameter.vxao || !mEngineParameters.voxelizationPassParameter.enable) {
		mFencePresentableImage.wait();
		mVxaoPass = nullptr;
		mVxaoPassCommandBuffer.reset(vk::CommandBuffer());
		return;
	}

	auto extent = vk::Extent2D{ mExtent.width / mEngineParameters.ambientOcclusionParameter.downSampling, mExtent.height / mEngineParameters.ambientOcclusionParameter.downSampling };

	if ((mVxaoPass != nullptr && mVxaoPass->getExtent() == extent) && !mVoxelGridHasChanged)
		return;

	mVxaoPass = std::make_unique<VXAOPass>(mDevice, mBufferFactory, mSceneGraphBuffers->getPerspectiveViewMatrixBuffer(), *mRenderDeferredPasses, *mVoxelizationPass, extent);

	mAmbientOcclusionFramebuffer = FramebufferBuilder::fullscreen(mDevice, mImageFactory, vk::Format::eR8Unorm, extent, mVxaoPass->getRenderPass());
	
	mVxaoPassCommandBuffer = std::move(mCommandPool.allocate(vk::CommandBufferLevel::ePrimary, 1)[0]);
	mVxaoPassCommandBuffer->begin(StructHelper::beginCommand());
	mVxaoPass->execute(mVxaoPassCommandBuffer, mAmbientOcclusionFramebuffer);
	mVxaoPassCommandBuffer->end();
}

void RendererFacade::createImGUIPass()
{
	mImguiPass = std::make_unique<ImGUIPass>(mImGUIInstance, mDevice, vk::Format::eB8G8R8A8Unorm, mExtent, mBufferFactory);
}

void RendererFacade::createVisualizePass(const CombinedImage & combinedImage, bool isMonochromatic, bool needGammaCorrection)
{
	std::vector<unsigned> rangesData = { isMonochromatic, needGammaCorrection };
	mVisualizePass = std::make_unique<RenderFullScreenPass>(mDevice, vk::Format::eB8G8R8A8Unorm, vk::ImageLayout::eUndefined, vk::ImageLayout::eShaderReadOnlyOptimal);
	mVisualizePass->setFragmentStage("../Shaders/RenderFullScreen/visualizationbuffer.spv");
	mVisualizePass->addSampler(vk::DescriptorImageInfo(combinedImage, combinedImage, vk::ImageLayout::eShaderReadOnlyOptimal), 0);
	mVisualizePass->addPushConstant(rangesData.data(), (uint32_t)rangesData.size() * sizeof(unsigned));
	mVisualizePass->create(mExtent);
	
	mVisualizePassCommandBuffer = std::move(mCommandPool.allocate(vk::CommandBufferLevel::ePrimary, 1)[0]);

	mVisualizePassCommandBuffer->begin(StructHelper::beginCommand());
	mVisualizePass->execute(mVisualizePassCommandBuffer, *mSwapchainFramebuffer);
	mVisualizePassCommandBuffer->end();
}

void RendererFacade::createVisualizeAlbedoPass()
{
	createVisualizePass(mRenderDeferredPasses->getAlbedoMap(), false, true);
}

void RendererFacade::createVisualizeTangentPass()
{
	createVisualizePass(mRenderDeferredPasses->getTangentMap(), false, false);
}

void RendererFacade::createVisualizeNormalPass()
{
	createVisualizePass(mRenderDeferredPasses->getNormalMap(), false, false);
}

void RendererFacade::createVisualizeAmbientOcclusionPass()
{
	createVisualizePass(mAmbientOcclusionFramebuffer.getCombinedImage(0), true, true);
}

void RendererFacade::createVisualizeVoxelGridPass()
{
	mVisualizePass = std::make_unique<RenderFullScreenPass>(mDevice, vk::Format::eB8G8R8A8Unorm, vk::ImageLayout::eUndefined, vk::ImageLayout::eShaderReadOnlyOptimal);
	mVisualizePass->setFragmentStage("../Shaders/Voxelization/visualization.spv");
	mVisualizePass->addUniformBuffer(vk::DescriptorBufferInfo(mSceneGraphBuffers->getPerspectiveViewMatrixBuffer(), 0, VK_WHOLE_SIZE), 0);
	mVisualizePass->addUniformBuffer(vk::DescriptorBufferInfo(mVoxelizationPass->getCubeVoxelizationInfoBuffer(), 0, VK_WHOLE_SIZE), 1);
	mVisualizePass->addStorageImage(vk::DescriptorImageInfo(mVoxelizationPass->getVoxelGrid(), mVoxelizationPass->getVoxelGrid(), vk::ImageLayout::eGeneral), 2);
	mVisualizePass->create(mExtent);

	mVisualizePassCommandBuffer = std::move(mCommandPool.allocate(vk::CommandBufferLevel::ePrimary, 1)[0]);
	mVisualizePassCommandBuffer->begin(StructHelper::beginCommand());
	auto transitionVoxelGrid = MemoryBarrier::image(mVoxelizationPass->getVoxelGrid(), vk::ImageLayout::eTransferSrcOptimal, vk::ImageLayout::eGeneral, ImageSubresource::rangeColor(), vk::AccessFlagBits::eTransferRead, vk::AccessFlagBits::eShaderRead);
	MemoryBarrier::barrier(transitionVoxelGrid, mVisualizePassCommandBuffer, vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader);
	mVisualizePass->execute(mVisualizePassCommandBuffer, *mSwapchainFramebuffer);
	mVisualizePassCommandBuffer->end();
}

vk::CommandBuffer RendererFacade::choseTheGoodPresentationPass()
{
	auto type = mEngineParameters.presentationPassParameter.type;
	if (type == FINAL_RENDERING) {
		mCurrentType = type;
		mVisualizePass = nullptr;
		mVisualizePassCommandBuffer.reset(vk::CommandBuffer());
		return mFinalRenderingPassCommandBuffer;
	}

	else {
		if (mCurrentType != type) {
			if (type == ALBEDO)
				createVisualizeAlbedoPass();

			else if (type == TANGENT)
				createVisualizeTangentPass();

			else if (type == NORMAL)
				createVisualizeNormalPass();

			else if (type == AMBIENT_OCCLUSION)
				createVisualizeAmbientOcclusionPass();

			else if (type == VOXEL_GRID)
				createVisualizeVoxelGridPass();
			
			mCurrentType = type;
		}

		return (*mVisualizePassCommandBuffer != vk::CommandBuffer()) ? mVisualizePassCommandBuffer : mFinalRenderingPassCommandBuffer;
	}
}
