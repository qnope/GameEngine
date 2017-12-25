#include "Pipeline.h"
#include "Tools/stream.h"
#include <map>
char const *_main = "main";

Pipeline::Pipeline(vk::Device device) : mDevice(device)
{

}

void Pipeline::setShaderStage(std::string filename, vk::ShaderStageFlagBits stage)
{
    static std::map<vk::ShaderStageFlagBits, EShLanguage> associationTable = {
        {vk::ShaderStageFlagBits::eVertex, EShLanguage::EShLangVertex},
        {vk::ShaderStageFlagBits::eTessellationControl, EShLanguage::EShLangTessControl},
        {vk::ShaderStageFlagBits::eTessellationEvaluation, EShLanguage::EShLangTessEvaluation},
        {vk::ShaderStageFlagBits::eGeometry, EShLanguage::EShLangGeometry},
        {vk::ShaderStageFlagBits::eFragment, EShLanguage::EShLangFragment},
        {vk::ShaderStageFlagBits::eCompute, EShLanguage::EShLangCompute},
    };

    assert(std::find_if(mShaderStages.begin(), mShaderStages.end(), [stage](auto s) {if (s.stage == stage) return true; return false; }) == mShaderStages.end());
    mShaders.emplace_back(mDevice, filename, associationTable[stage]);

    vk::PipelineShaderStageCreateInfo v;
    v.module = mShaders.back().get();
    v.stage = stage;
    v.pName = _main;

    mShaderStages << v;
}

void Pipeline::addVertexInputBinding(vk::VertexInputBindingDescription binding)
{
    mVertexInputBindingDescriptions << binding;
}

void Pipeline::addVertexInputAttribute(vk::VertexInputAttributeDescription attribute)
{
    mVertexInputAttributeDescriptions << attribute;
}

void Pipeline::setInputAssemblyState(vk::PipelineInputAssemblyStateCreateInfo state)
{
    mInputAssemblyState = state;
}

void Pipeline::setTessellationState(vk::PipelineTessellationStateCreateInfo state)
{
    mTesselationEnabled = true;
    mTessellationState = state;
}

void Pipeline::addViewport(vk::Viewport viewport)
{
    ++mViewportNumber;
    mViewports << viewport;
}

void Pipeline::addScissor(vk::Rect2D scissor)
{
    ++mScissorNumber;
    mScissors << scissor;
}

void Pipeline::addDynamicViewport()
{
    ++mViewportNumber;
    mDynamicStates << vk::DynamicState::eViewport;
}

void Pipeline::addDynamicScissor()
{
    ++mScissorNumber;
    mDynamicStates << vk::DynamicState::eScissor;
}

void Pipeline::setRasterizationState(vk::CullModeFlags cullMode, vk::FrontFace frontFace)
{
    mRasterizationState.cullMode = cullMode;
    mRasterizationState.frontFace = frontFace;
}

void Pipeline::setDepthState(vk::Bool32 enable, vk::Bool32 write, vk::CompareOp op)
{
    mDepthStencilEnabled = true;
    mDepthStencilState.depthTestEnable = enable;
    mDepthStencilState.depthWriteEnable = write;
    mDepthStencilState.depthCompareOp = op;
}

void Pipeline::setColorBlendLogicOp(vk::LogicOp op)
{
    mColorBlendLogicOpEnabled = true;
    mColorBlendLogicOp = op;
}

void Pipeline::addColorBlendAttachment(vk::Bool32 blendEnable, vk::BlendFactor srcColorBlendFactor, vk::BlendFactor dstColorBlendFactor, vk::BlendOp colorBlendOp, vk::BlendFactor srcAlphaBlendFactor, vk::BlendFactor dstAlphaBlendFactor, vk::BlendOp alphaBlendOp)
{
    mColorBlendAttachments.emplace_back(
        blendEnable,
        srcColorBlendFactor, dstColorBlendFactor, colorBlendOp,
        srcAlphaBlendFactor, dstAlphaBlendFactor, alphaBlendOp,
        PipelineHelper::colorComponentAll()
    );
}

void Pipeline::addColorBlendAttachment(vk::PipelineColorBlendAttachmentState attachment)
{
    mColorBlendAttachments << attachment;
}

void Pipeline::createGraphics(vk::RenderPass renderpass, vk::PipelineLayout layout, uint32_t subpass)
{
    auto vertexInputState = PipelineHelper::vertexInput(mVertexInputBindingDescriptions, mVertexInputAttributeDescriptions);

    vk::PipelineViewportStateCreateInfo viewportState{
        vk::PipelineViewportStateCreateFlags(),
        mViewportNumber, mViewports.data(),
        mScissorNumber, mScissors.data()
    };

    vk::PipelineDynamicStateCreateInfo dynamicState{
        vk::PipelineDynamicStateCreateFlags(),
        (uint32_t)mDynamicStates.size(), mDynamicStates.data()
    };

    vk::PipelineColorBlendStateCreateInfo colorBlendState{
        vk::PipelineColorBlendStateCreateFlags(),
        mColorBlendLogicOpEnabled, mColorBlendLogicOp,
        (uint32_t)mColorBlendAttachments.size(),
        mColorBlendAttachments.data()
    };

    vk::GraphicsPipelineCreateInfo info{
        vk::PipelineCreateFlags(),
        (uint32_t)mShaderStages.size(), mShaderStages.data(),
        &vertexInputState,
        &mInputAssemblyState,
        (mTesselationEnabled) ? &mTessellationState : nullptr,
        &viewportState,
        &mRasterizationState,
        &mMultisampleState,
        (mDepthStencilEnabled) ? &mDepthStencilState : nullptr,
        &colorBlendState,
        (!mDynamicStates.empty()) ? &dynamicState : nullptr,
        layout,
        renderpass, subpass
    };

    static_cast<vk::UniquePipeline&>(*this) = mDevice.createGraphicsPipelineUnique(vk::PipelineCache(), info);
}

void Pipeline::createCompute(vk::PipelineLayout layout)
{
    assert(mShaderStages.size() == 1);
    vk::ComputePipelineCreateInfo info{
        vk::PipelineCreateFlags(),
        mShaderStages[0],
        layout
    };

    static_cast<vk::UniquePipeline&>(*this) = mDevice.createComputePipelineUnique(vk::PipelineCache(), info);
}
