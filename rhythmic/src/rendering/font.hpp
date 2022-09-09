#ifndef _FONT_H_
#define _FONT_H_

#include <map>
#include <string>
#include <glm/vec3.hpp>
#include "sheet.hpp"
#include "../util/stb/stb_truetype.h"
#include "references.hpp"

#define FONT_TTF_RASTER_SCALE 64.0f

namespace Rhythmic
{
	struct FontChar
	{
		float x0, y0, x1, y1; // In a format for OpenGL
		float xsize, ysize;
		float xoff, yoff, xadvance;
	}; // Almost a direct copy of stbtt_bakedchar, but instead of shorts for texture position, it uses floats

	class Font
	{
	public:
		Font();
		~Font();

		bool CreateFontFromFile(const std::string &font);
		void DestroyFont();

		/*
		Returns size (in pixels) of the given string based on glyph data from the font
		Excludes special characters (such as \n or \r)
		*/
		float GetSize(const std::string &string, float scale);

		float GetScaledHeight();

		/*
		Draws a string at x,y with the size of scale using color.
		Alignment (0 left, 1 centered, 2 right)
		*/
		void DrawString(const std::string &string, float x, float y, float scale_x, float scale_y, const glm::vec4 &color, int alignment = 0, int cursorPos = -1);

		/*
		Draws a single character at x,y with the size of scale using color.
		*/
		void DrawChar(uint32_t c, float x, float y, float scale_x, float scale_y, const glm::vec4 &color);
	private:
		//bool				 m_usedTTF;
		//std::string			 m_charDictionary;
		FontChar			*m_chars;
		//SheetReference		 m_sheet;
		TextureReference	 m_texRef;

		float				m_fontScale;
	};

	typedef DataReference<Font, const std::string, 2> FontReference;
} // namespace Rhythmic

#endif
