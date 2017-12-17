#include "pipelinelayout.h"
#include "../Tools/stream.h"

PipelineLayout::PipelineLayout(vk::Device device) : mDevice(device)
{
}

void PipelineLayout::addDescriptorSetLayout(vk::DescriptorSetLayout layout)
{
    mDescriptorSetLayouts << layout;
}

void PipelineLayout::addPushConstantRange(vk::PushConstantRange range)
{
    mPushConstantRanges << range;
}

void PipelineLayout::create()
{
    vk::PipelineLayoutCreateInfo info;

    info.setLayoutCount = (uint32_t)mDescriptorSetLayouts.size();
    info.pSetLayouts = mDescriptorSetLayouts.data();
    info.pushConstantRangeCount = (uint32_t)mPushConstantRanges.size();
    info.pPushConstantRanges = mPushConstantRanges.data();

    static_cast<vk::UniquePipelineLayout&>(*this) = mDevice.createPipelineLayoutUnique(info);
}

std::vector<vk::DescriptorSetLayout> const & PipelineLayout::getDescriptorSetLayouts() const
{
    return mDescriptorSetLayouts;
}
