#include "imageview.h"
#include "structhelper.h"

ImageView::ImageView(Device & device, vk::ImageViewCreateInfo const & info)
{
	static_cast<vk::UniqueImageView&>(*this) = device->createImageViewUnique(info);
}

vk::ImageViewCreateInfo ImageView::depth2D(vk::Image image, vk::Format fmt)
{
	vk::ImageViewCreateInfo info;
	info.components = ImageSubresource::identityMapping();
	info.subresourceRange = ImageSubresource::rangeDepth();
	info.image = image;
	info.viewType = vk::ImageViewType::e2D;
	info.format = fmt;
	return info;
}

vk::ImageViewCreateInfo ImageView::color2D(vk::Image image, vk::Format fmt)
{
	vk::ImageViewCreateInfo info;
	info.components = ImageSubresource::identityMapping();
	info.subresourceRange = ImageSubresource::rangeColor();
	info.image = image;
	info.viewType = vk::ImageViewType::e2D;
	info.format = fmt;
	return info;
}

vk::ImageViewCreateInfo ImageView::color3D(vk::Image image, vk::Format fmt)
{
	vk::ImageViewCreateInfo info;
	info.components = ImageSubresource::identityMapping();
	info.subresourceRange = ImageSubresource::rangeColor();
	info.image = image;
	info.viewType = vk::ImageViewType::e3D;
	info.format = fmt;
	return info;
}

vk::ImageViewCreateInfo ImageView::colorMipmapSelection3D(vk::Image image, uint32_t mipmap, vk::Format fmt)
{
	vk::ImageViewCreateInfo info;
	info.components = ImageSubresource::identityMapping();
	info.subresourceRange = vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, mipmap, 1, 0, VK_REMAINING_ARRAY_LAYERS);
	info.image = image;
	info.viewType = vk::ImageViewType::e3D;
	info.format = fmt;
	return info;
}

