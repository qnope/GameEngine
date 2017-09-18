#pragma once

#include "vulkan.h"

struct SDL_Surface;
class ImageLoader
{
public:
	ImageLoader(std::string const &path);

	uint32_t getWidth() const;
	uint32_t getHeight() const;
	uint32_t getRowPitch() const;
	vk::Format getFormat() const;

	unsigned char const *getPixel() const;

	~ImageLoader();

private:
	SDL_Surface *mImage;
};
