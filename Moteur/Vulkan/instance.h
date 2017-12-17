#pragma once
#include "../Window/window.h"
#include "vulkan.h"

class Instance : public vk::UniqueInstance {
public:
    Instance(std::string applicationName, std::string engineName, Window &window, bool debug);

    std::vector<char const *> const &getValidationLayers() const;

    void createSurfaceKHR();

    const Window &getWindow();
    vk::SurfaceKHR getSurfaceKHR() const;

    ~Instance();
private:
    Window &mWindow;
    vk::UniqueSurfaceKHR mSurfaceKHR;
    VkDebugReportCallbackEXT mCallback;
    bool mDebug;
    std::vector<char const *> mValidationLayers;
};
