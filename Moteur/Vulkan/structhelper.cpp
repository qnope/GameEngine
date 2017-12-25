#include "StructHelper.h"

vk::AccessFlags Access::allBits = vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite | vk::AccessFlagBits::eHostRead | vk::AccessFlagBits::eHostWrite | vk::AccessFlagBits::eIndexRead | vk::AccessFlagBits::eIndirectCommandRead | vk::AccessFlagBits::eInputAttachmentRead | vk::AccessFlagBits::eMemoryRead | vk::AccessFlagBits::eMemoryWrite | vk::AccessFlagBits::eShaderRead | vk::AccessFlagBits::eShaderWrite | vk::AccessFlagBits::eTransferRead | vk::AccessFlagBits::eTransferWrite | vk::AccessFlagBits::eUniformRead | vk::AccessFlagBits::eVertexAttributeRead;
vk::AccessFlags Access::readBits = vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eHostRead | vk::AccessFlagBits::eIndexRead | vk::AccessFlagBits::eIndirectCommandRead | vk::AccessFlagBits::eInputAttachmentRead | vk::AccessFlagBits::eShaderRead | vk::AccessFlagBits::eTransferRead | vk::AccessFlagBits::eMemoryRead | vk::AccessFlagBits::eUniformRead | vk::AccessFlagBits::eVertexAttributeRead;
vk::AccessFlags Access::writeBits = vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite | vk::AccessFlagBits::eHostWrite | vk::AccessFlagBits::eMemoryWrite | vk::AccessFlagBits::eTransferWrite | vk::AccessFlagBits::eShaderWrite;
vk::AccessFlags Access::colorAttachmentBits = vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite;
vk::AccessFlags Access::depthAttachmentBits = vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite;
vk::AccessFlags Access::inputAttachmentBits = vk::AccessFlagBits::eInputAttachmentRead;
vk::AccessFlags Access::subpassBits = Access::colorAttachmentBits | Access::depthAttachmentBits | Access::inputAttachmentBits;
vk::AccessFlags Access::subpassWithoutInputBits = Access::colorAttachmentBits | Access::depthAttachmentBits;

vk::PipelineStageFlags PipelineStageFlags::subpassStages = vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests | vk::PipelineStageFlagBits::eLateFragmentTests;
