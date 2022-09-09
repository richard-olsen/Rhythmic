#ifndef RENDERING_SHEET_H_
#define RENDERING_SHEET_H_

#include "references.hpp"
#include "texture.hpp"
#include "vertex_batcher.hpp"

#include <glm/glm.hpp>
#include <glm/common.hpp>

namespace Rhythmic
{
	class Sheet 
	{
	public:
		Sheet();
		~Sheet();

		bool Create(IO_Image *image, int tiles_x, int tiles_y, int textureFlags = Rendering::TEXTURE_FLAG_CLAMP | Rendering::TEXTURE_FLAG_FILTER_LINEAR);
		void Destroy();

		void Bind();

		void AddSprite(int tile_x, int tile_y, float tile_x_extend, float tile_y_extend, const glm::vec3 &pos1, const glm::vec3 &pos2, const glm::vec3 &pos3, const glm::vec3 &pos4, const glm::vec4 &color);
		void AddSprite(int tile_x, int tile_y, float tile_x_extend, float tile_y_extend, const glm::vec2 &pos, const glm::vec2 &size, const glm::vec4 &color, bool mirror = false);
		void AddSprite(int tile_x, int tile_y, float tile_x_extend, float tile_y_extend, const glm::vec3 &pos, const glm::vec2 &size, const glm::vec4 &color, bool mirror = false); // z controls depth
		void AddSpriteScaled(int tile_x, int tile_y, float tile_x_extend, float tile_y_extend, const glm::vec2 &pos, const glm::vec2 &scale, const glm::vec4 &color, bool mirror = false);

		glm::vec4 GetTextureCoordForQuadOnSheet(int tile_x, int tile_y, float tile_x_extend, float tile_y_extend);

		Rendering::Texture *GetTexture();
	private:
		bool				m_created;

		Rendering::Texture	m_texture;
		float				m_spriteModifierX;
		float				m_spriteModifierY;
	};

	typedef DataReference<Sheet, const std::string, 1> SheetReference;
} // namespace Rhythmic

#endif
