#pragma once

#include <memory>
#include "SceneGraph/scenegraph.h"
#include "Pass/RenderSceneGraph/renderdeferredpasses.h"
#include "Pass/ImGui/imguipass.h"
#include "Pass/Voxelization/voxelizationpass.h"
#include "Pass/Voxelization/vxaopass.h"
#include "SceneGraph/scenegraphbuffer.h"
#include "Vulkan/commandbuffersubmitter.h"
#include "Vulkan/swapchain.h"

struct VoxelizationPassesParameter {
	bool enable{ true };
	int clipMapNumber{ 3 };
	int resolutionInPowerOfTwo{ 7 };
};

struct AmbientOcclusionParameter {
	bool enable() const { return ssao || vxao; }
	bool ssao{ false };
	bool vxao{ true };
	int downSampling{ 1 }; // 1 no downSampling, 2 : res / 2 etc
	VXAOParameter vxaoParameter;
};

enum { ALBEDO, TANGENT, NORMAL, VOXEL_GRID, PURE_DEBUG, AMBIENT_OCCLUSION, FINAL_RENDERING, N_DEBUG_PASS };

struct PresentationPassParameter {
	int type = AMBIENT_OCCLUSION;
};

struct EngineParameter {
	VoxelizationPassesParameter voxelizationPassParameter;
	AmbientOcclusionParameter ambientOcclusionParameter;
	PresentationPassParameter presentationPassParameter;
};

class RendererFacade {
public:
	RendererFacade(Device &device, SceneGraph &sceneGraph, BufferFactory &bufferFactory, ImageFactory &imageFactory, ImGUIInstance &imguiInstance, Swapchain &swapchain, vk::Extent2D extent);

	void setParameters(EngineParameter parameter);

	void newFrame();

	// return presentationRenderPass
	vk::Semaphore execute(vk::Semaphore waitRenderPresentableImageSemaphore);

	const std::vector<Profiling> *getVoxelizationPassProfiling() const;

private:
	Device &mDevice;
	SceneGraph &mSceneGraph;
	BufferFactory &mBufferFactory;
	ImageFactory &mImageFactory;
	ImGUIInstance &mImGUIInstance;

	CommandBufferSubmitter mSubmitter{};
	CommandPool mCommandPool{ mDevice, 0, false, false };
	vk::UniqueSemaphore mImagePresentable{ mDevice->createSemaphoreUnique(vk::SemaphoreCreateInfo()) };

	Fence mFencePresentableImage{ mDevice, true };

	vk::Extent2D mExtent;
	const Framebuffer *mSwapchainFramebuffer;

	Framebuffer mAmbientOcclusionFramebuffer;

	EngineParameter mEngineParameters;

	std::unique_ptr<SceneGraphBuffer> mSceneGraphBuffers;
	std::unique_ptr<RenderDeferredPasses> mRenderDeferredPasses;
	std::unique_ptr<RenderFullScreenPass> mFinalRenderingPass;
	std::unique_ptr<ImGUIPass> mImguiPass;

	std::vector<Profiling> mVoxelizationPassProfiling;
	bool mVoxelizationPassAlreadySubmitted{ false };
	std::unique_ptr<VoxelizationPass> mVoxelizationPass;
	std::unique_ptr<VXAOPass> mVxaoPass;

	vk::UniqueCommandBuffer mRenderDeferredPassesCommandBuffer;
	vk::UniqueCommandBuffer mFinalRenderingPassCommandBuffer;
	
	vk::UniqueCommandBuffer mVoxelizationPassCommandBuffer;
	vk::UniqueCommandBuffer mVxaoPassCommandBuffer;

	// These passes are for debugguing
	std::unique_ptr<RenderFullScreenPass> mVisualizePass;
	vk::UniqueCommandBuffer mVisualizePassCommandBuffer;
	int mCurrentType{ -1 };

	void createSceneGraphBuffers();
	void createDeferredPasses();
	void createFinalRenderingPass(Swapchain &swapchain);

	void createVoxelizationPass();
	void createVXAOPass();
	bool mVoxelGridHasChanged{ false };

	void createImGUIPass();

	void createVisualizePass(const CombinedImage &combinedImage, bool isMonochromatic, bool needGammaCorrection);
	void createVisualizeAlbedoPass();
	void createVisualizeTangentPass();
	void createVisualizeNormalPass();
	void createVisualizeAmbientOcclusionPass();

	void createVisualizeVoxelGridPass();

	vk::CommandBuffer choseTheGoodPresentationPass();
};
