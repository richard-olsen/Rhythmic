#include "texture.hpp"

#include "glad/glad.h"

#include <iostream>

#include <assert.h>

namespace Rhythmic
{
	namespace Rendering
	{
		int textureFormatTable[4] = {
			GL_ALPHA,
			GL_RG,
			GL_RGB,
			GL_RGBA
		};

		Texture::Texture() : m_textureObject(0), m_created(false)
		{}

		Texture::~Texture()
		{
			Destroy();
		}

		bool Texture::Create(const IO_Image *image, int flags)
		{
			if (m_created)
				Destroy();
			if (!m_created)
			{
				glGenTextures(1, &m_textureObject);
				m_created = true;

				UpdateTexture(image, flags);

				return true;
			}
			return false;
		}

		void Texture::Destroy()
		{
			if (m_created)
			{
				glDeleteTextures(1, &m_textureObject);
				m_created = false;
			}
		}

		float Texture::GetWidth() { return m_width; }
		float Texture::GetHeight() { return m_height; }

		void Texture::Bind()
		{
			if (m_created)
				glBindTexture(GL_TEXTURE_2D, m_textureObject);
		}

		const std::string &Texture::GetFileName()
		{
			return m_fileName;
		}

		void Texture::UpdateTexture(const IO_Image *image, int flags)
		{
			if (m_created)
			{
				glBindTexture(GL_TEXTURE_2D, m_textureObject);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, flags & TEXTURE_FLAG_FILTER_LINEAR ? GL_LINEAR : GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, flags & TEXTURE_FLAG_FILTER_LINEAR ? GL_LINEAR : GL_NEAREST);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, flags & TEXTURE_FLAG_CLAMP ? GL_CLAMP_TO_EDGE : GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, flags & TEXTURE_FLAG_CLAMP ? GL_CLAMP_TO_EDGE : GL_REPEAT);

				if (image != 0 && image->data != 0)
				{
					m_fileName = image->fileName;
					assert(image->component > 0 && image->component < 5);
					assert(image->component != 2);
					m_width = (float)image->width;
					m_height = (float)image->height;
					int format = textureFormatTable[image->component - 1];
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->width, image->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image->data[0]);
				}
				else
				{
					m_fileName = "";
					unsigned char data[4] = { 0xFF,0xFF,0xFF,0xFF };
					m_width = 1;
					m_height = 1;
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &data[0]);
				}
				glBindTexture(GL_TEXTURE_2D, 0);
			}
		}
	}
}