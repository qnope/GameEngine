#pragma once

#include "device.h"
#include "descriptorsetlayout.h"

class PipelineLayout : public vk::UniquePipelineLayout {
public:
	PipelineLayout() = default;
	PipelineLayout(vk::Device device);

	void addDescriptorSetLayout(vk::DescriptorSetLayout layout);
	void addPushConstantRange(vk::PushConstantRange range);

	void create();

	std::vector<vk::DescriptorSetLayout> const &getDescriptorSetLayouts() const;

private:
	vk::Device mDevice;
	std::vector<vk::DescriptorSetLayout> mDescriptorSetLayouts;
	std::vector<vk::PushConstantRange> mPushConstantRanges;
};
