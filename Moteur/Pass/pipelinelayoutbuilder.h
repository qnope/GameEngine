#pragma once

#include "../Vulkan/pipelinelayout.h"

struct PipelineLayoutBuilder {
	static PipelineLayout build(vk::Device device, const std::vector<vk::DescriptorSetLayout> &layouts, const std::vector<vk::PushConstantRange> ranges) {
		PipelineLayout layout(device);

		for (auto &l : layouts)
			layout.addDescriptorSetLayout(l);

		for (auto &r : ranges)
			layout.addPushConstantRange(r);

		layout.create();
		return layout;
	}

	static PipelineLayout build(vk::Device device, const std::vector<vk::DescriptorSetLayout> &layouts) {
		return build(device, layouts, {});
	}

	static PipelineLayout build(vk::Device device, const std::vector<vk::PushConstantRange> ranges) {
		return build(device, {}, ranges);
	}
};
