#pragma once

#include "../Descriptor/descriptorpoolbuilder.h"
#include "../Descriptor/descriptorsetlayoutbuilder.h"
#include "../Transfer/imagefactory.h"
#include "../Transfer/bufferfactory.h"
#include "materialkind.h"
#include "ModelImporter/material.h"
#include "drawer.h"

struct AlbedoColor {
	glm::vec4 albedoColor; // 16
	glm::vec4 padding[15];
};

static_assert(sizeof(AlbedoColor) % 256 == 0, "Albedo color must be 256 bytes aligned");

struct MaterialDescriptor {
	std::shared_ptr<CombinedImage> albedoTexture;
	glm::vec3 albedoColor;
	uint32_t offset;
};

class MaterialManager {
public:
	using MaterialIndex = std::tuple<uint32_t, uint32_t>;
	MaterialManager(vk::Device device, BufferFactory &bufferFactory, ImageFactory &imageFactory);

	void fillPipelines(const std::vector<vk::Pipeline> &pipelines);
	void fillPipelineLayouts(const std::vector<vk::PipelineLayout> &pipelineLayouts);

	// key = base materialIndex, value = {materialKindIndex, materialDescriptorIndex}
	std::vector<MaterialIndex> addMaterials(const std::vector<Material> &material);

	vk::DescriptorSet allocateDescriptorSet(uint32_t kind);

	// get pipeline, layout, set, and eventually the offset in function of materialIndex
	Drawer getMaterialDrawerValues(MaterialIndex index);

	vk::DescriptorSetLayout getAlbedoColorDescriptorSetLayout() const;
	vk::DescriptorSetLayout getAlbedoTextureDescriptorSetLayout() const;

private:
	vk::Device mDevice;
	BufferFactory &mBufferFactory;
	ImageFactory &mImageFactory;
	std::vector<DescriptorPoolWrapper> mDescriptorPools;
	std::vector<DescriptorSetLayout> mDescriptorSetLayouts;
	Buffer mAlbedoColorBuffer;
	Buffer mAlbedoColorStagingBuffer;
	std::vector<MaterialKind> mMaterialKinds;
	std::vector<std::vector<vk::DescriptorSet>> mDescriptorSets;
	std::vector<std::vector<MaterialDescriptor>> mMaterialDescriptors;
	std::vector<vk::Pipeline> mPipelines;
	std::vector<vk::PipelineLayout> mPipelineLayouts;

	uint32_t getMaterialKindIndex(const Material &material);
};
