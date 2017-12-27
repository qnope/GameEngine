#include "AbstractUniqueMaterialManager.h"
#include "Tools/string_tools.h"

AbstractUniqueMaterialManager::AbstractUniqueMaterialManager(vk::Device device, std::string pathToMaterialFunctionDefinition) :
    mDevice(device)
{
    mMaterialFunctionDefinition = readFile(pathToMaterialFunctionDefinition);
}

std::string AbstractUniqueMaterialManager::getFunctionDefinition() const {
    return mMaterialFunctionDefinition;
}

void AbstractUniqueMaterialManager::changePipeline(vk::Pipeline pipeline)
{
    mPipeline = pipeline;
}

void AbstractUniqueMaterialManager::changePipelineLayout(vk::PipelineLayout pipelineLayout)
{
    mPipelineLayout = pipelineLayout;
}

vk::DescriptorSetLayout AbstractUniqueMaterialManager::getDescriptorSetLayout() const
{
    return *mDescriptorSetLayout;
}

void AbstractUniqueMaterialManager::getDrawerMaterialValues(Drawer & drawer, const MaterialPointer & ptr) const
{
    assert(ptr.ptr == this);
    drawer.pipeline = mPipeline;
    drawer.pipelineLayout = mPipelineLayout;
}
