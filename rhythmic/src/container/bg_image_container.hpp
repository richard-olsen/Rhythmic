#ifndef BACKGROUND_IMAGE_CONTAINER_HPP_
#define BACKGROUND_IMAGE_CONTAINER_HPP_

#include "bg_image.hpp"

namespace Rhythmic
{
	namespace BGContainer
	{
		void Initialize();
		void Dispose();

		void Recache();

		/*
		Returns the background object itself with the given handle

		Will return NULL if the handle isn't valid
		*/
		StageBackgroundIMG *GetBackground(BackgroundIMGHandle index);
		/*
		Returns a handle to the background with the given name
		*/
		BackgroundIMGHandle FindBackground(const std::string &name);

		/*
		Returns the next valid background handle. Useful for switching backgrounds
		*/
		BackgroundIMGHandle GetNextValidHandle(BackgroundIMGHandle handle);
		/*
		Returns the previous valid background handle. Useful for switching backgrounds
		*/
		BackgroundIMGHandle GetPrevValidHandle(BackgroundIMGHandle handle);
	}
}

#endif
