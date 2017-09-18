#include "fullscreensubpass.h"
#include "../../Vulkan/structhelper.h"

FullScreenSubPass::FullScreenSubPass(vk::Device device) :
	mDevice(device),
	mDescriptorPool(device),
	mDescriptorSetLayout(device),
	mPipeline(device),
	mPushConstantData(128)
{
	addColorAttachment(0);
}

void FullScreenSubPass::setFragmentStage(std::string filename)
{
	mFragmentStage = filename;
}

void FullScreenSubPass::addUniformBuffer(vk::DescriptorBufferInfo info, uint32_t binding)
{
	mDescriptorSetLayout.addBinding(LayoutBinding::uniformBuffer(binding, 1, vk::ShaderStageFlagBits::eFragment));

	mBufferInfos.emplace_back(std::make_tuple(info, binding, vk::DescriptorType::eUniformBuffer));
	mDescriptorPool.addPoolSize(DescriptorPoolSize::uniformBuffer());
}

void FullScreenSubPass::addSampler(vk::DescriptorImageInfo info, uint32_t binding)
{
	mDescriptorSetLayout.addBinding(LayoutBinding::combinedImage(binding, 1, vk::ShaderStageFlagBits::eFragment));

	mImageInfos.emplace_back(std::make_tuple(info, binding, vk::DescriptorType::eCombinedImageSampler));
	mDescriptorPool.addPoolSize(DescriptorPoolSize::combinedImage());
}

void FullScreenSubPass::addStorageImage(vk::DescriptorImageInfo info, uint32_t binding)
{
	mDescriptorSetLayout.addBinding(LayoutBinding::storageImage(binding, 1, vk::ShaderStageFlagBits::eFragment));
	mImageInfos.emplace_back(std::make_tuple(info, binding, vk::DescriptorType::eStorageImage));
	mDescriptorPool.addPoolSize(DescriptorPoolSize::storageImage());
}

void FullScreenSubPass::addPushConstant(vk::PushConstantRange range, const void * data)
{
	mPushConstantRanges << range;
	auto begin = static_cast<const char*>(data);
	auto end = begin + range.size;
	std::copy(begin, end, mPushConstantData.begin() + range.offset);
}

void FullScreenSubPass::create(vk::RenderPass renderPass, vk::Extent2D extent, uint32_t index)
{
	mDescriptorSetLayout.create();
	mPipelineLayout = PipelineLayoutBuilder::build(mDevice, { mDescriptorSetLayout }, mPushConstantRanges);
	mPipeline = PipelineBuilder::buildFullScreenPipeline(mDevice, renderPass, extent, mPipelineLayout, mFragmentStage);

	mDescriptorPool.create(1);
	mDescriptorSet = mDescriptorPool.allocate(mDescriptorSetLayout);

	std::vector<vk::WriteDescriptorSet> writeDescriptorSets;
	for (const auto &bi : mBufferInfos)
		writeDescriptorSets << StructHelper::writeDescriptorSet(mDescriptorSet, std::get<1>(bi), &std::get<0>(bi), std::get<2>(bi));

	for (const auto &ii : mImageInfos)
		writeDescriptorSets << StructHelper::writeDescriptorSet(mDescriptorSet, std::get<1>(ii), &std::get<0>(ii), std::get<2>(ii));

	mDevice.updateDescriptorSets(writeDescriptorSets, {});
}

void FullScreenSubPass::execute(vk::CommandBuffer cmd) {
	cmd.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, mPipelineLayout, 0, mDescriptorSet, {});
	cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, mPipeline);

	for (auto r : mPushConstantRanges)
		cmd.pushConstants(mPipelineLayout, r.stageFlags, r.offset, r.size, &mPushConstantData[r.offset]);

	cmd.draw(4, 1, 0, 0);
}
