#include "Window/window.h"
#include "Tools/glm.h"
#include <thread>
#include <chrono>
#include <iostream>
#include "Vulkan/instance.h"
#include "Vulkan/device.h"
#include "Vulkan/swapchain.h"
#include "Transfer/bufferfactory.h"
#include "Transfer/imagefactory.h"
#include "SceneGraph/scenegraph.h"

#include "imgui/imguiinstance.h"
#include <memory>

#include "Vulkan/input.h"
#include "rendererfacade.h"
#include "interface.h"

#include "Physics/dynaobject.h"

#include <crtdbg.h>


#define CASSERT(expr) \
   do { \
	_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_WNDW);\
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_WNDW);\
	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_WNDW);\
      if (!(expr) && (1 == _CrtDbgReport( \
         _CRT_ASSERT, __FILE__, __LINE__, #expr, NULL))) \
         _CrtDbgBreak(); \
   } while (0)

#define NAME "Engine"
#define TIMESTEP 0.01f
typedef std::chrono::duration<float> time_s;


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
	Instance mInstance{ "", "", mWindow, true };
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

vk::Semaphore render(SuperWindow& window, Device& device, SceneGraph& sceneGraph, Camera& camera, Interface& interface, std::unique_ptr<RendererFacade> &rendererFacade) {
	if (window.isResized()) {
		rendererFacade = std::make_unique<RendererFacade>(device, sceneGraph, window.getBufferFactory(), window.getImageFactory(), window.getImGUIInstance(), window.getExtent());
		for (auto i(0u); i < window.getNumberImages(); ++i)
			window.getSwapchain().createFramebuffer(rendererFacade->getPresentationRenderPass());
	}

	auto nextImage{ window.getNextImage() };

	sceneGraph.setCamera(camera);
	rendererFacade->newFrame();

	interface.setVoxelizationProfiling(rendererFacade->getVoxelizationPassProfiling());
	interface.execute();
	interface.checkError();

	rendererFacade->setParameters(interface.getParameters());

	return rendererFacade->execute(window.getImageAvailableSemaphore(), window.getSwapchain().getFramebuffers()[window.getNextImage()]);
}

void updateCamera(Camera &camera) {
	if (Input::instance().keyPressed[GLFW_KEY_LEFT])
		camera.position += glm::vec3(0.0, 0.0, 1.0);

	if (Input::instance().keyPressed[GLFW_KEY_RIGHT])
		camera.position += glm::vec3(0.0, 0.0, -1.0);
	
	if (Input::instance().keyPressed[GLFW_KEY_UP])
		camera.position += glm::vec3(-1.0, 0.0, 0.0);
	
	if (Input::instance().keyPressed[GLFW_KEY_DOWN])
		camera.position += glm::vec3(1.0, 0.0, 0.0);
}

void computePhysicalStep(const float& timeSimulated, const float& period, DynaObject& object) {
	object.update(timeSimulated, period);
}

void computeRenderState(const float& timeLeft, const float& period, DynaObject& object) {
	float alpha = timeLeft / period;
	object.computeRenderState(alpha);
}

void run() {
	SuperWindow window(800, 600, NAME);
	decltype(auto) device = window.getDevice();

	CommandPool commandPoolSceneGraphPass(device, device.getQueueFamilyIndices().graphicFamily, false, false);
	SceneGraph sceneGraph(device);

	auto rootNode = sceneGraph.getRootNode();

	auto cubeManager = rootNode->addModel("../Models/cube.obj");
	auto sponzaManager = rootNode->addModel("../Models/Sponza/sponza.obj");

	auto sponza = sponzaManager.createEntity();
	sponza.scale(glm::vec3(1.f / 10.f));
	auto firstCube = cubeManager.createEntity();
	auto aabb = sceneGraph.getAABB();

	DynaObject d(&firstCube);

	d.scale(glm::vec3(10.f));
	float rotate = 0.0f;

	//FPSManager fpsManager(30);

	bool show_test_window = true;
	bool show_another_window = false;

	std::unique_ptr<RendererFacade> rendererFacade;

	Interface interface(NAME);

	Camera camera;

	camera.position = glm::vec3(50.f, 20.f, 0.f);
	camera.direction = glm::vec3(-1.0f, -0.0f, 0.0f);

	time_s timeSimulated(0.f);
	const time_s deltaTimeStep(TIMESTEP);
	time_s accumulator(0.f);
	auto currentTime = std::chrono::high_resolution_clock::now();

	while (window.isRunning()) {
		auto semaphoreWaitRenderingFinished = render(window, device, sceneGraph, camera, interface, rendererFacade);
		updateCamera(camera);
		auto newTime = std::chrono::high_resolution_clock::now();
		auto frameTime = newTime - currentTime;
		currentTime = newTime;
		accumulator += std::chrono::duration_cast<time_s>(frameTime);
		while (accumulator > deltaTimeStep) {

			computePhysicalStep(timeSimulated.count(), deltaTimeStep.count(), d);
			accumulator -= deltaTimeStep;
			timeSimulated += deltaTimeStep;
		}
		computeRenderState(accumulator.count(), deltaTimeStep.count(), d);
		//fpsManager.wait();
		window.present(semaphoreWaitRenderingFinished);
	}
	device->waitIdle();
}

int main() {
	run();

	getchar();

	return 0;
}