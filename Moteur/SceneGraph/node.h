#pragma once

#include <vector>
#include <functional>
#include <numeric>
#include <deque>
#include "MeshManager.h"
#include "Material/MaterialsManager.h"
#include "EntityManager.h"
#include "Tools/vector_view.h"

class Node {
public:
    // DrawCommand, instanceValidity, instanceMatrix, AABB, original AABB
    using InstancesOfOneObject = std::tuple<DrawCmd, std::deque<bool>, std::deque<glm::mat4>, std::deque<AABB>, AABB>;

    Node(MeshManager &meshManager, MaterialsManager &materialsManager);

    EntityManager addModel(std::string path);

    uint32_t getNumberInstances() const;
    uint32_t getNumberDrawCalls() const;

    AABB getAABB(glm::mat4 parent = glm::mat4{ 1.0f }) const;

    void computeMatrices(glm::mat4 parent, vector_view<glm::mat4> &matriceBufferView);
    void buildIndirectCommands(vector_view<vk::DrawIndexedIndirectCommand> &indirectCommandsBufferView, uint32_t &currentInstance);

    void draw(std::vector<Drawer> &drawers, bool enableMaterials);

    void setMatrix(glm::mat4 matrix = glm::mat4(1.0f));

private:
    MeshManager &mMeshManager;
    MaterialsManager &mMaterialsManager;

    glm::mat4 mNodeTransformation = glm::mat4(1.0f);

    std::vector<Node> mNodeChild;
    std::deque<InstancesOfOneObject> mInstancesByObject;
    std::vector<std::size_t> mModelsAlreadyLoad;

    template<typename Function, typename RecursiveFunction>
    uint32_t getNumber(const Function &f, const RecursiveFunction &recursiveFunction) const {
        auto result = std::accumulate(mInstancesByObject.begin(), mInstancesByObject.end(), 0u, f);

        for (auto &c : mNodeChild)
            result += (c.*recursiveFunction)();

        return result;
    }
};
