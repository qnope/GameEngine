#pragma once

#include "MeshManager.h"
#include "Material/MaterialsManager.h"
#include "Transfer/BufferFactory.h"
#include "Transfer/ImageFactory.h"
#include "Node.h"
#include "Tools/vector_view.h"

#include "Camera.h"

class SceneGraph {
public:
    SceneGraph(Device &device);

    std::shared_ptr<Node> getRootNode();

    uint32_t getNumberInstances() const;
    uint32_t getNumberDrawCalls() const;

    void computeMatrices(vector_view<glm::mat4> &matriceBufferView);
    void buildIndirectCommands(vector_view<vk::DrawIndexedIndirectCommand> &indirectCommandBufferView);

    void prepareWithoutMaterials(vk::Pipeline pipeline, vk::PipelineLayout pipelineLayout);
    void prepareWithMaterials(const std::vector<vk::Pipeline> &pipelines, const std::vector<vk::PipelineLayout> &pipelineLayouts);

    void compile(vk::CommandBuffer cmd, uint32_t materialSetNumber, vk::Buffer indirectBuffer);

    const MaterialsManager &getMaterialsManager() const;

    AABB getAABB() const;

    void setCamera(Camera camera);
    Camera getCamera() const;

private:
    ImageFactory mImageFactory;
    BufferFactory mBufferFactory;
    MeshManager mMeshManager;
    MaterialsManager mMaterialsManager;

    std::shared_ptr<Node> mRootNode;

    std::vector<Drawer> mDrawers;

    vk::Pipeline mNonMaterialPipeline;
    vk::PipelineLayout mNonMaterialPipelineLayout;
    bool mEnableMaterials{ false };

    Camera mCamera;
};
