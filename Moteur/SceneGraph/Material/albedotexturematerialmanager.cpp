#include "albedotexturematerialmanager.h"
#include "../../Descriptor/descriptorpoolbuilder.h"
#include "../../Descriptor/descriptorsetlayoutbuilder.h"

AlbedoTextureMaterialManager::AlbedoTextureMaterialManager(vk::Device device, ImageFactory & imageFactory) :
    AbstractUniqueMaterialManager(device),
    mImageFactory(imageFactory)
{
    mDescriptorPool = std::make_unique<DescriptorPoolWrapper>(DescriptorPoolBuilder::monoCombinedSampler(device, 10));
    mDescriptorSetLayout = std::make_unique<DescriptorSetLayout>(DescriptorSetLayoutBuilder::monoCombinedImage(device, vk::ShaderStageFlagBits::eFragment));
}

bool AlbedoTextureMaterialManager::isAccepted(Material material) const
{
    return material.isOnlyAlbedoTexture();
}

AbstractUniqueMaterialManager::MaterialPointer AlbedoTextureMaterialManager::addMaterial(Material material)
{
    MaterialPointer p;

    p.ptr = this;
    p.index = mValues.size();

    Descriptor descriptor;
    auto descriptorSet = mDescriptorSets.emplace_back(mDescriptorPool->allocate(*mDescriptorSetLayout));

    descriptor.albedoTexture = std::make_shared<CombinedImage>(mImageFactory.loadImage(material.albedoTexturePath, true, true));

    vk::DescriptorImageInfo albedoInfo(*descriptor.albedoTexture, *descriptor.albedoTexture, vk::ImageLayout::eShaderReadOnlyOptimal);

    mDevice.updateDescriptorSets({ StructHelper::writeDescriptorSet(descriptorSet, 0, &albedoInfo) }, {});

    mValues.emplace_back(descriptor);

    return p;
}

void AlbedoTextureMaterialManager::getDrawerMaterialValues(Drawer & drawer, const MaterialPointer & ptr) const
{
    AbstractUniqueMaterialManager::getDrawerMaterialValues(drawer, ptr);
    drawer.materialSet = mDescriptorSets[ptr.index];
}
