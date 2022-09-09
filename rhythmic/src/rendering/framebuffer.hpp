#ifndef FRAMEBUFFER_HPP_
#define FRAMEBUFFER_HPP_

namespace Rhythmic
{
	/*
	Class allows to change the render target to a texture

	These textures can be used to create some awesome effects
	such as blurring, edge detection, warping the screen, etc.
	*/
	class FrameBuffer
	{
	public:
		FrameBuffer();
		~FrameBuffer();

		/*
		Creates a frame buffer the size of the window. Calling this will
		also make the frame buffer automatically resize itself when the
		window changes size

		Notes: Will bind global render target upon creation!

		Return: True if successful
		*/
		bool Create(int width, int height);

		/*
		Destroy the frame buffer object
		*/
		void Destroy();

		/*
		This unbinds all frame buffers, and gives the ability to
		draw on the screen again.
		*/
		static void BindGlobalTarget();

		/*
		Grants the ability to draw to and only to this particular
		frame buffer object
		*/
		void BindTarget();

		/*
		Binds the texture object, allowing the framebuffer to be
		used as an ordinary texture
		*/
		void Bind();

		/*
		Returns if the framebuffer is valid
		*/
		bool IsValid();

		int GetWidth();
		int GetHeight();
	private:
		bool m_created;

		int m_width;
		int m_height;

		unsigned int m_textureObject;
		unsigned int m_frameBufferObject;
		unsigned int m_renderTargetObject;

	};
}

#endif

