#pragma once

#include "device.h"

class Sampler : public vk::UniqueSampler{
public:
	Sampler(vk::Device device, const vk::SamplerCreateInfo &info);

	static vk::SamplerCreateInfo simple(vk::Filter filter = vk::Filter::eLinear,
		vk::SamplerMipmapMode mipmapFilter = vk::SamplerMipmapMode::eLinear,
		vk::SamplerAddressMode address = vk::SamplerAddressMode::eRepeat,
		bool anisotropy = false, float maxAnisotropy = 0.0f,
		bool compareEnable = false, vk::CompareOp compareOp = vk::CompareOp::eLess,
		float minLod = 0.0f, float maxLod = VK_REMAINING_MIP_LEVELS,
		vk::BorderColor borderColor = vk::BorderColor::eFloatTransparentBlack,
		bool unnormalizedCoordinates = false);
};