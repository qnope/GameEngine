#include "Window\window.h"
#include "Tools\glm.h"
#include <thread>
#include <chrono>
#include <iostream>
#include "Vulkan/instance.h"
#include "Vulkan/device.h"
#include "Vulkan\swapchain.h"
#include "Transfer/bufferfactory.h"
#include "Transfer/imagefactory.h"
#include "SceneGraph/scenegraph.h"

#include "imgui/imguiinstance.h"
#include <memory>

#include "Vulkan/input.h"
#include "rendererfacade.h"
#include "interface.h"

#define NAME "Engine"

class SuperWindow {
public:
	SuperWindow(int w, int h, std::string name) :
		mWindow(w, h, name) {
		Input::instance(mWindow);
	}

	bool isRunning() const {
		return mWindow.isRunning();
	}

	Device &getDevice() { return mDevice; }

	uint32_t getNumberImages() const { return mSwapchain->getImageCount(); }

	uint32_t getNextImage() {
		if (mAlreadyPresented == true) {
			mCurrentSemaphoreIndex = mNextImage;
			mNextImage = mDevice->acquireNextImageKHR(*mSwapchain, std::numeric_limits<uint64_t>::max(), mImageAvailableSemaphores[mCurrentSemaphoreIndex], vk::Fence()).value;
		}

		mAlreadyPresented = false;
		return mNextImage;
	}

	int getWidth() const { return mWindow.getWidth(); }
	int getHeight() const { return mWindow.getHeight(); }
	vk::Extent2D getExtent() const { return vk::Extent2D{ (uint32_t)getWidth(), (uint32_t)getHeight() }; }

	bool isResized() {
		static int w{ 0 }, h{ 0 };

		if (w != getWidth() || h != getHeight()) {
			w = getWidth();
			h = getHeight();
			mDevice->waitIdle();
			mSwapchain = std::make_unique<Swapchain>(mDevice, mSwapchain.get());
			mImageAvailableSemaphores.resize(mSwapchain->getImageCount());
			for (auto &semaphore : mImageAvailableSemaphores)
				semaphore = mDevice->createSemaphoreUnique(vk::SemaphoreCreateInfo());
			return true;
		}
		return false;
	}

	vk::Format getFormat() const { return mSwapchain->getFormat(); }

	Swapchain &getSwapchain() { return *mSwapchain; }
	const Framebuffer &getCurrentFramebuffer() const { return mSwapchain->getFramebuffers()[mNextImage]; }
	vk::Framebuffer getFramebuffer(uint32_t index) const { return mSwapchain->getFramebuffers()[index]; }

	vk::Semaphore getImageAvailableSemaphore() { return mImageAvailableSemaphores[mCurrentSemaphoreIndex]; }

	void present(vk::Semaphore semaphoreToWait) {
		assert(mAlreadyPresented == false);
		mCommandBufferSubmitter.addWaitSemaphore(semaphoreToWait);
		mCommandBufferSubmitter.present(*mSwapchain, mNextImage, mDevice.getPresentQueue());
		mAlreadyPresented = true;
	}

	BufferFactory &getBufferFactory() { return mBufferFactory; }
	ImageFactory &getImageFactory() { return mImageFactory; }
	ImGUIInstance &getImGUIInstance() { return mImGUIInstance; }

private:
	Window mWindow;
	Instance mInstance{ "", "", mWindow, false };
	Device mDevice{ mInstance };
	std::unique_ptr<Swapchain> mSwapchain;
	std::vector<vk::UniqueSemaphore> mImageAvailableSemaphores;
	CommandBufferSubmitter mCommandBufferSubmitter{};
	uint32_t mNextImage{ 0 };
	uint32_t mCurrentSemaphoreIndex{ 0 };
	bool mAlreadyPresented{ true };
	BufferFactory mBufferFactory{ mDevice };
	ImageFactory mImageFactory{ mDevice };
	ImGUIInstance mImGUIInstance{ mDevice, mWindow, mImageFactory };
};

class FPSManager {
public:
	FPSManager(uint32_t fps) : mFrameDurationInMilisecond(1000 / fps) {}

	void wait() {
		static auto lastTime = std::chrono::high_resolution_clock::now();
		auto currentTime = std::chrono::high_resolution_clock::now();
		auto endTime = lastTime + mFrameDurationInMilisecond;
		auto waitTime = endTime - currentTime;

		if (waitTime.count() > 0)
			std::this_thread::sleep_for(waitTime);

		lastTime = std::chrono::high_resolution_clock::now();
	}

private:
	const std::chrono::milliseconds mFrameDurationInMilisecond;
};

void run() {
	SuperWindow window(800, 600, NAME);
	decltype(auto) device = window.getDevice();

	CommandPool commandPoolSceneGraphPass(device, device.getQueueFamilyIndices().graphicFamily, false, false);
	SceneGraph sceneGraph(device);

	auto rootNode = sceneGraph.getRootNode();

	//auto cubeManager = rootNode->addModel("../Models/cube.obj");
	auto sponzaManager = rootNode->addModel("../Models/Sponza/sponza.obj");

	auto sponza = sponzaManager.createEntity();
	sponza.scale(glm::vec3(1.0 / 10.0));
	//auto firstCube = cubeManager.createEntity();

	auto aabb = sceneGraph.getAABB();

	float rotate = 0.0f;

	FPSManager fpsManager(30);

	bool show_test_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImColor(114, 144, 154);

	std::vector<RendererFacade> rendererFacades{};

	Interface interface(NAME);

	Camera camera;

	camera.position = glm::vec3(-40, 25, 0.0);
	camera.direction = glm::vec3(-1.0f, -0.0f, 0.0f);

	while (window.isRunning()) {
		if (window.isResized()) {
			rendererFacades.clear();
			for (auto i(0u); i < window.getNumberImages(); ++i)
				rendererFacades.emplace_back(device, sceneGraph, window.getBufferFactory(), window.getImageFactory(), window.getImGUIInstance(), window.getSwapchain(), window.getExtent());
		}

	/*	firstCube.identity();
		firstCube.translate(0, 500, 0);
		firstCube.rotate(glm::vec3(0, 1, 0), rotate);
		firstCube.scale(glm::vec3(200));*/

		//rotate += 360.0f / (2* 300);
		if (rotate > 360.0f)
			rotate -= 360.0f;

		auto nextImage{ window.getNextImage() };

		sceneGraph.setCamera(camera);
		rendererFacades[nextImage].newFrame();

		interface.setVoxelizationProfiling(rendererFacades[nextImage].getVoxelizationPassProfiling());
		interface.execute();
		interface.checkError();
		
		rendererFacades[nextImage].setParameters(interface.getParameters());
		
		auto semaphoreWaitRenderingFinished = rendererFacades[nextImage].execute(window.getImageAvailableSemaphore());

		fpsManager.wait();

		window.present(semaphoreWaitRenderingFinished);
	}
	device->waitIdle();
}

int main() {
	run();

	getchar();

	return 0;
}