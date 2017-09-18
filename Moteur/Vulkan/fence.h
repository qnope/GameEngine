#pragma once

#include "device.h"

class Fence : public vk::UniqueFence {
public:
	Fence() = default;
	Fence(vk::Device device, bool signaled = false);

	void reset();
	void wait();
private:
	vk::Device mDevice;
};