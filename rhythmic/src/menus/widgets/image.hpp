#ifndef RHYTHMIC_WIDGET_IMAGE_HPP_
#define RHYTHMIC_WIDGET_IMAGE_HPP_

#include "../widget.hpp"

#include "../../rendering/texture.hpp"

#include "glm/vec2.hpp"

namespace Rhythmic
{
	class WidgetImage : public WidgetBase
	{
	public:
		/*
		Constructor for the image widget in the menus
		Image -> The image structure
		Flags -> Passed when creating the texture
		UseAsBackground -> A quick and easy way to scale the image to fit the window
		*/
		WidgetImage(const IO_Image &image, int flags, bool useAsBackground = false, bool allowScaling = true);
		virtual ~WidgetImage();

		virtual void Update(float delta);
		virtual void Render(float interpolation);

		void SetImage(const IO_Image &image, int flags);
	private:
		Rendering::Texture m_image;

		bool m_allowScaling;

		glm::vec2 m_invImageSize;
	};
}

#endif

