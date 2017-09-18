#include <iostream>
#include "../Tools/stream.h"
#include "device.h"

static constexpr float priority = 1.0f;

QueueFamilyIndices findQueueFamilies(const Instance &instance, vk::PhysicalDevice device) {
	QueueFamilyIndices indices;

	auto queues = device.getQueueFamilyProperties();

	for (uint32_t i = 0; i < queues.size(); ++i) {
		bool isUsed = false;
		if (queues[i].queueCount > 0) {
			if (queues[i].queueFlags & vk::QueueFlagBits::eGraphics && indices.graphicFamily == -1)
				indices.graphicFamily = i, isUsed = true;

			if (queues[i].queueFlags & vk::QueueFlagBits::eCompute && indices.computeFamily == -1)
				indices.computeFamily = i, isUsed = true;

			if (queues[i].queueFlags & vk::QueueFlagBits::eTransfer && indices.transferFamily == -1)
				indices.transferFamily = i, isUsed = true;

			if (device.getSurfaceSupportKHR(i, instance.getSurfaceKHR()) && indices.presentFamily == -1)
				indices.presentFamily = i, isUsed = true;

			if (isUsed) {
				vk::DeviceQueueCreateInfo info;
				info.queueCount = 1;
				info.pQueuePriorities = &priority;
				info.queueFamilyIndex = i;
				indices.queueCreateInfos << info;
			}

			if (indices.isComplete())
				return indices;
		}
	}

	throw std::runtime_error("Unable to find queues");
}

vk::PhysicalDevice whichDeviceIsSuitable(const Instance &instance, const std::vector<vk::PhysicalDevice> &physicalDevices) {
	for(auto &pd: physicalDevices)
	if (pd.getProperties().deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
		if(findQueueFamilies(instance, pd).isComplete())
			return pd;
	throw(std::runtime_error("Unable to find discrete GPU"));
}

Device::Device(Instance &instance) : mInstance(instance)
{
	instance.createSurfaceKHR();
	auto physicalDevices = instance.get().enumeratePhysicalDevices();
	mPhysicalDevice = whichDeviceIsSuitable(instance, physicalDevices);

	std::cout << "GPU find is : " << mPhysicalDevice.getProperties().deviceName << std::endl;

	mQueueFamilyIndices = findQueueFamilies(instance, mPhysicalDevice);

	vk::DeviceCreateInfo deviceInfo;

	vk::PhysicalDeviceFeatures features = mPhysicalDevice.getFeatures();
	mOneTimestampInNanoseconds = (uint32_t)mPhysicalDevice.getProperties().limits.timestampPeriod;

	decltype(auto) layers = instance.getValidationLayers();

	std::vector<const char*> extensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

	deviceInfo.enabledLayerCount = (uint32_t)layers.size();
	deviceInfo.ppEnabledLayerNames = layers.data();

	deviceInfo.pQueueCreateInfos = mQueueFamilyIndices.queueCreateInfos.data();
	deviceInfo.queueCreateInfoCount = (uint32_t)mQueueFamilyIndices.queueCreateInfos.size();

	deviceInfo.pEnabledFeatures = &features;

	deviceInfo.enabledExtensionCount = (uint32_t)extensions.size();
	deviceInfo.ppEnabledExtensionNames = extensions.data();

	static_cast<vk::UniqueDevice&>(*this) = mPhysicalDevice.createDeviceUnique(deviceInfo);

	mGraphicQueue = get().getQueue(mQueueFamilyIndices.graphicFamily, 0);
	mComputeQueue = get().getQueue(mQueueFamilyIndices.computeFamily, 0);
	mTransferQueue = get().getQueue(mQueueFamilyIndices.transferFamily, 0);
	mPresentQueue = get().getQueue(mQueueFamilyIndices.presentFamily, 0);
}

vk::Queue Device::getGraphicQueue() const
{
	return mGraphicQueue;
}

vk::Queue Device::getComputeQueue() const
{
	return mComputeQueue;
}

vk::Queue Device::getTransferQueue() const
{
	return mTransferQueue;
}

vk::Queue Device::getPresentQueue() const
{
	return mPresentQueue;
}

vk::PhysicalDevice Device::getPhysicalDevice() const
{
	return mPhysicalDevice;
}

Instance & Device::getInstance()
{
	return mInstance;
}

QueueFamilyIndices Device::getQueueFamilyIndices() const
{
	return mQueueFamilyIndices;
}

uint32_t Device::getTimestampTime() const
{
	return mOneTimestampInNanoseconds;
}
