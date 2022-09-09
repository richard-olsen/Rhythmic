#define STB_TRUETYPE_IMPLEMENTATION
#include "font.hpp"

#include "render_manager.hpp"

#include <fstream>

#include "../util/misc.hpp"
#include <wge/core/logging.hpp>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../util/stb/stb_image_write.h"

#include "../util/utf8/core.h"

#include "../themes/theme.hpp"

namespace Rhythmic
{
	Font::Font() : m_chars(0)
	{

	}
	Font::~Font()
	{
		DestroyFont();
	}

	bool Font::CreateFontFromFile(const std::string &font)
	{
		// Read ttf into a file
		std::ifstream file(font, std::ios::binary);
		
		if (!file.is_open())
		{
			LOG_ERROR("Failed to open font: {0}", font);
			return false;
		}

		file.seekg(0, file.end);
		size_t size = file.tellg();
		unsigned char *data = new unsigned char[size];

		file.seekg(0, file.beg);
		file.read((char*)data, size);
		file.close();

		// Font loading
		Rhythmic::IO_Image image;
		image.width = 1024;
		image.height = 1024;
		image.component = 4;
		image.data = new unsigned char[image.width * image.height * image.component];
		unsigned char *fontAlpha = new unsigned char[image.width * image.height];

		stbtt_bakedchar *chars = new stbtt_bakedchar[1024];
		m_chars = new FontChar[512];
		
		stbtt_fontinfo info;

		stbtt_InitFont(&info, data, 0);

		m_fontScale = stbtt_ScaleForPixelHeight(&info, FONT_TTF_RASTER_SCALE);

		stbtt_BakeFontBitmap(data, 0, FONT_TTF_RASTER_SCALE, fontAlpha, image.width, image.height, 32, 512, chars);
		
		for (unsigned int y = 0; y < image.height; y++)
		{
			for (unsigned int x = 0; x < image.width; x++)
			{
				int indexAlpha = x + (y * image.width); // 4 is the component, we want to skip it all in the index
				int index = indexAlpha * 4;

				image.data[index + 0] = 255;
				image.data[index + 1] = 255;
				image.data[index + 2] = 255;
				image.data[index + 3] = fontAlpha[indexAlpha];
			}
		}
		delete[] fontAlpha;
		// Debugging
		stbi_write_png((font + ".png").c_str(), image.width, image.height, image.component, image.data, 0);

		m_texRef = RenderManager::CreateTexture(&image, font);

		for (unsigned int i = 0; i < 512; i++)
		{
			m_chars[i].x0 = (float)chars[i].x0 / (float)image.width;
			m_chars[i].x1 = (float)chars[i].x1 / (float)image.width;
			m_chars[i].y0 = (float)chars[i].y0 / (float)image.height;
			m_chars[i].y1 = (float)chars[i].y1 / (float)image.height;
			m_chars[i].xsize = chars[i].x1 - chars[i].x0;
			m_chars[i].ysize = chars[i].y1 - chars[i].y0;
			m_chars[i].xoff = chars[i].xoff;
			m_chars[i].xadvance = chars[i].xadvance;
			m_chars[i].yoff = chars[i].yoff;
		}


		delete[] chars;
		delete[] image.data;
		delete[] data;
		return true;
	}
	void Font::DestroyFont()
	{
		if (m_chars)
		{
			delete[] m_chars;
			m_chars = 0;
		}
		m_texRef.Dispose();
	}

	float Font::GetSize(const std::string &string, float scale)
	{
		float retVal = 0.0f;

		float uScale = m_fontScale * scale;

		auto it = string.begin();

		while (it != string.end())
		{
			uint32_t cp; // Code Point of the character
			if (utf8::internal::validate_next(it, string.end(), cp) != utf8::internal::UTF8_OK)
			{
				it++;
				continue;
			}

			if (cp < 32)
				continue;
			cp -= 32;
			//if (c > 1024) continue;
			const FontChar &cdata = m_chars[cp];

			//retVal += (cdata.xoff * uScale) + (cdata.xsize * uScale);
			retVal += cdata.xadvance * uScale;
		}

		//for (unsigned int i = 0; i < string.size(); i++)
		//{
		//	char c = string[i];
		//	if (c < 32)
		//		continue;
		//	c -= 32;
		//	//if (c > 1024) continue;
		//	const FontChar &cdata = m_chars[c];

		//	retVal += (cdata.xoff * uScale) + (cdata.xsize * uScale);
		//}

		return retVal;
	}

	float Font::GetScaledHeight()
	{
		return m_fontScale;
	}
	
	void Font::DrawString(const std::string &string, float x, float y, float scale_x, float scale_y, const glm::vec4 &color, int alignment, int cursorPos)
	{
		Rendering::VertexBatcher *batcher = RenderManager::GetBatcher();

		float uScaleX = m_fontScale * scale_x;
		float uScaleY = m_fontScale * scale_y;

		// Do alignment later
		float adjX = x; // Alignment X
		if (alignment == 1) // Centered
		{
			/*float x2 = x;
			for (unsigned int i = 0; i < string.size(); i++)
			{
				const char &c = string[i];
				if (c >= 32)
					x2 -= m_chars[c - 32].xadvance * uScaleX;
			}
			adjX = (x + x2) * 0.5f;*/
			float size = GetSize(string, scale_x);
			adjX -= size * 0.5f;
		}
		if (alignment == 2) // Right
		{
			/*for (unsigned int i = 0; i < string.size(); i++)
			{
				const char &c = string[i];
				if (c >= 32)
					adjX -= m_chars[c - 32].xadvance * uScaleX;
			}*/
			float size = GetSize(string, scale_x);
			adjX -= size;
		}
		// Other than that, ignore it and do left

		float pX = adjX;		// X position
		float pY = y + (uScaleY * FONT_TTF_RASTER_SCALE * 0.25f);		// Y position

		float cursorOffset = 0;
		if (cursorPos >= 0)
			cursorOffset = m_chars['|' - 32].xadvance * uScaleX * 0.5f;

		m_texRef->Bind();

		batcher->Use();
		batcher->Clear();

		auto it = string.begin();
		int i = 0;

		while (it != string.end())
		{
			uint32_t cp; // Code Point of the character
			if (utf8::internal::validate_next(it, string.end(), cp) != utf8::internal::UTF8_OK)
			{
				it++;
				continue;
			}

			if (cp < 32)
				continue;
			if (cp == '\n')
			{
				pY += scale_y;
				pX = adjX;
			}

			if (i == cursorPos)
				DrawChar('|', pX - cursorOffset, pY, uScaleX, uScaleY, color);
			DrawChar(cp, pX, pY, uScaleX, uScaleY, color);
			i++;

			pX += m_chars[cp - 32].xadvance * uScaleX;
		}

		/*for (unsigned int i = 0; i < string.size(); i++)
		{
			char c = string[i];
			if (c < 32)
				continue;
			if (c == '\n')
			{
				pY += scale_y;
				pX = adjX;
			}

			if (i == cursorPos)
				DrawChar('|', pX - cursorOffset, pY, uScaleX, uScaleY, color);
			DrawChar(c, pX, pY, uScaleX, uScaleY, color);

			pX += m_chars[c-32].xadvance * uScaleX;
		}*/

		if (string.size() == cursorPos)
			DrawChar('|', pX - cursorOffset, pY, uScaleX, uScaleY, color);
		
		batcher->Update();
		batcher->Draw();
	}
	void Font::DrawChar(uint32_t c, float x, float y, float scale_x, float scale_y, const glm::vec4 &color)
	{
		Rendering::VertexBatcher *batcher = RenderManager::GetBatcher();

		if (c < 32)
			return;
		c -= 32;
		//if (c > 1024) continue;
		const FontChar &cdata = m_chars[c];

		float uX = x  + (cdata.xoff * scale_x);		// Used X position
		float uX2= uX + (cdata.xsize * scale_x);		// Used X size
		float uY = y  + (cdata.yoff * scale_y);		// Used Y position
		float uY2= uY + (cdata.ysize * scale_y);

		batcher->AddVertex(uX,  uY,  0, cdata.x0, cdata.y0, color.x, color.y, color.z, color.w);
		batcher->AddVertex(uX2, uY,  0, cdata.x1, cdata.y0, color.x, color.y, color.z, color.w);
		batcher->AddVertex(uX2, uY2, 0, cdata.x1, cdata.y1, color.x, color.y, color.z, color.w);

		batcher->AddVertex(uX2, uY2, 0, cdata.x1, cdata.y1, color.x, color.y, color.z, color.w);
		batcher->AddVertex(uX,  uY2, 0, cdata.x0, cdata.y1, color.x, color.y, color.z, color.w);
		batcher->AddVertex(uX,  uY,  0, cdata.x0, cdata.y0, color.x, color.y, color.z, color.w);
	}
}