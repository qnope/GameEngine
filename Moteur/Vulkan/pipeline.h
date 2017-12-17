#pragma once

#include "shader.h"
#include "structhelper.h"

class Pipeline : public vk::UniquePipeline {
public:
    Pipeline() = default;
    Pipeline(vk::Device device);

    void setShaderStage(std::string filename, vk::ShaderStageFlagBits stage);

    void addVertexInputBinding(vk::VertexInputBindingDescription binding);
    void addVertexInputAttribute(vk::VertexInputAttributeDescription attribute);

    void setInputAssemblyState(vk::PipelineInputAssemblyStateCreateInfo state);

    void setTessellationState(vk::PipelineTessellationStateCreateInfo state);

    void addViewport(vk::Viewport viewport);
    void addScissor(vk::Rect2D scissor);

    void addDynamicViewport();
    void addDynamicScissor();

    void setRasterizationState(vk::CullModeFlags cullMode = vk::CullModeFlagBits::eNone, vk::FrontFace frontFace = vk::FrontFace::eCounterClockwise);

    void setDepthState(vk::Bool32 enable = false, vk::Bool32 write = true, vk::CompareOp op = vk::CompareOp::eLess);

    void setColorBlendLogicOp(vk::LogicOp op);
    void addColorBlendAttachment(vk::Bool32 blendEnable = false,
        vk::BlendFactor srcColorBlendFactor = vk::BlendFactor::eOne, vk::BlendFactor dstColorBlendFactor = vk::BlendFactor::eOne,
        vk::BlendOp colorBlendOp = vk::BlendOp::eAdd,
        vk::BlendFactor srcAlphaBlendFactor = vk::BlendFactor::eOne, vk::BlendFactor dstAlphaBlendFactor = vk::BlendFactor::eOne,
        vk::BlendOp alphaBlendOp = vk::BlendOp::eAdd);

    void addColorBlendAttachment(vk::PipelineColorBlendAttachmentState attachment);

    void createGraphics(vk::RenderPass renderpass, vk::PipelineLayout layout, uint32_t subpass);
    void createCompute(vk::PipelineLayout layout);

protected:
    vk::Device mDevice;

    std::vector<Shader> mShaders;
    std::vector<vk::PipelineShaderStageCreateInfo> mShaderStages;

    std::vector<vk::VertexInputBindingDescription> mVertexInputBindingDescriptions;
    std::vector<vk::VertexInputAttributeDescription> mVertexInputAttributeDescriptions;
    vk::PipelineInputAssemblyStateCreateInfo mInputAssemblyState{ PipelineHelper::inputAssemblyTri() };

    bool mTesselationEnabled{ false };
    vk::PipelineTessellationStateCreateInfo mTessellationState;

    uint32_t mViewportNumber{ 0 };
    uint32_t mScissorNumber{ 0 };
    std::vector<vk::Viewport> mViewports;
    std::vector<vk::Rect2D> mScissors;

    vk::PipelineRasterizationStateCreateInfo mRasterizationState{ PipelineHelper::rasterisationSimple() };
    vk::PipelineMultisampleStateCreateInfo mMultisampleState{ PipelineHelper::multisampleSimple() };

    bool mDepthStencilEnabled{ false };
    vk::PipelineDepthStencilStateCreateInfo mDepthStencilState;

    bool mColorBlendLogicOpEnabled{ false };
    vk::LogicOp mColorBlendLogicOp;
    std::vector<vk::PipelineColorBlendAttachmentState> mColorBlendAttachments;

    std::vector<vk::DynamicState> mDynamicStates;
};
