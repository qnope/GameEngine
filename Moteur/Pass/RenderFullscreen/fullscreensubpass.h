#pragma once

#include "Pass/Subpass.h"
#include "Pass/PipelineBuilder.h"
#include "Vulkan/DescriptorPool.h"
#include "Descriptor/DescriptorSetLayoutBuilder.h"
#include "Pass/PipelineLayoutBuilder.h"

class FullscreenSubpass final : public Subpass {
public:
    FullscreenSubpass(vk::Device device);

    void setFragmentStage(std::string filename);
    void addUniformBuffer(vk::DescriptorBufferInfo info, uint32_t binding);
    void addSampler(vk::DescriptorImageInfo info, uint32_t binding);
    void addStorageImage(vk::DescriptorImageInfo info, uint32_t binding);

    void addPushConstant(vk::PushConstantRange range, const void *data);

private:
    void create(vk::RenderPass renderPass, vk::Extent2D extent, uint32_t indexsubpass) override;

    void execute(vk::CommandBuffer cmd);

    vk::Device mDevice;

    DescriptorPool mDescriptorPool;
    vk::DescriptorSet mDescriptorSet;
    DescriptorSetLayout mDescriptorSetLayout;

    std::string mFragmentStage;

    std::vector<std::tuple<vk::DescriptorBufferInfo, uint32_t, vk::DescriptorType>> mBufferInfos;
    std::vector<std::tuple<vk::DescriptorImageInfo, uint32_t, vk::DescriptorType>> mImageInfos;

    std::vector<vk::PushConstantRange> mPushConstantRanges;
    std::vector<unsigned char> mPushConstantData;

    PipelineLayout mPipelineLayout;
    Pipeline mPipeline;
};
