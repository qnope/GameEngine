#pragma once
#include "imageview.h"
#include "../Transfer/imagefactory.h"

class Framebuffer : public vk::UniqueFramebuffer {
public:
	Framebuffer() = default;
	Framebuffer(vk::Device device, vk::RenderPass renderpass, std::vector<CombinedImage> &&images);
	Framebuffer(vk::Device device, vk::RenderPass renderpass, vk::Extent2D extent, const std::vector<vk::ImageView> &imageViews);
	Framebuffer(vk::Device device, vk::RenderPass renderpass, vk::Extent2D extent, vk::ImageView imageView);

	const CombinedImage &getCombinedImage(uint32_t index) const;

	vk::Extent2D getExtent() const;

private:
	std::vector<CombinedImage> mCombinedImages;
	vk::Extent2D mExtent;
};
