#ifndef IO_IMAGE_H_
#define IO_IMAGE_H_

#include <string>

namespace Rhythmic
{
	struct IO_Image 
	{
		int width;
		int height;
		int component;
		unsigned char *data;

		std::string fileName;

		IO_Image();
	};

	void IO_LoadImage(const std::string &file, IO_Image *image);
	void IO_FreeImageData(IO_Image *image);

} // namespace Rhythmic

#endif
