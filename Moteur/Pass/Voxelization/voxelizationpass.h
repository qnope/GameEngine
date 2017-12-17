#pragma once

#include "../../Descriptor/descriptorpoolbuilder.h"
#include "../renderpassbuilder.h"
#include "../framebufferbuilder.h"
#include "../../Descriptor/descriptorsetlayoutbuilder.h"
#include "../pipelinelayoutbuilder.h"
#include "../../Transfer/imagefactory.h"
#include "../../Vulkan/querypool.h"

#define MAX_CLIPMAP 10

struct CubeVoxelizationInfo {
    uint32_t clipMapNumber;
    uint32_t voxelGridResolution;
    uint32_t p1, p2;
    glm::vec4 cubeCenterDiagonal[MAX_CLIPMAP]; // xyz center, w diagonal length
};

class VoxelizationPass {
public:
    VoxelizationPass(Device &device, uint32_t resolution, uint32_t nClipMap, SceneGraph &sceneGraph, const Buffer &modelMatricesBuffer, const Buffer &indirectBuffer, ImageFactory &imageFactory, BufferFactory &bufferFactory);

    void prepare();
    void execute(vk::CommandBuffer cmd);

    std::vector<Profiling> getProfiling();

    const CombinedImage &getVoxelGrid() const;
    const Buffer &getCubeVoxelizationInfoBuffer() const;

    uint32_t getVoxelGridResolution() const;
    uint32_t getNumberClipMap() const;

private:
    vk::Device mDevice;
    SceneGraph &mSceneGraph;

    QueryPoolTimeStamp mQueryPoolTimeStamp;

    DescriptorPoolWrapper mDescriptorPool{ DescriptorPoolBuilder::monoUniformBufferMonoStorageImage(mDevice, 1) };
    DescriptorSetLayout mDescriptorSetLayout{ DescriptorSetLayoutBuilder::monoUniformBufferMonoStorageImage(mDevice, vk::ShaderStageFlagBits::eGeometry | vk::ShaderStageFlagBits::eFragment, vk::ShaderStageFlagBits::eFragment) };
    vk::DescriptorSet mCubeVoxelizationInfoDescriptorSet{ mDescriptorPool.allocate(mDescriptorSetLayout) };
    PipelineLayout mPipelineLayout{ PipelineLayoutBuilder::build(mDevice, {mDescriptorSetLayout}) };

    Buffer mCubeVoxelizationInfoStagingBuffer;
    Buffer mCubeVoxelizationInfoBuffer;

    uint32_t mResolution;
    uint32_t mClipMapNumber;

    RenderPass mRenderPass;
    Framebuffer mFramebuffer;

    std::unique_ptr<CombinedImage> mVoxelGrid;

    DescriptorPoolWrapper mDownsamplerDescriptorPool{ DescriptorPoolBuilder::voxelDownsampler(mDevice) };
    DescriptorSetLayout mDownsamplerDescriptorSetLayout{ DescriptorSetLayoutBuilder::voxelDownsampler(mDevice) };
    vk::DescriptorSet mDownsamplerDescriptorSet{ mDownsamplerDescriptorPool.allocate(mDownsamplerDescriptorSetLayout) };
    PipelineLayout mDownsamplerPipelineLayout{ PipelineLayoutBuilder::build(mDevice, { mDownsamplerDescriptorSetLayout }, {vk::PushConstantRange(vk::ShaderStageFlagBits::eCompute, 0, 4)}) };
    Pipeline mDownsamplerPipeline{ PipelineBuilder::buildVoxelDownsamplerOnlyGeometryPipeline(mDevice, mDownsamplerPipelineLayout) };
    Sampler mDownsamplerSampler{ mDevice, Sampler::simple(vk::Filter::eNearest, vk::SamplerMipmapMode::eNearest, vk::SamplerAddressMode::eClampToEdge, false, 0.0f, false, vk::CompareOp::eLess, 0.0f, 0.0f) };
};