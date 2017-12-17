#pragma once
#include "instance.h"

struct QueueFamilyIndices {
    int graphicFamily = -1;
    int computeFamily = -1;
    int transferFamily = -1;
    int presentFamily = -1;

    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;

    bool isComplete() {
        return graphicFamily >= 0 && computeFamily >= 0 && transferFamily >= 0 && presentFamily >= 0;
    }
};

class Device : public vk::UniqueDevice {
public:
    Device(Instance &instance);

    vk::Queue getGraphicQueue() const;
    vk::Queue getComputeQueue() const;
    vk::Queue getTransferQueue() const;
    vk::Queue getPresentQueue() const;

    vk::PhysicalDevice getPhysicalDevice() const;
    Instance &getInstance();
    QueueFamilyIndices getQueueFamilyIndices() const;

    uint32_t getTimestampTime() const;

private:
    Instance &mInstance;
    QueueFamilyIndices mQueueFamilyIndices;
    vk::Queue mGraphicQueue, mComputeQueue, mTransferQueue, mPresentQueue;
    vk::PhysicalDevice mPhysicalDevice;
    uint32_t mOneTimestampInNanoseconds;
};