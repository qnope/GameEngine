#include "MaterialsManager.h"

void MaterialsManager::addKindMaterialManager(std::unique_ptr<AbstractUniqueMaterialManager>&& ptr)
{
    mMaterialManagers.emplace_back(std::move(ptr));
}

std::vector<AbstractUniqueMaterialManager::MaterialPointer> MaterialsManager::addMaterials(std::vector<Material> materials)
{
    std::vector<AbstractUniqueMaterialManager::MaterialPointer> indices;

    for (auto m : materials)
        for (auto &p : mMaterialManagers)
            if (p->isAccepted(m))
                indices.emplace_back(p->addMaterial(m));

    return indices;
}

std::vector<vk::DescriptorSetLayout> MaterialsManager::getAllDescriptorSetLayouts() const
{
    std::vector<vk::DescriptorSetLayout> layouts;

    layouts.reserve(mMaterialManagers.size());

    for (auto &p : mMaterialManagers)
        layouts.emplace_back(p->getDescriptorSetLayout());

    return layouts;
}

std::vector<std::string> MaterialsManager::getAllFunctionDescriptions() const {
    std::vector<std::string> functionDefinitions;

    functionDefinitions.reserve(mMaterialManagers.size());

    for(auto &p : mMaterialManagers)
        functionDefinitions.emplace_back(p->getFunctionDefinition());

    return functionDefinitions;
}

void MaterialsManager::getDrawerMaterialValues(Drawer & drawer, const AbstractUniqueMaterialManager::MaterialPointer & ptr) const
{
    ptr.ptr->getDrawerMaterialValues(drawer, ptr);
}

void MaterialsManager::fillPipelines(const std::vector<vk::Pipeline>& pipelines)
{
    assert(pipelines.size() == mMaterialManagers.size());
    for (auto i(0u); i < pipelines.size(); ++i)
        mMaterialManagers[i]->changePipeline(pipelines[i]);
}

void MaterialsManager::fillPipelineLayouts(const std::vector<vk::PipelineLayout>& pipelineLayouts)
{
    assert(pipelineLayouts.size() == mMaterialManagers.size());
    for (auto i(0u); i < pipelineLayouts.size(); ++i)
        mMaterialManagers[i]->changePipelineLayout(pipelineLayouts[i]);
}
