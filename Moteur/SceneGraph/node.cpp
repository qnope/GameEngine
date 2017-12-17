#include <functional>
#include "node.h"
#include "ModelImporter/modelimporter.h"

Node::Node(MeshManager &meshManager, MaterialsManager &materialsManager) :
    mMeshManager(meshManager),
    mMaterialsManager(materialsManager) {

}

EntityManager Node::addModel(std::string path) {
    auto hash = std::hash<std::string>{}(path);
    auto it = std::find(mModelsAlreadyLoad.begin(), mModelsAlreadyLoad.end(), hash);
    // Model already loaded
    if (it != mModelsAlreadyLoad.end()) {
        auto index = std::distance(mModelsAlreadyLoad.begin(), it);
        return EntityManager{ &std::get<1>(mInstancesByObject[index]), &std::get<2>(mInstancesByObject[index]), &std::get<3>(mInstancesByObject[index]), std::get<4>(mInstancesByObject[index]) };
    }

    ModelImporter importer(path);
    auto cmd = mMeshManager.addMesh(importer.mMeshes);
    auto materialIndice = mMaterialsManager.addMaterials(importer.mMaterials);
    AABB aabb;
    for (int i = 0; i < importer.mMeshes.size(); ++i) {
        cmd.mParts[i].materialPointer = materialIndice[importer.mMeshes[i].materialIndex];
        aabb << importer.mMeshes[i].aabb;
    }
    mInstancesByObject << std::make_tuple(cmd, std::deque<bool>{}, std::deque<glm::mat4>{}, std::deque<AABB>{}, aabb);
    mModelsAlreadyLoad.push_back(hash);

    return EntityManager{ &std::get<1>(mInstancesByObject.back()), &std::get<2>(mInstancesByObject.back()), &std::get<3>(mInstancesByObject.back()), std::get<4>(mInstancesByObject.back()) };
}

uint32_t Node::getNumberInstances() const
{
    return getNumber([](auto init, auto instancesOfOneObject) {
        const auto &instanceValidities = std::get<1>(instancesOfOneObject);
        return init + (uint32_t)std::count(instanceValidities.begin(), instanceValidities.end(), true);
    }, &Node::getNumberInstances);
}

uint32_t Node::getNumberDrawCalls() const
{
    return getNumber([](auto init, auto instancesOfOneObject) {
        return init + (uint32_t)std::get<0>(instancesOfOneObject).mParts.size();
    }, &Node::getNumberDrawCalls);
}

AABB Node::getAABB(glm::mat4 parent) const
{
    auto m = parent * mNodeTransformation;
    AABB res;

    for (auto &instances : mInstancesByObject)
        for (auto &aabb : std::get<3>(instances))
            res << aabb * m;

    for (auto &child : mNodeChild)
        res << child.getAABB(m);
    return res;
}

void Node::computeMatrices(glm::mat4 parent, vector_view<glm::mat4> &matriceBufferView)
{
    auto m = parent * mNodeTransformation;

    for (const auto &instance : mInstancesByObject) {
        const auto &[cmd, validities, matrices, aabbs, aabb] = instance;
        auto itValidity = validities.begin();
        auto itMatrix = matrices.begin();
        auto endMatrix = matrices.end();

        for (; itMatrix != endMatrix; ++itMatrix, ++itValidity)
            if (*itValidity)
                matriceBufferView.push_back(m * *itMatrix);
        (void)cmd; (void)aabb; (void)aabbs;
    }

    for (auto &c : mNodeChild)
        c.computeMatrices(m, matriceBufferView);
}

void Node::buildIndirectCommands(vector_view<vk::DrawIndexedIndirectCommand>& indirectCommandsBufferView, uint32_t &currentInstance)
{
    int i = 0;
    for (const auto &instance : mInstancesByObject) {
        const auto &[cmd, validities, matrices, aabbs, aabb] = instance;
        for (const auto &part : cmd.mParts) {
            vk::DrawIndexedIndirectCommand indirectCommand;
            indirectCommand.firstIndex = part.firstIndex;
            indirectCommand.indexCount = part.indexCount;
            indirectCommand.vertexOffset = part.vertexOffset;

            indirectCommand.instanceCount = (uint32_t)validities.size();
            indirectCommand.firstInstance = currentInstance;

            indirectCommandsBufferView.push_back(indirectCommand);
        }
        currentInstance += (uint32_t)validities.size();
        (void)matrices; (void)aabb; (void)aabbs;
    }

    for (auto &c : mNodeChild)
        c.buildIndirectCommands(indirectCommandsBufferView, currentInstance);
}

void Node::draw(std::vector<Drawer> &drawers, bool enableMaterials)
{
    for (const auto &instance : mInstancesByObject) {
        const auto &[cmd, validities, matrices, aabbs, aabb] = instance;
        for (const auto &part : cmd.mParts) {
            Drawer drawer;

            if (enableMaterials)
                mMaterialsManager.getDrawerMaterialValues(drawer, part.materialPointer);

            drawer.vbo = part.vbo;
            drawer.ibo = part.ibo;

            drawers << drawer;
        }
        (void)validities; (void)matrices; (void)aabb; (void)aabbs;
    }

    for (auto &c : mNodeChild)
        c.draw(drawers, enableMaterials);
}

void Node::setMatrix(glm::mat4 matrix)
{
    mNodeTransformation = matrix;
}
