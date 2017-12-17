#pragma once

#include "device.h"

class DescriptorSetLayout : public vk::UniqueDescriptorSetLayout {
public:
    DescriptorSetLayout(vk::Device device);

    void addBinding(vk::DescriptorSetLayoutBinding binding);

    void create();

private:
    vk::Device mDevice;
    std::vector<vk::DescriptorSetLayoutBinding> mBindings;
};