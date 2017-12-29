#include "AlbedoColorMaterialManager.h"
#include "Descriptor/DescriptorPoolBuilder.h"
#include "Descriptor/DescriptorSetLayoutBuilder.h"

AlbedoColorMaterialManager::AlbedoColorMaterialManager(vk::Device device, BufferFactory & bufferFactory) :
    AbstractUniqueMaterialManager(device, "../Shaders/MaterialFunctionDefinition/AlbedoColor.frag"),
    mBufferFactory(bufferFactory),
    mStagingBuffer(bufferFactory.createEmptyBuffer(sizeof(Descriptor), vk::BufferUsageFlagBits::eTransferSrc, false)),
    mBuffer(bufferFactory.createEmptyBuffer(sizeof(Descriptor) * 1000, vk::BufferUsageFlagBits::eUniformBuffer | vk::BufferUsageFlagBits::eTransferDst, true))
{
    mDescriptorPool = std::make_unique<DescriptorPoolWrapper>(DescriptorPoolBuilder::monoDynamicUniformBuffer(device, 1));
    mDescriptorSetLayout = std::make_unique<DescriptorSetLayout>(DescriptorSetLayoutBuilder::monoUniformBufferDynamic(device, vk::ShaderStageFlagBits::eFragment));
    mDescriptorSet = mDescriptorPool->allocate(*mDescriptorSetLayout);

    vk::DescriptorBufferInfo ubo(mBuffer, 0, VK_WHOLE_SIZE);
    auto writeUbo = StructHelper::writeDescriptorSet(mDescriptorSet, 0, &ubo, vk::DescriptorType::eUniformBufferDynamic);
    mDevice.updateDescriptorSets({ writeUbo }, {});
}

bool AlbedoColorMaterialManager::isAccepted(Material material) const
{
    return material.isOnlyAlbedoColor();
}

void AlbedoColorMaterialManager::registerMaterial(Material material)
{
    Descriptor descriptor;

    descriptor.color = glm::vec4(material.albedoColor, 1.0);

    mStagingBuffer.resetOffset();
    mStagingBuffer.push_data(descriptor);
    mBufferFactory.transfer(mStagingBuffer, mBuffer, 0, mNumberMaterialRegistered * sizeof(Descriptor), sizeof(Descriptor));

    mValues.emplace_back(descriptor);
}

void AlbedoColorMaterialManager::getDrawerDescriptorSet(Drawer & drawer, const MaterialPointer & ptr) const
{
    drawer.materialSet = mDescriptorSet;
    drawer.offsetMaterialUbo = ptr.index * sizeof(Descriptor);
}
