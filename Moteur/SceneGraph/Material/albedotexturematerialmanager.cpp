#include "AlbedoTextureMaterialManager.h"
#include "Descriptor/DescriptorPoolBuilder.h"
#include "Descriptor/DescriptorSetLayoutBuilder.h"

AlbedoTextureMaterialManager::AlbedoTextureMaterialManager(vk::Device device, ImageFactory & imageFactory) :
    AbstractUniqueMaterialManager(device, "../Shaders/MaterialFunctionDefinition/AlbedoTexture.frag"),
    mImageFactory(imageFactory)
{
    mDescriptorPool = std::make_unique<DescriptorPoolWrapper>(DescriptorPoolBuilder::monoCombinedSampler(device, 10));
    mDescriptorSetLayout = std::make_unique<DescriptorSetLayout>(DescriptorSetLayoutBuilder::monoCombinedImage(device, vk::ShaderStageFlagBits::eFragment));
}

bool AlbedoTextureMaterialManager::isAccepted(Material material) const
{
    return material.isOnlyAlbedoTexture();
}

void AlbedoTextureMaterialManager::registerMaterial(Material material)
{
    Descriptor descriptor;
    auto descriptorSet = mDescriptorSets.emplace_back(mDescriptorPool->allocate(*mDescriptorSetLayout));

    descriptor.albedoTexture = std::make_shared<CombinedImage>(mImageFactory.loadImage(material.albedoTexturePath, true, true));

    vk::DescriptorImageInfo albedoInfo(*descriptor.albedoTexture, *descriptor.albedoTexture, vk::ImageLayout::eShaderReadOnlyOptimal);
    mDevice.updateDescriptorSets({ StructHelper::writeDescriptorSet(descriptorSet, 0, &albedoInfo) }, {});
    mValues.emplace_back(descriptor);
}

void AlbedoTextureMaterialManager::getDrawerDescriptorSet(Drawer & drawer, const MaterialPointer & ptr) const
{
    drawer.materialSet = mDescriptorSets[ptr.index];
}
