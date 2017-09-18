#pragma once

#include "device.h"

class CommandBufferSubmitter {
public:
	void addCommandBuffer(vk::CommandBuffer cmd);
	void addCommandBuffers(const std::vector<vk::UniqueCommandBuffer> &cmds);
	void addWaitSemaphore(vk::Semaphore semaphore);
	void addWaitSemaphores(const std::vector<vk::UniqueSemaphore> &semaphores);
	void addSignalSemaphore(vk::Semaphore semaphore);
	void addSignalSemaphores(const std::vector<vk::UniqueSemaphore> &semaphores);
	void addWaitingStage(vk::PipelineStageFlags pipelineStage);
	void addWaitingStages(const std::vector<vk::PipelineStageFlags> &pipelineStages);

	void submit(vk::Queue queue, vk::Fence fence = vk::Fence());

	void present(vk::SwapchainKHR swapchain, uint32_t imageIndex, vk::Queue presentQueue);

private:
	std::vector<vk::CommandBuffer> mCommandBuffers;
	std::vector<vk::Semaphore> mWaitSemaphores;
	std::vector<vk::Semaphore> mSignalSemaphores;
	std::vector<vk::PipelineStageFlags> mWaitStages;
};
