#include "materialmanager.h"
#include "../Tools/stream.h"

MaterialManager::MaterialManager(vk::Device device, BufferFactory &bufferFactory, ImageFactory &imageFactory) :
	mDevice(device),
	mBufferFactory(bufferFactory),
	mImageFactory(imageFactory),
	mAlbedoColorBuffer(bufferFactory.createEmptyBuffer(sizeof(AlbedoColor) * 1000, vk::BufferUsageFlagBits::eUniformBuffer | vk::BufferUsageFlagBits::eTransferDst, true)),
	mAlbedoColorStagingBuffer(bufferFactory.createEmptyBuffer(sizeof(AlbedoColor), vk::BufferUsageFlagBits::eTransferSrc, false))
{
	mDescriptorSets.resize(3);
	mMaterialDescriptors.resize(3);

	mDescriptorPools.emplace_back(DescriptorPoolBuilder::monoDynamicUniformBuffer(mDevice, 1));
	mDescriptorPools.emplace_back(DescriptorPoolBuilder::monoCombinedSampler(mDevice, 10));
	mDescriptorPools.emplace_back(DescriptorPoolBuilder::PBRTexture(mDevice, 10));

#define flags vk::ShaderStageFlagBits::eFragment
	mDescriptorSetLayouts.emplace_back(DescriptorSetLayoutBuilder::monoUniformBufferDynamic(mDevice, flags));
	mDescriptorSetLayouts.emplace_back(DescriptorSetLayoutBuilder::monoCombinedImage(mDevice, flags));
	mDescriptorSetLayouts.emplace_back(DescriptorSetLayoutBuilder::PBRTexture(mDevice, flags));
#undef flags

	mDescriptorSets[0] << mDescriptorPools[0].allocate(mDescriptorSetLayouts[0]);

	vk::DescriptorBufferInfo uboAlbedoColor(mAlbedoColorBuffer, 0, sizeof(AlbedoColor));
	device.updateDescriptorSets(StructHelper::writeDescriptorSet(mDescriptorSets[0][0], 0, &uboAlbedoColor, vk::DescriptorType::eUniformBufferDynamic), {});
}

void MaterialManager::fillPipelines(const std::vector<vk::Pipeline>& pipelines)
{
	mPipelines = pipelines;
}

void MaterialManager::fillPipelineLayouts(const std::vector<vk::PipelineLayout>& pipelineLayouts)
{
	mPipelineLayouts = pipelineLayouts;
}

std::vector<MaterialManager::MaterialIndex> MaterialManager::addMaterials(const std::vector<Material>& material)
{
	std::vector<std::tuple<uint32_t, uint32_t>> indices;
	indices.reserve(material.size());

	for (const auto &m : material) {
		auto kind = getMaterialKindIndex(m);
		auto indice = (uint32_t)mMaterialDescriptors[kind].size();
		indices << std::make_tuple(kind, indice);

		MaterialDescriptor materialDescriptor;
		auto descriptorSet = allocateDescriptorSet(kind);

		if (kind == 0) {
			mAlbedoColorStagingBuffer.resetOffset();
			mAlbedoColorStagingBuffer.push_data(m.albedoColor);
			mBufferFactory.transfer(mAlbedoColorStagingBuffer, mAlbedoColorBuffer, 0, indice * sizeof(AlbedoColor), sizeof(AlbedoColor));

			materialDescriptor.albedoColor = m.albedoColor;
			materialDescriptor.offset = indice * sizeof(AlbedoColor);
		}

		else if (kind == 1) {
			materialDescriptor.albedoTexture = std::make_shared<CombinedImage>(mImageFactory.loadImage(m.albedoTexturePath, true, true));

			vk::DescriptorImageInfo imageInfo(*materialDescriptor.albedoTexture, *materialDescriptor.albedoTexture, vk::ImageLayout::eShaderReadOnlyOptimal);
			mDevice.updateDescriptorSets(StructHelper::writeDescriptorSet(descriptorSet, 0, &imageInfo), {});
		}

		else if (kind == 2) {
			materialDescriptor.albedoTexture = std::make_shared<CombinedImage>(mImageFactory.loadImage(m.albedoTexturePath, true, true));
			materialDescriptor.normalTexture = std::make_shared<CombinedImage>(mImageFactory.loadImage(m.normalTexturePath, true, true));
			materialDescriptor.roughnessTexture = std::make_shared<CombinedImage>(mImageFactory.loadImage(m.roughnessTexturePath, true, true));
			materialDescriptor.metallicTexture = std::make_shared<CombinedImage>(mImageFactory.loadImage(m.metallicTexturePath, true, true));

			vk::DescriptorImageInfo albedoInfo(*materialDescriptor.albedoTexture, *materialDescriptor.albedoTexture, vk::ImageLayout::eShaderReadOnlyOptimal);
			vk::DescriptorImageInfo normalInfo(*materialDescriptor.normalTexture, *materialDescriptor.normalTexture, vk::ImageLayout::eShaderReadOnlyOptimal);
			vk::DescriptorImageInfo roughnessInfo(*materialDescriptor.roughnessTexture, *materialDescriptor.roughnessTexture, vk::ImageLayout::eShaderReadOnlyOptimal);
			vk::DescriptorImageInfo metallicInfo(*materialDescriptor.metallicTexture, *materialDescriptor.metallicTexture, vk::ImageLayout::eShaderReadOnlyOptimal);

			mDevice.updateDescriptorSets({ StructHelper::writeDescriptorSet(descriptorSet, 0, &albedoInfo), StructHelper::writeDescriptorSet(descriptorSet, 1, &normalInfo), StructHelper::writeDescriptorSet(descriptorSet, 2, &roughnessInfo), StructHelper::writeDescriptorSet(descriptorSet, 3, &metallicInfo) }, {});
		}

		mMaterialDescriptors[kind] << materialDescriptor;
	}

	return indices;
}

vk::DescriptorSet MaterialManager::allocateDescriptorSet(uint32_t kind)
{
	// kind == 0 : no need to reallocate

	if (kind == 1 || kind == 2)
		mDescriptorSets[kind] << mDescriptorPools[kind].allocate(mDescriptorSetLayouts[kind]);

	return mDescriptorSets[kind].back();
}

Drawer MaterialManager::getMaterialDrawerValues(MaterialIndex index)
{
	Drawer drawer;
	auto kind = std::get<0>(index);
	auto whichSet = std::get<1>(index);

	drawer.pipelineLayout = mPipelineLayouts[kind];
	drawer.pipeline = mPipelines[kind];

	if (kind == 0) {
		drawer.materialSet = mDescriptorSets[kind][0];
		drawer.offsetAlbedoColor = mMaterialDescriptors[kind][whichSet].offset;
	}

	else if (kind == 1) {
		drawer.materialSet = mDescriptorSets[kind][whichSet];
		drawer.offsetAlbedoColor = ~(0u);
	}

	else if (kind == 2) {
		drawer.offsetAlbedoColor = ~(0u);
		drawer.materialSet = mDescriptorSets[kind][whichSet];
	}

	return drawer;
}

vk::DescriptorSetLayout MaterialManager::getAlbedoColorDescriptorSetLayout() const
{
	return mDescriptorSetLayouts[0];
}

vk::DescriptorSetLayout MaterialManager::getAlbedoTextureDescriptorSetLayout() const
{
	return mDescriptorSetLayouts[1];
}

vk::DescriptorSetLayout MaterialManager::getPBRTextureDescriptorSetLayout() const
{
	return mDescriptorSetLayouts[2];
}

uint32_t MaterialManager::getMaterialKindIndex(const Material & material)
{
	if (material.useAlbedoTexture == false)
		return 0; // Albedo Color

	else if (material.useAlbedoTexture == true) {
		if (material.useMetallicTexture && material.useNormalTexture && material.useRoughnessTexture)
			return 2; // PBR Texture

		return 1; // Albedo texture
	}

	else
		assert("Bad material");
	return ~0u;
}
