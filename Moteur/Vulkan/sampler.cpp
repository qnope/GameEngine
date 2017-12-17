#include "sampler.h"

Sampler::Sampler(vk::Device device, const vk::SamplerCreateInfo & info)
{
    static_cast<vk::UniqueSampler&>(*this) = device.createSamplerUnique(info);
}

vk::SamplerCreateInfo Sampler::simple(vk::Filter filter, vk::SamplerMipmapMode mipmapFilter, vk::SamplerAddressMode address, bool anisotropy, float maxAnisotropy, bool compareEnable, vk::CompareOp compareOp, float minLod, float maxLod, vk::BorderColor borderColor, bool unnormalizedCoordinates)
{
    vk::SamplerCreateInfo info;

    info.magFilter = info.minFilter = filter;
    info.mipmapMode = mipmapFilter;
    info.addressModeU = info.addressModeV = info.addressModeW = address;
    info.anisotropyEnable = anisotropy;
    info.maxAnisotropy = maxAnisotropy;
    info.compareEnable = compareEnable;
    info.compareOp = compareOp;
    info.minLod = minLod;
    info.maxLod = maxLod;
    info.borderColor = borderColor;
    info.unnormalizedCoordinates = unnormalizedCoordinates;

    return info;
}