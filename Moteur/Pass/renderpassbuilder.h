#pragma once

#include "renderpass.h"
#include "RenderSceneGraph/fillgbuffersubpass.h"
#include "RenderFullscreen/fullscreensubpass.h"
#include "Voxelization/voxelizationgeometrysubpass.h"

#include "ImGui/imguisubpass.h"

struct RenderPassBuilder {
	static std::tuple<RenderPass, std::shared_ptr<FillGBufferSubpass>> relatedSceneGraph(vk::Device device, SceneGraph &sceneGraph, vk::DescriptorSetLayout matrixDescriptorSetLayout, vk::DescriptorSet matrixDescriptorSet, vk::Extent2D extent, const Buffer &modelMatricesBuffer, const Buffer &indirectBuffer);
	static std::tuple<RenderPass, std::shared_ptr<FullScreenSubPass>> fullScreen(vk::Device device, vk::Format format, vk::ImageLayout initialLayout, vk::ImageLayout finalLayout);
	static std::tuple<RenderPass, std::shared_ptr<ImGUISubpass>> imgui(ImGUIInstance &instance, vk::Device device, vk::Format format, vk::Extent2D extent, BufferFactory &bufferFactory, vk::ImageLayout initialLayout, vk::ImageLayout finalLayout);
	static std::tuple<RenderPass, std::shared_ptr<Subpass>> voxelizationPass(vk::Device device, uint32_t resolution, uint32_t clipMapNumber, bool onlyGeometry, SceneGraph & sceneGraph, vk::PipelineLayout pipelineLayout, vk::DescriptorSet cubeVoxelizationInfoImageDescriptorSet, const Buffer &cubeVoxelizationInfoBuffer, const Buffer & modelMatricesBuffer, const Buffer & indirectBuffer, std::unique_ptr<CombinedImage>& combinedImage, ImageFactory & imageFactory);
};
