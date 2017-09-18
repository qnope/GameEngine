#include "materialmanager.h"
#include "../Tools/stream.h"

MaterialManager::MaterialManager(vk::Device device, BufferFactory &bufferFactory, ImageFactory &imageFactory) :
	mDevice(device),
	mBufferFactory(bufferFactory),
	mImageFactory(imageFactory),
	mAlbedoColorBuffer(bufferFactory.createEmptyBuffer(sizeof(AlbedoColor) * 1000, vk::BufferUsageFlagBits::eUniformBuffer | vk::BufferUsageFlagBits::eTransferDst, true)),
	mAlbedoColorStagingBuffer(bufferFactory.createEmptyBuffer(sizeof(AlbedoColor), vk::BufferUsageFlagBits::eTransferSrc, false))
{
	MaterialKind onlyAlbedoColor;
	onlyAlbedoColor.mColorAlbedo = true;
	mMaterialKinds << onlyAlbedoColor;

	mDescriptorSets.resize(2);
	mMaterialDescriptors.resize(2);

	mDescriptorPools.emplace_back(DescriptorPoolBuilder::monoDynamicUniformBuffer(mDevice, 1));
	mDescriptorPools.emplace_back(DescriptorPoolBuilder::monoCombinedSampler(mDevice, 10));

#define flags vk::ShaderStageFlagBits::eFragment
	mDescriptorSetLayouts.emplace_back(DescriptorSetLayoutBuilder::monoUniformBufferDynamic(mDevice, flags));
	mDescriptorSetLayouts.emplace_back(DescriptorSetLayoutBuilder::monoCombinedImage(mDevice, flags));
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

		mMaterialDescriptors[kind] << materialDescriptor;
	}

	return indices;
}

vk::DescriptorSet MaterialManager::allocateDescriptorSet(uint32_t kind)
{
	// kind == 0 : no need to reallocate

	if (kind == 1)
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

	if (kind == 1) {
		drawer.materialSet = mDescriptorSets[kind][whichSet];
		drawer.offsetAlbedoColor = ~(0u);
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

uint32_t MaterialManager::getMaterialKindIndex(const Material & material)
{
	if (material.useAlbedoTexture == false)
		return 0;

	else if (material.useAlbedoTexture == true)
		return 1;

	else
		assert("Bad material");
	return ~0u;
}
