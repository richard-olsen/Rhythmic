#include "io_image.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "../util/stb/stb_image.h"

#include <wge/core/logging.hpp>

#include <iostream>

namespace Rhythmic
{
	IO_Image::IO_Image() :
		width(0),
		height(0),
		component(0),
		data(0),
		fileName("")
	{ }
	
	void IO_LoadImage(const std::string &file, IO_Image *image)
	{
		image->data = stbi_load(file.c_str(), &image->width, &image->height, &image->component, 4);
		if (image->data == 0)
		{
			LOG_ERROR("Failed to load image: {0}", file);
		}
		else
		{
			LOG_TRACE("Image load successful: {0}", file);
		}
		image->fileName = file;
	}

	void IO_FreeImageData(IO_Image *image)
	{
		stbi_image_free(image->data);
	}
}