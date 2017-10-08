#pragma once

#include "descriptorpoolwrapper.h"
#include "../Tools/variadics.h"
#include "../Vulkan/structhelper.h"

struct DescriptorPoolBuilder {
private:
	// Types / Number by pool
	using PS = vk::DescriptorPoolSize;
	template<typename ...PSs>
	static DescriptorPoolWrapper build(vk::Device device, uint32_t numberSetByPool, PSs ... ps) {
		DescriptorPoolWrapper wrapper(device, numberSetByPool);
		EXPAND_FUNCTION(wrapper.addPoolSize(PS(ps.type, ps.descriptorCount * numberSetByPool)));
		return wrapper;
	}

public:
	static DescriptorPoolWrapper monoNonDynamicUniformBuffer(vk::Device device, uint32_t numberSetByPool) {
		return build(device, numberSetByPool, DescriptorPoolSize::uniformBuffer());
	}

	static DescriptorPoolWrapper monoDynamicUniformBuffer(vk::Device device, uint32_t numberSetByPool) {
		return build(device, numberSetByPool, DescriptorPoolSize::uniformBufferDynamic());
	}

	static DescriptorPoolWrapper monoCombinedSampler(vk::Device device, uint32_t numberSetByPool) {
		return build(device, numberSetByPool, DescriptorPoolSize::combinedImage());
	}

	static DescriptorPoolWrapper monoUniformBufferMonoStorageImage(vk::Device device, uint32_t numberSetByPool) {
		return build(device, numberSetByPool, DescriptorPoolSize::uniformBuffer(), DescriptorPoolSize::storageImage());
	}

	static DescriptorPoolWrapper voxelDownsampler(vk::Device device) {
		return build(device, 1, DescriptorPoolSize::uniformBuffer(), DescriptorPoolSize::combinedImage(), DescriptorPoolSize::storageImage());
	}

	static DescriptorPoolWrapper normalPBRTexture(vk::Device device, uint32_t numberSetByPool) {
		return build(device, numberSetByPool, DescriptorPoolSize::combinedImage(4)); // albedo, normal, roughness, metallic
	}
};