#include "framebuffer.hpp"

#include "glad/glad.h"

#include "../io/io_window.hpp"
#include <wge/core/logging.hpp>

namespace Rhythmic
{
	FrameBuffer::FrameBuffer() :
		m_created(false),
		m_width(0),
		m_height(0),
		m_frameBufferObject(0),
		m_renderTargetObject(0),
		m_textureObject(0)
	{ }

	FrameBuffer::~FrameBuffer()
	{
		Destroy();
	}

	bool FrameBuffer::Create(int width, int height)
	{
		if (m_created)
			Destroy();

		m_width = width;
		m_height = height;

		bool successful = true;

		glGenFramebuffers(1, &m_frameBufferObject);
		glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferObject);

		glGenTextures(1, &m_textureObject);
		glBindTexture(GL_TEXTURE_2D, m_textureObject);

		// Pass NULL for pixels, it tells OpenGL that the image is empty
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

		/*
		Make sure the texture is using a linear filter, and the edges are clamped
		*/
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glBindTexture(GL_TEXTURE_2D, 0);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_textureObject, 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			LOG_WARNING("Failed to create Frame Buffer!");

			/*
			Delete the texture since frame buffer status failed
			*/
			glDeleteTextures(1, &m_textureObject);

			successful = false;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		m_created = successful;

		return successful;
	}

	void FrameBuffer::Destroy()
	{
		if (m_created)
		{
			glDeleteTextures(1, &m_textureObject);
			glDeleteFramebuffers(1, &m_frameBufferObject);
		}
	}

	void FrameBuffer::BindTarget()
	{
		if (m_created)
			glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferObject);
	}

	void FrameBuffer::Bind()
	{
		if (m_created)
			glBindTexture(GL_TEXTURE_2D, m_textureObject);
	}

	bool FrameBuffer::IsValid()
	{
		return m_created;
	}

	int FrameBuffer::GetWidth()
	{
		return m_width;
	}
	int FrameBuffer::GetHeight()
	{
		return m_height;
	}

	/*
	All STATIC functions below
	*/
	void FrameBuffer::BindGlobalTarget()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}
