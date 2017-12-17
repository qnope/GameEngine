#include "pbrnormaltexturematerialmanager.h"
#include "../../Descriptor/descriptorpoolbuilder.h"
#include "../../Descriptor/descriptorsetlayoutbuilder.h"

PBRNormalTextureMaterialManager::PBRNormalTextureMaterialManager(vk::Device device, ImageFactory & imageFactory) :
    AbstractUniqueMaterialManager(device),
    mImageFactory(imageFactory)
{
    mDescriptorPool = std::make_unique<DescriptorPoolWrapper>(DescriptorPoolBuilder::normalPBRTexture(device, 10));
    mDescriptorSetLayout = std::make_unique<DescriptorSetLayout>(DescriptorSetLayoutBuilder::normalPBRTexture(device, vk::ShaderStageFlagBits::eFragment));
}

bool PBRNormalTextureMaterialManager::isAccepted(Material material) const
{
    return material.isNormalPBRTexture();
}

AbstractUniqueMaterialManager::MaterialPointer PBRNormalTextureMaterialManager::addMaterial(Material material)
{
    MaterialPointer p;

    p.ptr = this;
    p.index = mValues.size();

    Descriptor descriptor;
    auto descriptorSet = mDescriptorSets.emplace_back(mDescriptorPool->allocate(*mDescriptorSetLayout));

    descriptor.albedoTexture = std::make_shared<CombinedImage>(mImageFactory.loadImage(material.albedoTexturePath, true, true));
    descriptor.normalTexture = std::make_shared<CombinedImage>(mImageFactory.loadImage(material.normalTexturePath, true, true));
    descriptor.roughnessTexture = std::make_shared<CombinedImage>(mImageFactory.loadImage(material.roughnessTexturePath, true, true));
    descriptor.metallicTexture = std::make_shared<CombinedImage>(mImageFactory.loadImage(material.metallicTexturePath, true, true));

    vk::DescriptorImageInfo albedoInfo(*descriptor.albedoTexture, *descriptor.albedoTexture, vk::ImageLayout::eShaderReadOnlyOptimal);
    vk::DescriptorImageInfo normalInfo(*descriptor.normalTexture, *descriptor.normalTexture, vk::ImageLayout::eShaderReadOnlyOptimal);
    vk::DescriptorImageInfo roughnessInfo(*descriptor.roughnessTexture, *descriptor.roughnessTexture, vk::ImageLayout::eShaderReadOnlyOptimal);
    vk::DescriptorImageInfo metallicInfo(*descriptor.metallicTexture, *descriptor.metallicTexture, vk::ImageLayout::eShaderReadOnlyOptimal);

    mDevice.updateDescriptorSets({ StructHelper::writeDescriptorSet(descriptorSet, 0, &albedoInfo), StructHelper::writeDescriptorSet(descriptorSet, 1, &normalInfo), StructHelper::writeDescriptorSet(descriptorSet, 2, &roughnessInfo), StructHelper::writeDescriptorSet(descriptorSet, 3, &metallicInfo) }, {});

    mValues.emplace_back(descriptor);

    return p;
}

void PBRNormalTextureMaterialManager::getDrawerMaterialValues(Drawer & drawer, const MaterialPointer & ptr) const
{
    AbstractUniqueMaterialManager::getDrawerMaterialValues(drawer, ptr);
    drawer.materialSet = mDescriptorSets[ptr.index];
}
