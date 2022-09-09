#ifndef RENDERING_TEXTURE_H_
#define RENDERING_TEXTURE_H_

#include "references.hpp"
#include "../io/io_image.hpp"

namespace Rhythmic
{
	namespace Rendering 
	{
		constexpr int TEXTURE_FLAG_FILTER_LINEAR = (1 << 0);
		constexpr int TEXTURE_FLAG_CLAMP = (1 << 1);

		class Texture
		{
		public:
			Texture();
			~Texture();

			bool Create(const IO_Image *image, int flags = (TEXTURE_FLAG_FILTER_LINEAR));
			void Destroy();

			float GetWidth();
			float GetHeight();

			void UpdateTexture(const IO_Image *image, int flags = (TEXTURE_FLAG_FILTER_LINEAR));

			void Bind();

			const std::string &GetFileName();
		private:
			bool			m_created;
			float			m_width;
			float			m_height;
			unsigned int	m_textureObject;

			std::string m_fileName;
		};
	} // namespace Rendering

	typedef DataReference<Rendering::Texture, const std::string, 0> TextureReference;
}

#endif
