#include "fence.h"

Fence::Fence(vk::Device device, bool signaled) : mDevice(device)
{
	vk::FenceCreateInfo info;
	info.flags = (signaled) ? vk::FenceCreateFlagBits::eSignaled : vk::FenceCreateFlags();
	static_cast<vk::UniqueFence&>(*this) = mDevice.createFenceUnique(info);
}

void Fence::reset()
{
	mDevice.resetFences((vk::Fence)*this);
}

void Fence::wait()
{
	mDevice.waitForFences((vk::Fence)*this, true, std::numeric_limits<uint64_t>::max());
}
