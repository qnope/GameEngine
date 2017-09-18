#include "descriptorsetlayout.h"
#include "../Tools/stream.h"

DescriptorSetLayout::DescriptorSetLayout(vk::Device device) : mDevice(device)
{
}

void DescriptorSetLayout::addBinding(vk::DescriptorSetLayoutBinding binding)
{
	mBindings << binding;
}

void DescriptorSetLayout::create() {
	vk::DescriptorSetLayoutCreateInfo layoutInfo;
	layoutInfo.bindingCount = (uint32_t)mBindings.size();
	layoutInfo.pBindings = mBindings.data();

	static_cast<vk::UniqueDescriptorSetLayout&>(*this) = mDevice.createDescriptorSetLayoutUnique(layoutInfo);
}