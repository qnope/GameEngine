#include "descriptorpoolwrapper.h"
#include "../Tools/stream.h"

DescriptorPoolWrapper::DescriptorPoolWrapper(vk::Device device, uint32_t numberSetByPool) :
	mDevice(device),
	mNumberSetByPool(numberSetByPool)
{
}

std::vector<vk::DescriptorSet> DescriptorPoolWrapper::allocate(const std::vector<vk::DescriptorSetLayout> &layouts)
{
	// We must create another descriptorpool
	if (mNumberAllocatedDescriptorSets % mNumberSetByPool == 0) {
		mDescriptorPools.emplace_back(mDevice);

		for(const auto &ps : mDescriptorPoolSizes)
			mDescriptorPools.back().addPoolSize(ps);

		mDescriptorPools.back().create(mNumberSetByPool);
	}

	mNumberAllocatedDescriptorSets += (uint32_t)layouts.size();
	return mDescriptorPools.back().allocate(layouts);
}

std::vector<vk::DescriptorSet> DescriptorPoolWrapper::allocate(vk::DescriptorSetLayout layout, uint32_t number)
{
	return allocate(std::vector<vk::DescriptorSetLayout>(number, layout));
}

vk::DescriptorSet DescriptorPoolWrapper::allocate(vk::DescriptorSetLayout layout)
{
	return allocate((std::vector<vk::DescriptorSetLayout>{ {layout} }))[0];
}

void DescriptorPoolWrapper::addPoolSize(std::tuple<vk::DescriptorType, uint32_t> poolSize)
{
	const auto &[t, c] = poolSize;
	addPoolSize(vk::DescriptorPoolSize(t, c));
}

void DescriptorPoolWrapper::addPoolSize(vk::DescriptorPoolSize poolSize)
{
	mDescriptorPoolSizes << poolSize;
}
