#include "imguipass.h"
#include "../../Vulkan/input.h"

ImGUIPass::ImGUIPass(ImGUIInstance &imGUIInstance, Device &device, vk::Format format, vk::Extent2D extent, BufferFactory &bufferFactory) :
	mImGUIInstance(imGUIInstance),
	mDevice(device),
	mTransientCommandPool(device, device.getQueueFamilyIndices().graphicFamily, true, true),
	mCommandBuffer(std::move(mTransientCommandPool.allocate(vk::CommandBufferLevel::ePrimary, 1)[0]))
{
	std::tie(mRenderPass, mSubpass) = RenderPassBuilder::imgui(imGUIInstance, device, format, extent, bufferFactory, vk::ImageLayout::eShaderReadOnlyOptimal, vk::ImageLayout::ePresentSrcKHR);
}

void ImGUIPass::newFrame()
{
	ImGuiIO& io = ImGui::GetIO();
	static double g_Time = 0.0;

	// Setup display size (every frame to accommodate for window resizing)
	int w, h;
	int display_w, display_h;
	glfwGetWindowSize(mImGUIInstance.window, &w, &h);
	glfwGetFramebufferSize(mImGUIInstance.window, &display_w, &display_h);
	io.DisplaySize = ImVec2((float)w, (float)h);
	io.DisplayFramebufferScale = ImVec2(w > 0 ? ((float)display_w / w) : 0, h > 0 ? ((float)display_h / h) : 0);

	// Setup time step
	double current_time = glfwGetTime();
	io.DeltaTime = g_Time > 0.0 ? (float)(current_time - g_Time) : (float)(1.0f / 60.0f);
	g_Time = current_time;

	// Setup inputs
	// (we already got mouse wheel, keyboard keys & characters from glfw callbacks polled in glfwPollEvents())
	auto &input = Input::instance();
	if (glfwGetWindowAttrib(mImGUIInstance.window, GLFW_FOCUSED))
	{
		io.MousePos = ImVec2{ (float)input.mouseXPos, (float)input.mouseYPos };   // Mouse position in screen coordinates (set to -1,-1 if no mouse / on another screen, etc.)
	}
	else
	{
		io.MousePos = ImVec2(-1, -1);
	}

	for (int i = 0; i < 3; i++)
	{
		io.MouseDown[i] = input.mouseButtonPressed[i] || glfwGetMouseButton(mImGUIInstance.window, i) != 0;    // If a mouse press event came, always pass it as "mouse held this frame", so we don't miss click-release events that are shorter than 1 frame.
		input.mouseButtonPressed[i] = false;
	}

	io.MouseWheel = (float)input.scrollYOffset;
	input.scrollYOffset = 0.0f;

	// Hide OS mouse cursor if ImGui is drawing it
	glfwSetInputMode(mImGUIInstance.window, GLFW_CURSOR, io.MouseDrawCursor ? GLFW_CURSOR_HIDDEN : GLFW_CURSOR_NORMAL);

	// Start the frame
	ImGui::NewFrame();
}

vk::CommandBuffer ImGUIPass::execute(const Framebuffer &framebuffer)
{
	ImGui::Render();

	mCommandBuffer->begin(StructHelper::beginCommand(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));

	mRenderPass.execute(mCommandBuffer, framebuffer, framebuffer.getExtent());

	mCommandBuffer->end();

	return mCommandBuffer;
}
