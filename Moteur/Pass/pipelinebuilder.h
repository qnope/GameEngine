#pragma once
#include "../imgui/imguiinstance.h"
#include "../Vulkan/pipeline.h"

struct PipelineBuilder {
	static Pipeline buildFullScreenPipeline(vk::Device device, vk::RenderPass renderpass, vk::Extent2D extent2D, vk::PipelineLayout layout, std::string fragmentShader);
	static Pipeline buildImGUIPipeline(ImGUIInstance &instance, vk::RenderPass renderPass);
	static Pipeline buildFillGBufferPipeline(vk::Device device, vk::RenderPass renderpass, uint32_t subpass, vk::Extent2D extent2D, vk::PipelineLayout layout, std::string fragmentShader);
	static Pipeline buildVoxelizationOnlyGeometryPipeline(vk::Device device, vk::RenderPass renderPass, uint32_t resolution, vk::PipelineLayout layout);

	static Pipeline buildVoxelDownsamplerOnlyGeometryPipeline(vk::Device device, vk::PipelineLayout layout);
};