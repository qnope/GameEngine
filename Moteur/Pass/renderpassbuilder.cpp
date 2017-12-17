#include "renderpassbuilder.h"

std::tuple<RenderPass, std::shared_ptr<FillGBufferSubpass>> RenderPassBuilder::relatedSceneGraph(vk::Device device, SceneGraph & sceneGraph, vk::DescriptorSetLayout matrixDescriptorSetLayout, vk::DescriptorSet matrixDescriptorSet, vk::Extent2D extent, const Buffer &modelMatricesBuffer, const Buffer &indirectBuffer)
{
    RenderPass renderPass{ device };
    renderPass.addAttachment(StructHelper::attachmentDescriptionSimpleWithoutStencil(vk::Format::eR8G8B8A8Unorm, vk::ImageLayout::eUndefined, vk::ImageLayout::eShaderReadOnlyOptimal), Clear::black()); // albedo
    renderPass.addAttachment(StructHelper::attachmentDescriptionSimpleWithoutStencil(vk::Format::eR32G32B32A32Sfloat, vk::ImageLayout::eUndefined, vk::ImageLayout::eShaderReadOnlyOptimal), Clear::black()); // tangent
    renderPass.addAttachment(StructHelper::attachmentDescriptionSimpleWithoutStencil(vk::Format::eR32G32B32A32Sfloat, vk::ImageLayout::eUndefined, vk::ImageLayout::eShaderReadOnlyOptimal), Clear::black()); // normal
    renderPass.addAttachment(StructHelper::attachmentDescriptionSimpleWithoutStencil(vk::Format::eR8G8Unorm, vk::ImageLayout::eUndefined, vk::ImageLayout::eShaderReadOnlyOptimal), Clear::black());
    renderPass.addAttachment(StructHelper::attachmentDescriptionSimpleWithoutStencil(vk::Format::eD32Sfloat, vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthStencilReadOnlyOptimal), Clear::horizon());; // depth

    auto fillGBufferSubpass{ std::make_shared<FillGBufferSubpass>(device, sceneGraph, matrixDescriptorSetLayout, matrixDescriptorSet, modelMatricesBuffer, indirectBuffer) };

    renderPass.addSubpass(fillGBufferSubpass, false);

    renderPass.addDependency(nullptr, renderPass.getFirstSubpass(),
        vk::PipelineStageFlagBits::eTopOfPipe,
        vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests,
        vk::AccessFlags(),
        Access::subpassWithoutInputBits);

    renderPass.addDependency(renderPass.getLastSubpass(), nullptr,
        vk::PipelineStageFlagBits::eEarlyFragmentTests | vk::PipelineStageFlagBits::eColorAttachmentOutput,
        vk::PipelineStageFlagBits::eBottomOfPipe,
        Access::subpassWithoutInputBits,
        vk::AccessFlags());

    renderPass.create(extent);
    return std::make_tuple(std::move(renderPass), fillGBufferSubpass);
}

std::tuple<RenderPass, std::shared_ptr<FullScreenSubPass>> RenderPassBuilder::fullScreen(vk::Device device, vk::Format format, vk::ImageLayout initialLayout, vk::ImageLayout finalLayout)
{
    auto colorAttachment = StructHelper::attachmentDescriptionSimpleWithoutStencil(format, initialLayout, finalLayout);

    auto subpass{ std::make_shared<FullScreenSubPass>(device) };
    RenderPass renderPass{ device };
    renderPass.addSubpass(subpass, false);
    renderPass.addAttachment(colorAttachment, Clear::black());

    renderPass.addDependency(
        nullptr, subpass,
        vk::PipelineStageFlagBits::eTopOfPipe,
        vk::PipelineStageFlagBits::eFragmentShader | vk::PipelineStageFlagBits::eColorAttachmentOutput, // Fragment shader will read datas
        vk::AccessFlags(),
        vk::AccessFlagBits::eUniformRead | vk::AccessFlagBits::eShaderRead | Access::colorAttachmentBits);

    renderPass.addDependency(
        subpass, nullptr,
        vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::PipelineStageFlagBits::eBottomOfPipe,
        Access::colorAttachmentBits, vk::AccessFlags());

    return std::make_tuple(std::move(renderPass), subpass);
}

std::tuple<RenderPass, std::shared_ptr<ImGUISubpass>> RenderPassBuilder::imgui(ImGUIInstance & instance, vk::Device device, vk::Format format, vk::Extent2D extent, BufferFactory & bufferFactory, vk::ImageLayout initialLayout, vk::ImageLayout finalLayout)
{
    auto colorAttachment = StructHelper::attachmentDescriptionSimpleWithoutStencilWithoutClear(format, initialLayout, finalLayout);

    auto subpass{ std::make_shared<ImGUISubpass>(instance, device, bufferFactory) };
    RenderPass renderPass{ device };
    renderPass.addSubpass(subpass, false);
    renderPass.addAttachment(colorAttachment, Clear::black());

    renderPass.addDependency(
        nullptr, subpass,
        vk::PipelineStageFlagBits::eBottomOfPipe, vk::PipelineStageFlagBits::eColorAttachmentOutput,
        vk::AccessFlags(), Access::colorAttachmentBits);

    renderPass.addDependency(
        subpass, nullptr,
        vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::PipelineStageFlagBits::eBottomOfPipe,
        Access::colorAttachmentBits, vk::AccessFlags());

    renderPass.create(extent);
    return std::make_tuple(std::move(renderPass), subpass);
}

std::tuple<RenderPass, std::shared_ptr<Subpass>> RenderPassBuilder::voxelizationPass(vk::Device device, uint32_t resolution, uint32_t clipMapNumber, bool onlyGeometry, SceneGraph & sceneGraph, vk::PipelineLayout pipelineLayout, vk::DescriptorSet cubeVoxelizationInfoImageDescriptorSet, const Buffer &cubeVoxelizationInfoBuffer, const Buffer & modelMatricesBuffer, const Buffer & indirectBuffer, std::unique_ptr<CombinedImage>& combinedImage, ImageFactory & imageFactory)
{
    std::shared_ptr<Subpass> subPass;
    RenderPass renderPass{ device };

    if (onlyGeometry)
        subPass = std::make_shared<VoxelizationGeometrySubPass>(device, resolution, clipMapNumber, sceneGraph, pipelineLayout, cubeVoxelizationInfoImageDescriptorSet, cubeVoxelizationInfoBuffer, modelMatricesBuffer, indirectBuffer, combinedImage, imageFactory);

    renderPass.addSubpass(subPass, false);

    renderPass.addDependency(nullptr, renderPass.getFirstSubpass(),
        vk::PipelineStageFlagBits::eTransfer, // UBO Voxelization Information
        vk::PipelineStageFlagBits::eGeometryShader | vk::PipelineStageFlagBits::eFragmentShader, // This UBO is used in geometry and fragment shaders
        vk::AccessFlagBits::eTransferWrite,
        vk::AccessFlagBits::eUniformRead); // UBO are uniform read

    renderPass.addDependency(renderPass.getLastSubpass(), nullptr,
        vk::PipelineStageFlagBits::eTopOfPipe,
        vk::PipelineStageFlagBits::eBottomOfPipe,
        vk::AccessFlags(),
        vk::AccessFlags());

    renderPass.create(vk::Extent2D{ resolution, resolution });

    return std::tuple<RenderPass, std::shared_ptr<Subpass>>(std::move(renderPass), subPass);
}
