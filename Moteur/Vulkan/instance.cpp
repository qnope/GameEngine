#include "Instance.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <iostream>
#include <SDL2/SDL_image.h>
#include "Tools/stream.h"

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugReportFlagsEXT flags,
    VkDebugReportObjectTypeEXT objType,
    uint64_t obj,
    size_t location,
    int32_t code,
    const char* layerPrefix,
    const char* msg,
    void* userData) {

    std::cerr << "validation layer: " << msg << std::endl;

    return VK_FALSE;
}

auto getExtensions() {
    uint32_t count = 0;
    auto _extensions = glfwGetRequiredInstanceExtensions(&count);

    return std::vector<const char*>(_extensions, _extensions + count);
}

Instance::Instance(std::string applicationName, std::string engineName, Window &window, bool debug) : mWindow(window), mDebug(debug) {
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF | IMG_INIT_WEBP);
    vk::ApplicationInfo appInfo;
    appInfo.apiVersion = VK_MAKE_VERSION(1, 0, 54);
    appInfo.pApplicationName = applicationName.c_str();
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 54);
    appInfo.pEngineName = engineName.c_str();
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 54);

    std::vector<const char*> extensions = getExtensions();
    if (mDebug)
        extensions << VK_EXT_DEBUG_REPORT_EXTENSION_NAME;

    if (mDebug)
        mValidationLayers << "VK_LAYER_LUNARG_standard_validation" << "VK_LAYER_LUNARG_monitor";

    vk::InstanceCreateInfo info;
    info.pApplicationInfo = &appInfo;
    info.enabledExtensionCount = (uint32_t)extensions.size();
    info.ppEnabledExtensionNames = extensions.data();
    info.enabledLayerCount = (uint32_t)mValidationLayers.size();;
    info.ppEnabledLayerNames = mValidationLayers.data();

    static_cast<vk::UniqueInstance&>(*this) = vk::createInstanceUnique(info);

    std::cout << "Extensions are : \n" << extensions;
    std::cout << "Validations layers are : \n" << mValidationLayers;

    if (mDebug) {
        VkDebugReportCallbackCreateInfoEXT debugInfo;
        debugInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT | VK_DEBUG_REPORT_DEBUG_BIT_EXT;// | VK_DEBUG_REPORT_INFORMATION_BIT_EXT;
        debugInfo.pfnCallback = debugCallback;

        auto func = (PFN_vkCreateDebugReportCallbackEXT)this->get().getProcAddr("vkCreateDebugReportCallbackEXT");
        func(this->get(), &debugInfo, nullptr, &mCallback);
    }
}

std::vector<char const*> const &Instance::getValidationLayers() const
{
    return mValidationLayers;
}

void Instance::createSurfaceKHR()
{
    VkSurfaceKHR surface;
    glfwCreateWindowSurface(this->get(), mWindow, nullptr, &surface);
    vk::SurfaceKHRDeleter deleter(this->get());
    mSurfaceKHR = vk::UniqueHandle<vk::SurfaceKHR, vk::SurfaceKHRDeleter>(surface, deleter);
}

const Window & Instance::getWindow()
{
    return mWindow;
}

vk::SurfaceKHR Instance::getSurfaceKHR() const
{
    return mSurfaceKHR.get();
}

Instance::~Instance() {
    if (get() != vk::Instance()) {
        if (mDebug) {
            auto func = (PFN_vkDestroyDebugReportCallbackEXT)this->get().getProcAddr("vkDestroyDebugReportCallbackEXT");
            func(this->get(), mCallback, nullptr);
        }
        IMG_Quit();
        SDL_Quit();
    }
}
