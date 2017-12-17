// ImGui GLFW binding with Vulkan + shaders
// FIXME: Changes of ImTextureID aren't supported by this binding! Please, someone add it!

// You can copy and use unmodified imgui_impl_* files in your project. See main.cpp for an example of using this.
// If you use this binding you'll need to call 5 functions: ImGui_ImplXXXX_Init(), ImGui_ImplXXX_CreateFontsTexture(), ImGui_ImplXXXX_NewFrame(), ImGui_ImplXXXX_Render() and ImGui_ImplXXXX_Shutdown().
// If you are new to ImGui, see examples/README.txt and documentation at the top of imgui.cpp.
// https://github.com/ocornut/imgui

#include "imguiinstance.h"

#include "../Descriptor/descriptorpoolbuilder.h"

static const char* getClipboardCallback(void* user_data)
{
    return glfwGetClipboardString((GLFWwindow*)user_data);
}

static void setClipboardCallback(void* user_data, const char* text)
{
    glfwSetClipboardString((GLFWwindow*)user_data, text);
}

ImGUIInstance::ImGUIInstance(Device &device, Window &window, ImageFactory &imageFactory) :
    device(device),
    window(window)
{
    ImGuiIO& io = ImGui::GetIO();

    unsigned char* pixels;
    int width, height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
    int rowPitch = width * 4 * sizeof(char);
    size_t upload_size = height * rowPitch;
    std::vector<unsigned char> pixelsVector(upload_size);
    std::copy(pixels, pixels + upload_size, pixelsVector.begin());

    fontImage = std::make_unique<CombinedImage>(imageFactory.loadImage(pixelsVector, width, height, rowPitch, vk::Format::eR8G8B8A8Unorm, false, true));

    vk::DescriptorImageInfo imageInfo(*fontImage, *fontImage, vk::ImageLayout::eShaderReadOnlyOptimal);
    auto write = StructHelper::writeDescriptorSet(descriptorSet, 0, &imageInfo, vk::DescriptorType::eCombinedImageSampler);
    this->device.updateDescriptorSets(write, {});

    io.Fonts->TexID = (void*)(intptr_t)(VkImage)(vk::Image)(fontImage->image);

    io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;                         // Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array.
    io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
    io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
    io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
    io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
    io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
    io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
    io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
    io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
    io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
    io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
    io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
    io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
    io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
    io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
    io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
    io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
    io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
    io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;

    io.RenderDrawListsFn = nullptr;
    io.ClipboardUserData = this->window;
    io.SetClipboardTextFn = setClipboardCallback;
    io.GetClipboardTextFn = getClipboardCallback;
}

ImGUIInstance::~ImGUIInstance()
{
    ImGui::Shutdown();
}
