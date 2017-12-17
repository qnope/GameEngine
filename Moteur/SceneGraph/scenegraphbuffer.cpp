#include "scenegraphbuffer.h"

SceneGraphBuffer::SceneGraphBuffer(Device &device, SceneGraph & sceneGraph, BufferFactory & bufferFactory, uint32_t numberObjects, uint32_t numberCommands) :
    mSceneGraph(sceneGraph),
    mTransientCommandPool(device, device.getQueueFamilyIndices().transferFamily, true, true),
    mPerspectiveViewMatrixStagingBuffer(bufferFactory.createEmptyBuffer(sizeof(CameraInformation), vk::BufferUsageFlagBits::eTransferSrc, false)),
    mPerspectiveViewMatrixBuffer(bufferFactory.createEmptyBuffer(sizeof(CameraInformation), vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eUniformBuffer, true)),
    mModelMatricesStatingBuffer(bufferFactory.createEmptyBuffer(sizeof(glm::mat4) * numberObjects, vk::BufferUsageFlagBits::eTransferSrc, false)),
    mModelMatricesBuffer(bufferFactory.createEmptyBuffer(sizeof(glm::mat4) * numberObjects, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer, true)),
    mModelMatricesStagingView((glm::mat4*)mModelMatricesStatingBuffer.getPtr(), numberObjects),
    mIndirectStagingBuffer(bufferFactory.createEmptyBuffer(sizeof(vk::DrawIndexedIndirectCommand) * numberCommands, vk::BufferUsageFlagBits::eTransferSrc, false)),
    mIndirectBuffer(bufferFactory.createEmptyBuffer(sizeof(vk::DrawIndexedIndirectCommand) * numberCommands, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndirectBuffer, true)),
    mIndirectStatingBufferView((vk::DrawIndexedIndirectCommand*) mIndirectStagingBuffer.getPtr(), numberCommands)
{
}

vk::CommandBuffer SceneGraphBuffer::execute()
{
    buildMatrices();
    buildIndirectCommands();

    mCommandBuffer->begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit, nullptr));

    CameraInformation ci;

    auto camera = mSceneGraph.getCamera();
    mPerspectiveViewMatrixStagingBuffer.resetOffset();
    ci.projection = camera.getPerspectiveMatrix();
    ci.view = camera.getViewMatrix();
    mPerspectiveViewMatrixStagingBuffer.push_data(ci);

    auto numberInstances = mSceneGraph.getNumberInstances();
    auto numberDrawCalls = mSceneGraph.getNumberDrawCalls();

    auto instancesSize = numberInstances * sizeof(glm::mat4);
    auto drawCallsSize = numberDrawCalls * sizeof(vk::DrawIndexedIndirectCommand);

    StructHelper::transfer(mPerspectiveViewMatrixStagingBuffer, mPerspectiveViewMatrixBuffer, mCommandBuffer, 0, 0, sizeof(CameraInformation));
    StructHelper::transfer(mModelMatricesStatingBuffer, mModelMatricesBuffer, mCommandBuffer, 0, 0, instancesSize);
    StructHelper::transfer(mIndirectStagingBuffer, mIndirectBuffer, mCommandBuffer, 0, 0, drawCallsSize);

    auto barrier = MemoryBarrier::global(vk::AccessFlagBits::eTransferWrite, vk::AccessFlagBits::eUniformRead | vk::AccessFlagBits::eIndirectCommandRead | vk::AccessFlagBits::eVertexAttributeRead);

    MemoryBarrier::barrier(barrier, mCommandBuffer, vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eVertexInput | vk::PipelineStageFlagBits::eVertexShader | vk::PipelineStageFlagBits::eDrawIndirect);

    mCommandBuffer->end();

    return mCommandBuffer;
}

void SceneGraphBuffer::buildMatrices()
{
    mModelMatricesStagingView.clear();
    mSceneGraph.computeMatrices(mModelMatricesStagingView);
}

void SceneGraphBuffer::buildIndirectCommands()
{
    mIndirectStatingBufferView.clear();
    mSceneGraph.buildIndirectCommands(mIndirectStatingBufferView);
}

const Buffer & SceneGraphBuffer::getPerspectiveViewMatrixBuffer() const
{
    return mPerspectiveViewMatrixBuffer;
}

const Buffer & SceneGraphBuffer::getModelMatricesBuffer() const
{
    return mModelMatricesBuffer;
}

const Buffer & SceneGraphBuffer::getIndirectBuffer() const
{
    return mIndirectBuffer;
}
