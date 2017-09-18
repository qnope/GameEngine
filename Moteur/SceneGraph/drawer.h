#pragma once

#include "../Vulkan/vulkan.h"
#include "ModelImporter/mesh.h"

// A full object 
struct DrawCmd {
	// Part of one object
	struct _ObjectPart {
		using MaterialIndex = std::tuple<uint32_t, uint32_t>;
		vk::Buffer vbo;
		vk::Buffer ibo;
		uint32_t indexCount;
		uint32_t vertexOffset;
		uint32_t firstIndex;
		MaterialIndex materialIndex;
		AABB aabb;
	};

	uint32_t firstInstance = 0;
	uint32_t instanceCount = 1;

	std::vector<_ObjectPart> mParts;
};

struct Drawer {
	vk::Buffer vbo;
	vk::Buffer ibo;

	vk::Pipeline pipeline;
	vk::PipelineLayout pipelineLayout;

	DrawCmd::_ObjectPart::MaterialIndex materialIndex;
	vk::DescriptorSet materialSet;
	uint32_t offsetAlbedoColor;
};