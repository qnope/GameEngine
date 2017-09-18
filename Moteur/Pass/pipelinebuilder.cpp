#include "pipelinebuilder.h"

#include "../SceneGraph/ModelImporter/mesh.h"

Pipeline PipelineBuilder::buildFullScreenPipeline(vk::Device device, vk::RenderPass renderpass, vk::Extent2D extent2D, vk::PipelineLayout layout, std::string fragmentShader)
{
	Pipeline pipeline(device);

	pipeline.setShaderStage("../Shaders/RenderFullScreen/quad_vert.spv", vk::ShaderStageFlagBits::eVertex);
	pipeline.setShaderStage(fragmentShader, vk::ShaderStageFlagBits::eFragment);

	pipeline.setInputAssemblyState(PipelineHelper::inputAssemblyTriStrip());
	pipeline.addViewport(PipelineHelper::simpleViewport(extent2D));
	pipeline.addScissor(PipelineHelper::simpleScissor(extent2D));

	pipeline.addColorBlendAttachment(PipelineHelper::colorBlendAttachmentSimple());

	pipeline.createGraphics(renderpass, layout, 0);
	return pipeline;
}

Pipeline PipelineBuilder::buildImGUIPipeline(ImGUIInstance &instance, vk::RenderPass renderPass)
{
	Pipeline pipeline(instance.device);

	pipeline.setShaderStage("../Shaders/ImGUI/imgui_vert.spv", vk::ShaderStageFlagBits::eVertex);
	pipeline.setShaderStage("../Shaders/ImGUI/imgui_frag.spv", vk::ShaderStageFlagBits::eFragment);

	pipeline.addVertexInputBinding(vk::VertexInputBindingDescription(0, sizeof(ImDrawVert), vk::VertexInputRate::eVertex));

	pipeline.addVertexInputAttribute(vk::VertexInputAttributeDescription(0, 0, vk::Format::eR32G32Sfloat, offsetof(ImDrawVert, pos)));
	pipeline.addVertexInputAttribute(vk::VertexInputAttributeDescription(1, 0, vk::Format::eR32G32Sfloat, offsetof(ImDrawVert, uv)));
	pipeline.addVertexInputAttribute(vk::VertexInputAttributeDescription(2, 0, vk::Format::eR8G8B8A8Unorm, offsetof(ImDrawVert, col)));

	pipeline.addColorBlendAttachment(true, vk::BlendFactor::eSrcAlpha, vk::BlendFactor::eOneMinusSrcAlpha,
		vk::BlendOp::eAdd, vk::BlendFactor::eOneMinusSrcAlpha, vk::BlendFactor::eZero, vk::BlendOp::eAdd);

	pipeline.addDynamicViewport();
	pipeline.addDynamicScissor();

	pipeline.createGraphics(renderPass, instance.pipelineLayout, 0);
	return pipeline;
}

Pipeline PipelineBuilder::buildFillGBufferPipeline(vk::Device device, vk::RenderPass renderpass, uint32_t subpass, vk::Extent2D extent2D, vk::PipelineLayout layout, std::string fragmentShader)
{
	Pipeline pipeline(device);

	pipeline.setShaderStage("../Shaders/RenderSceneGraphRelated/renderscenegraph_vert.spv", vk::ShaderStageFlagBits::eVertex);
	pipeline.setShaderStage(fragmentShader, vk::ShaderStageFlagBits::eFragment);

	pipeline.addVertexInputBinding(vk::VertexInputBindingDescription(0, sizeof(Vertex), vk::VertexInputRate::eVertex));
	pipeline.addVertexInputBinding(vk::VertexInputBindingDescription(1, sizeof(glm::mat4), vk::VertexInputRate::eInstance));

	pipeline.addVertexInputAttribute(vk::VertexInputAttributeDescription(0, 0, vk::Format::eR32G32B32Sfloat, 0u)); // pos
	pipeline.addVertexInputAttribute(vk::VertexInputAttributeDescription(1, 0, vk::Format::eR32G32B32Sfloat, sizeof(glm::vec3))); // tan
	pipeline.addVertexInputAttribute(vk::VertexInputAttributeDescription(2, 0, vk::Format::eR32G32B32Sfloat, 2 * sizeof(glm::vec3))); // nor
	pipeline.addVertexInputAttribute(vk::VertexInputAttributeDescription(3, 0, vk::Format::eR32G32Sfloat, 3 * sizeof(glm::vec3))); // uv

	pipeline.addVertexInputAttribute(vk::VertexInputAttributeDescription(4, 1, vk::Format::eR32G32B32A32Sfloat, 0u));
	pipeline.addVertexInputAttribute(vk::VertexInputAttributeDescription(5, 1, vk::Format::eR32G32B32A32Sfloat, sizeof(glm::vec4)));
	pipeline.addVertexInputAttribute(vk::VertexInputAttributeDescription(6, 1, vk::Format::eR32G32B32A32Sfloat, 2 * sizeof(glm::vec4)));
	pipeline.addVertexInputAttribute(vk::VertexInputAttributeDescription(7, 1, vk::Format::eR32G32B32A32Sfloat, 3 * sizeof(glm::vec4)));

	pipeline.addViewport(PipelineHelper::simpleViewport(extent2D));
	pipeline.addScissor(PipelineHelper::simpleScissor(extent2D));

	pipeline.setDepthState(true);

	for (int i = 0; i < 3; ++i)
		pipeline.addColorBlendAttachment(PipelineHelper::colorBlendAttachmentSimple());

	pipeline.createGraphics(renderpass, layout, 0);

	return pipeline;
}

Pipeline PipelineBuilder::buildVoxelizationOnlyGeometryPipeline(vk::Device device, vk::RenderPass renderPass, uint32_t resolution, vk::PipelineLayout layout)
{
	Pipeline pipeline{ device };

	pipeline.addVertexInputBinding(vk::VertexInputBindingDescription(0, sizeof(Vertex), vk::VertexInputRate::eVertex));
	pipeline.addVertexInputBinding(vk::VertexInputBindingDescription(1, sizeof(glm::mat4), vk::VertexInputRate::eInstance));

	pipeline.addVertexInputAttribute(vk::VertexInputAttributeDescription(0, 0, vk::Format::eR32G32B32Sfloat, 0u));

	pipeline.addVertexInputAttribute(vk::VertexInputAttributeDescription(1, 1, vk::Format::eR32G32B32A32Sfloat, 0u));
	pipeline.addVertexInputAttribute(vk::VertexInputAttributeDescription(2, 1, vk::Format::eR32G32B32A32Sfloat, sizeof(glm::vec4)));
	pipeline.addVertexInputAttribute(vk::VertexInputAttributeDescription(3, 1, vk::Format::eR32G32B32A32Sfloat, 2 * sizeof(glm::vec4)));
	pipeline.addVertexInputAttribute(vk::VertexInputAttributeDescription(4, 1, vk::Format::eR32G32B32A32Sfloat, 3 * sizeof(glm::vec4)));

	pipeline.setShaderStage("../Shaders/Voxelization/voxelization_vert.spv", vk::ShaderStageFlagBits::eVertex);
	pipeline.setShaderStage("../Shaders/Voxelization/voxelization_geom.spv", vk::ShaderStageFlagBits::eGeometry);
	pipeline.setShaderStage("../Shaders/Voxelization/voxelizationgeometry_frag.spv", vk::ShaderStageFlagBits::eFragment);

	vk::Extent2D extent{ resolution, resolution };
	pipeline.addViewport(PipelineHelper::simpleViewport(extent));
	pipeline.addScissor(PipelineHelper::simpleScissor(extent));

	pipeline.createGraphics(renderPass, layout, 0);

	return pipeline;
}

Pipeline PipelineBuilder::buildVoxelDownsamplerOnlyGeometryPipeline(vk::Device device, vk::PipelineLayout layout)
{
	Pipeline pipeline{ device };

	pipeline.setShaderStage("../Shaders/Voxelization/geometrydownsampler.spv", vk::ShaderStageFlagBits::eCompute);
	pipeline.createCompute(layout);

	return pipeline;
}
