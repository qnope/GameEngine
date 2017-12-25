#pragma once

#include "SceneGraph.h"

struct CameraInformation {
    glm::mat4 projection;
    glm::mat4 view;
    glm::vec4 positionCamera;
};

class SceneGraphBuffer {
public:
    SceneGraphBuffer(Device &device, SceneGraph &sceneGraph, BufferFactory &bufferFactory, uint32_t numberObjects = 10'000, uint32_t numberCommands = 100'000);

    vk::CommandBuffer execute();

    void buildMatrices();
    void buildIndirectCommands();

    const Buffer &getPerspectiveViewMatrixBuffer() const;
    const Buffer &getModelMatricesBuffer() const;
    const Buffer &getIndirectBuffer() const;

private:
    SceneGraph &mSceneGraph;

    CommandPool mTransientCommandPool;
    vk::UniqueCommandBuffer mCommandBuffer{ std::move(mTransientCommandPool.allocate(vk::CommandBufferLevel::ePrimary, 1)[0]) };

    Buffer mPerspectiveViewMatrixStagingBuffer;
    Buffer mPerspectiveViewMatrixBuffer;

    Buffer mModelMatricesStatingBuffer;
    Buffer mModelMatricesBuffer;
    vector_view<glm::mat4> mModelMatricesStagingView;

    Buffer mIndirectStagingBuffer;
    Buffer mIndirectBuffer;
    vector_view<vk::DrawIndexedIndirectCommand> mIndirectStatingBufferView;
};
