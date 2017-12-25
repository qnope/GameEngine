#pragma once

#include "vulkan.h"
#include "Buffer.h"
#include "Image.h"

struct Clear {
    static vk::ClearColorValue black() {
        return vk::ClearColorValue(std::array<float, 4>{ { 0.0, 0.0, 0.0, 0.0 }});
    }

    static vk::ClearDepthStencilValue horizon() {
        return vk::ClearDepthStencilValue(1.0f, 0u);
    }
};

struct ImageSubresource {
    static vk::ComponentMapping identityMapping() {
        return vk::ComponentMapping(vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity);
    }

    static vk::ImageSubresourceLayers layersDepth(uint32_t mipLevel = 0, uint32_t baseArrayLayer = 0, uint32_t layerCount = 1) {
        return vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eDepth, mipLevel, baseArrayLayer, layerCount);
    }

    static vk::ImageSubresourceLayers layersColor(uint32_t mipLevel = 0, uint32_t baseArrayLayer = 0, uint32_t layerCount = 1) {
        return vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, mipLevel, baseArrayLayer, layerCount);
    }

    static vk::ImageSubresourceRange rangeDepth() {
        return vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eDepth, 0u, VK_REMAINING_MIP_LEVELS, 0u, VK_REMAINING_ARRAY_LAYERS);
    }

    static vk::ImageSubresourceRange rangeColor(uint32_t mipLevel = 0, uint32_t levelCount = VK_REMAINING_MIP_LEVELS) {
        return vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, mipLevel, levelCount, 0u, VK_REMAINING_ARRAY_LAYERS);
    }
};

struct Access {
    static vk::AccessFlags allBits;

    static vk::AccessFlags readBits;
    static vk::AccessFlags writeBits;

    static vk::AccessFlags colorAttachmentBits;
    static vk::AccessFlags inputAttachmentBits;
    static vk::AccessFlags depthAttachmentBits;

    static vk::AccessFlags subpassBits;
    static vk::AccessFlags subpassWithoutInputBits;
};

struct PipelineStageFlags {
    static vk::PipelineStageFlags subpassStages;
};

struct MemoryBarrier {
    static vk::MemoryBarrier global(
        vk::AccessFlags src = Access::allBits,
        vk::AccessFlags dst = Access::allBits) {
        return vk::MemoryBarrier(src, dst);
    }

    static vk::BufferMemoryBarrier buffer(vk::Buffer buffer,
        vk::AccessFlags src = Access::allBits,
        vk::AccessFlags dst = Access::allBits,
        vk::DeviceSize offset = 0, vk::DeviceSize size = VK_WHOLE_SIZE) {
        return vk::BufferMemoryBarrier(src, dst, 0, 0, buffer, offset, size);
    }

    static vk::ImageMemoryBarrier image(vk::Image image,
        vk::ImageLayout oldLayout, vk::ImageLayout newLayout,
        vk::ImageSubresourceRange range = ImageSubresource::rangeColor(),
        vk::AccessFlags src = Access::allBits,
        vk::AccessFlags dst = Access::allBits) {
        return vk::ImageMemoryBarrier{ src, dst, oldLayout, newLayout, 0u, 0u, image, range };
    }

    static void barrier(const vk::ArrayProxy<const vk::MemoryBarrier> &barriers, vk::CommandBuffer cmd,
        vk::PipelineStageFlags src = vk::PipelineStageFlagBits::eAllCommands,
        vk::PipelineStageFlags dst = vk::PipelineStageFlagBits::eAllCommands,
        vk::DependencyFlags dependencyFlags = vk::DependencyFlags()) {
        cmd.pipelineBarrier(src, dst, dependencyFlags, barriers, {}, {});
    }

    static void barrier(const vk::ArrayProxy<const vk::BufferMemoryBarrier> &barriers,
        vk::CommandBuffer cmd,
        vk::PipelineStageFlags src = vk::PipelineStageFlagBits::eAllCommands,
        vk::PipelineStageFlags dst = vk::PipelineStageFlagBits::eAllCommands,
        vk::DependencyFlags dependencyFlags = vk::DependencyFlags()) {
        cmd.pipelineBarrier(src, dst, dependencyFlags, {}, barriers, {});
    }

    static void barrier(const vk::ArrayProxy<const vk::ImageMemoryBarrier> &barriers,
        vk::CommandBuffer cmd,
        vk::PipelineStageFlags src = vk::PipelineStageFlagBits::eAllCommands,
        vk::PipelineStageFlags dst = vk::PipelineStageFlagBits::eAllCommands,
        vk::DependencyFlags dependencyFlags = vk::DependencyFlags()) {
        cmd.pipelineBarrier(src, dst, dependencyFlags, {}, {}, barriers);
    }

    static void barrier(const vk::ArrayProxy<const vk::BufferMemoryBarrier> &buffers, const vk::ArrayProxy<const vk::ImageMemoryBarrier> images,
        vk::CommandBuffer cmd,
        vk::PipelineStageFlags src = vk::PipelineStageFlagBits::eAllCommands,
        vk::PipelineStageFlags dst = vk::PipelineStageFlagBits::eAllCommands,
        vk::DependencyFlags dependencyFlags = vk::DependencyFlags()) {
        cmd.pipelineBarrier(src, dst, dependencyFlags, {}, buffers, images);
    }

    static void transition(vk::Image img,
        vk::ImageLayout oldLayout, vk::ImageLayout newLayout,
        vk::CommandBuffer cmd, vk::ImageSubresourceRange subresourceRange = ImageSubresource::rangeColor(),
        vk::PipelineStageFlags srcStage = vk::PipelineStageFlagBits::eAllCommands,
        vk::PipelineStageFlagBits dstStage = vk::PipelineStageFlagBits::eAllCommands,
        vk::DependencyFlagBits dependencyFlags = vk::DependencyFlagBits()) {

        assert(
            newLayout == vk::ImageLayout::eShaderReadOnlyOptimal ||
            newLayout == vk::ImageLayout::eTransferDstOptimal ||
            newLayout == vk::ImageLayout::eTransferSrcOptimal);

        assert(
            oldLayout == vk::ImageLayout::ePreinitialized ||
            oldLayout == vk::ImageLayout::eUndefined ||
            oldLayout == vk::ImageLayout::eTransferDstOptimal ||
            oldLayout == vk::ImageLayout::eTransferSrcOptimal);

        vk::AccessFlags srcAccess;
        vk::AccessFlags dstAccess;

        if (oldLayout == vk::ImageLayout::eTransferDstOptimal)
            srcAccess = vk::AccessFlagBits::eTransferWrite;

        else if (oldLayout == vk::ImageLayout::eTransferSrcOptimal)
            srcAccess = vk::AccessFlagBits::eTransferRead;

        if (newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
            dstAccess = vk::AccessFlagBits::eShaderRead;

        else if (newLayout == vk::ImageLayout::eTransferDstOptimal)
            dstAccess = vk::AccessFlagBits::eTransferWrite; // Useless, but it is for the layers

        else if (newLayout == vk::ImageLayout::eTransferSrcOptimal)
            dstAccess = vk::AccessFlagBits::eTransferRead;

        vk::ImageMemoryBarrier barrier(srcAccess, dstAccess, oldLayout, newLayout, 0, 0, img, subresourceRange);
        cmd.pipelineBarrier(srcStage, dstStage, dependencyFlags, {}, {}, barrier);
    }

    static vk::SubpassDependency subpassDependency(uint32_t src, uint32_t dst, vk::AccessFlags srcAccess = Access::allBits, vk::AccessFlags dstAccess = Access::allBits,
        vk::PipelineStageFlags srcStage = vk::PipelineStageFlagBits::eAllCommands, vk::PipelineStageFlags dstStage = vk::PipelineStageFlagBits::eAllCommands,
        vk::DependencyFlags dependency = vk::DependencyFlags()) {
        return vk::SubpassDependency(src, dst, srcStage, dstStage, srcAccess, dstAccess, dependency);
    }
};

struct PipelineHelper {
    static vk::PipelineVertexInputStateCreateInfo vertexInputEmpty() {
        return vk::PipelineVertexInputStateCreateInfo();
    }

    static vk::PipelineVertexInputStateCreateInfo vertexInput(const std::vector<vk::VertexInputBindingDescription> &bindingsDescription, const std::vector<vk::VertexInputAttributeDescription> &attributeDescriptions) {
        return vk::PipelineVertexInputStateCreateInfo(vk::PipelineVertexInputStateCreateFlags(), (uint32_t)bindingsDescription.size(), bindingsDescription.data(), (uint32_t)attributeDescriptions.size(), attributeDescriptions.data());
    }

    static vk::PipelineInputAssemblyStateCreateInfo inputAssemblyTri() {
        vk::PipelineInputAssemblyStateCreateInfo info;
        info.topology = vk::PrimitiveTopology::eTriangleList;
        return info;
    }

    static vk::PipelineInputAssemblyStateCreateInfo inputAssemblyTriStrip() {
        vk::PipelineInputAssemblyStateCreateInfo r;
        r.topology = vk::PrimitiveTopology::eTriangleStrip;
        return r;
    }

    static vk::Viewport simpleViewport(vk::Extent2D extent) {
        return vk::Viewport(0.0f, 0.0f, (float)extent.width, (float)extent.height, 0.0f, 1.0f);
    }

    static vk::Rect2D simpleScissor(vk::Extent2D extent) {
        return vk::Rect2D(vk::Offset2D(), extent);
    }

    static vk::PipelineViewportStateCreateInfo simpleViewportState(vk::Viewport *vp, vk::Rect2D *s) {
        return vk::PipelineViewportStateCreateInfo{ vk::PipelineViewportStateCreateFlags(), 1u, vp, 1u, s };
    }

    static vk::PipelineViewportStateCreateInfo dynamicViewportState() {
        return vk::PipelineViewportStateCreateInfo{ vk::PipelineViewportStateCreateFlags(), 1u, nullptr, 1u, nullptr };
    }

    static vk::PipelineDynamicStateCreateInfo dynamicState(const std::vector<vk::DynamicState> &states) {
        return vk::PipelineDynamicStateCreateInfo{ vk::PipelineDynamicStateCreateFlags(), (uint32_t)states.size(), states.data() };
    }

    static vk::PipelineRasterizationStateCreateInfo rasterisationSimple() {
        vk::PipelineRasterizationStateCreateInfo info;
        info.polygonMode = vk::PolygonMode::eFill;
        info.cullMode = vk::CullModeFlagBits::eNone;
        info.frontFace = vk::FrontFace::eCounterClockwise;
        info.lineWidth = 1.0f;
        return info;
    }

    static vk::PipelineMultisampleStateCreateInfo multisampleSimple() {
        vk::PipelineMultisampleStateCreateInfo info;
        info.rasterizationSamples = vk::SampleCountFlagBits::e1;
        info.minSampleShading = 1.0f;
        info.pSampleMask = nullptr;
        return info;
    }

    static vk::PipelineDepthStencilStateCreateInfo depthStencilOnlyDepth(bool write, vk::CompareOp compareOp = vk::CompareOp::eLess) {
        vk::PipelineDepthStencilStateCreateInfo info;
        info.depthTestEnable = true;
        info.depthWriteEnable = write;
        info.depthCompareOp = compareOp;
        return info;
    }

    static vk::ColorComponentFlags colorComponentAll() {
        return vk::ColorComponentFlagBits::eA | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eR;
    }

    static vk::PipelineColorBlendAttachmentState colorBlendAttachmentSimple() {
        vk::PipelineColorBlendAttachmentState state;
        state.colorWriteMask = colorComponentAll();
        return state;
    }

    static vk::PipelineColorBlendStateCreateInfo colorBlend(std::vector<vk::PipelineColorBlendAttachmentState> &attachments) {
        vk::PipelineColorBlendStateCreateInfo info;
        info.attachmentCount = (uint32_t)attachments.size();
        info.pAttachments = attachments.data();
        return info;
    }
};

struct DescriptorPoolSize {
    static vk::DescriptorPoolSize uniformBufferDynamic(uint32_t count = 1) {
        return vk::DescriptorPoolSize(vk::DescriptorType::eUniformBufferDynamic, count);
    }

    static vk::DescriptorPoolSize uniformBuffer(uint32_t count = 1) {
        return vk::DescriptorPoolSize(vk::DescriptorType::eUniformBuffer, count);
    }

    static vk::DescriptorPoolSize combinedImage(uint32_t count = 1) {
        return vk::DescriptorPoolSize(vk::DescriptorType::eCombinedImageSampler, count);
    }

    static vk::DescriptorPoolSize sampler(uint32_t count = 1) {
        return vk::DescriptorPoolSize(vk::DescriptorType::eSampler, count);
    }

    static vk::DescriptorPoolSize sampledImage(uint32_t count = 1) {
        return vk::DescriptorPoolSize(vk::DescriptorType::eSampledImage, count);
    }

    static vk::DescriptorPoolSize storageImage(uint32_t count = 1) {
        return vk::DescriptorPoolSize(vk::DescriptorType::eStorageImage, count);
    }

    static vk::DescriptorPoolSize texelBuffer(uint32_t count = 1) {
        return vk::DescriptorPoolSize(vk::DescriptorType::eUniformTexelBuffer, count);
    }

    static vk::DescriptorPoolSize storageTexelBuffer(uint32_t count = 1) {
        return vk::DescriptorPoolSize(vk::DescriptorType::eStorageTexelBuffer, count);
    }

    static vk::DescriptorPoolSize storageBuffer(uint32_t count = 1) {
        return vk::DescriptorPoolSize(vk::DescriptorType::eStorageBuffer, count);
    }

    static vk::DescriptorPoolSize storageBufferDynamic(uint32_t count = 1) {
        return vk::DescriptorPoolSize(vk::DescriptorType::eStorageBufferDynamic, count);
    }

    static vk::DescriptorPoolSize inputAttachment(uint32_t count = 1) {
        return vk::DescriptorPoolSize(vk::DescriptorType::eInputAttachment, count);
    }
};

struct LayoutBinding {
    static vk::DescriptorSetLayoutBinding uniformBuffer(uint32_t binding, uint32_t number, vk::ShaderStageFlags stages = vk::ShaderStageFlagBits::eAll) {
        return vk::DescriptorSetLayoutBinding(binding, vk::DescriptorType::eUniformBuffer, number, stages);
    }

    static vk::DescriptorSetLayoutBinding uniformBufferDynamic(uint32_t binding, uint32_t number, vk::ShaderStageFlags stages = vk::ShaderStageFlagBits::eAll) {
        return vk::DescriptorSetLayoutBinding(binding, vk::DescriptorType::eUniformBufferDynamic, number, stages);
    }

    static vk::DescriptorSetLayoutBinding combinedImage(uint32_t binding, uint32_t number, vk::ShaderStageFlags stages = vk::ShaderStageFlagBits::eAll) {
        return vk::DescriptorSetLayoutBinding(binding, vk::DescriptorType::eCombinedImageSampler, number, stages);
    }

    static vk::DescriptorSetLayoutBinding storageImage(uint32_t binding, uint32_t number, vk::ShaderStageFlags stages = vk::ShaderStageFlagBits::eAll) {
        return vk::DescriptorSetLayoutBinding(binding, vk::DescriptorType::eStorageImage, number, stages);
    }
};

struct StructHelper {
    static vk::AttachmentDescription attachmentDescriptionSimpleWithoutStencil(vk::Format format, vk::ImageLayout initialLayout, vk::ImageLayout finalLayout) {
        vk::AttachmentDescription colorAttachment;
        colorAttachment.format = format;
        colorAttachment.samples = vk::SampleCountFlagBits::e1;
        colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
        colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
        colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
        colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
        colorAttachment.initialLayout = initialLayout;
        colorAttachment.finalLayout = finalLayout;
        return colorAttachment;
    }

    static vk::AttachmentDescription attachmentDescriptionSimpleWithoutStencilWithoutClear(vk::Format format, vk::ImageLayout initialLayout, vk::ImageLayout finalLayout) {
        vk::AttachmentDescription colorAttachment;

        colorAttachment.format = format;
        colorAttachment.samples = vk::SampleCountFlagBits::e1;
        colorAttachment.loadOp = vk::AttachmentLoadOp::eLoad;
        colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
        colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
        colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
        colorAttachment.initialLayout = initialLayout;
        colorAttachment.finalLayout = finalLayout;

        return colorAttachment;
    }

    static vk::WriteDescriptorSet writeDescriptorSet(vk::DescriptorSet set, uint32_t binding, const vk::DescriptorBufferInfo *info, vk::DescriptorType type = vk::DescriptorType::eUniformBuffer) {
        return vk::WriteDescriptorSet(set, binding, 0, 1, type, nullptr, info, nullptr);
    }

    static vk::WriteDescriptorSet writeDescriptorSet(vk::DescriptorSet set, uint32_t binding, const vk::DescriptorImageInfo *info, vk::DescriptorType type = vk::DescriptorType::eCombinedImageSampler) {
        return vk::WriteDescriptorSet(set, binding, 0, 1, type, info, nullptr, nullptr);
    }

    static vk::CommandBufferBeginInfo beginCommand(vk::CommandBufferUsageFlags flags = vk::CommandBufferUsageFlags(),
        vk::CommandBufferInheritanceInfo *inheritanceInfo = nullptr) {
        return vk::CommandBufferBeginInfo(flags, inheritanceInfo);
    }

    static void transfer(const Buffer &src, const Buffer &dst, vk::CommandBuffer cmd,
        vk::DeviceSize srcOffset = 0, vk::DeviceSize dstOffset = 0,
        vk::DeviceSize size = VK_WHOLE_SIZE) {
        if (size == VK_WHOLE_SIZE)
            size = std::min<vk::DeviceSize>(src.getSize() - srcOffset, dst.getSize() - dstOffset);

        assert(srcOffset + size <= src.getSize());
        assert(dstOffset + size <= dst.getSize());

        assert((src.getUsage() & vk::BufferUsageFlagBits::eTransferSrc) == vk::BufferUsageFlagBits::eTransferSrc);
        assert((dst.getUsage() & vk::BufferUsageFlagBits::eTransferDst) == vk::BufferUsageFlagBits::eTransferDst);

        cmd.copyBuffer(src, dst, vk::BufferCopy(srcOffset, dstOffset, size));
    }

    static void transfer(const Buffer &src, const Image &dst, vk::CommandBuffer cmd,
        vk::DeviceSize srcOffset = 0,
        vk::ImageSubresourceLayers imageSubresourceLayers = ImageSubresource::layersColor(),
        vk::Offset3D dstOffset = vk::Offset3D(),
        vk::Extent3D dstExtent = vk::Extent3D()) {

        assert((src.getUsage() & vk::BufferUsageFlagBits::eTransferSrc) == vk::BufferUsageFlagBits::eTransferSrc);
        assert((dst.getUsage() & vk::ImageUsageFlagBits::eTransferDst) == vk::ImageUsageFlagBits::eTransferDst);

        if (dstExtent == vk::Extent3D()) {
            dstExtent = dst.getSize();
            dstExtent.width -= dstOffset.x;
            dstExtent.height -= dstOffset.y;
            dstExtent.depth -= dstOffset.z;
        }

        cmd.copyBufferToImage(src, dst, vk::ImageLayout::eTransferDstOptimal, vk::BufferImageCopy(srcOffset, 0, 0, imageSubresourceLayers, dstOffset, dstExtent));
    }
};
