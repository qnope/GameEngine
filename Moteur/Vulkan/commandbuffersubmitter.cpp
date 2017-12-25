#include "CommandBufferSubmitter.h"
#include "Tools/stream.h"

void CommandBufferSubmitter::addCommandBuffer(vk::CommandBuffer cmd)
{
    mCommandBuffers << cmd;
}

void CommandBufferSubmitter::addCommandBuffers(const std::vector<vk::UniqueCommandBuffer>& cmds)
{
    mCommandBuffers << cmds;
}

void CommandBufferSubmitter::addWaitSemaphore(vk::Semaphore semaphore)
{
    mWaitSemaphores << semaphore;
}

void CommandBufferSubmitter::addWaitSemaphores(const std::vector<vk::UniqueSemaphore>& semaphores)
{
    mWaitSemaphores << semaphores;
}

void CommandBufferSubmitter::addSignalSemaphore(vk::Semaphore semaphore)
{
    mSignalSemaphores << semaphore;
}

void CommandBufferSubmitter::addSignalSemaphores(const std::vector<vk::UniqueSemaphore>& semaphores)
{
    mSignalSemaphores << semaphores;
}

void CommandBufferSubmitter::addWaitingStage(vk::PipelineStageFlags pipelineStage)
{
    mWaitStages << pipelineStage;
}

void CommandBufferSubmitter::addWaitingStages(const std::vector<vk::PipelineStageFlags>& pipelineStages)
{
    mWaitStages << pipelineStages;
}

void CommandBufferSubmitter::submit(vk::Queue queue, vk::Fence fence)
{
    assert(!mCommandBuffers.empty());
    assert(mWaitSemaphores.size() == mWaitStages.size());

    vk::SubmitInfo info;
    info.commandBufferCount = (uint32_t)mCommandBuffers.size();
    info.pCommandBuffers = mCommandBuffers.data();

    info.signalSemaphoreCount = (uint32_t)mSignalSemaphores.size();
    info.pSignalSemaphores = mSignalSemaphores.data();

    info.pWaitDstStageMask = mWaitStages.data();
    info.waitSemaphoreCount = (uint32_t)mWaitSemaphores.size();
    info.pWaitSemaphores = mWaitSemaphores.data();

    queue.submit(info, fence);

    mCommandBuffers.clear();
    mSignalSemaphores.clear();
    mWaitSemaphores.clear();
    mWaitStages.clear();
}

void CommandBufferSubmitter::present(vk::SwapchainKHR swapchain, uint32_t imageIndex, vk::Queue presentQueue)
{
    vk::PresentInfoKHR presentInfo;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pSwapchains = &swapchain;
    presentInfo.swapchainCount = 1;

    presentInfo.pWaitSemaphores = mWaitSemaphores.data();
    presentInfo.waitSemaphoreCount = (uint32_t)mWaitSemaphores.size();

    presentQueue.presentKHR(presentInfo);

    mCommandBuffers.clear();
    mSignalSemaphores.clear();
    mWaitSemaphores.clear();
    mWaitStages.clear();
}
