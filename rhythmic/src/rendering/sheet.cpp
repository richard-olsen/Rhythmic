#include "sheet.hpp"

#include "render_manager.hpp"

namespace Rhythmic
{
	Sheet::Sheet() : m_created(false),
					 m_texture(),
					 m_spriteModifierX(0),
					 m_spriteModifierY(0)
	{}

	Sheet::~Sheet()
	{
		Destroy();
	}

	bool Sheet::Create(IO_Image *image, int tiles_x, int tiles_y, int textureFlags)
	{
		if (m_created)
			Destroy();

		if (!m_created)
		{
			m_spriteModifierX = 1.0f / (float) tiles_x;
			m_spriteModifierY = 1.0f / (float) tiles_y;

			bool c = m_texture.Create(image, textureFlags);
			if (!c)
			{
				m_spriteModifierX = 0;
				m_spriteModifierY = 0;
				return false;
			}

			m_created = true;
			return true;
		}
		return false;
	}

	void Sheet::Destroy()
	{
		if (m_created)
		{
			m_spriteModifierX = 0;
			m_spriteModifierY = 0;
			m_texture.Destroy();
			m_created = false;
		}
	}

	void Sheet::Bind()
	{
		if (m_created)
			m_texture.Bind();
	}

	glm::vec4 Sheet::GetTextureCoordForQuadOnSheet(int tile_x, int tile_y, float tile_x_extend, float tile_y_extend)
	{
		if (m_created)
		{
			float u = m_spriteModifierX * tile_x;
			float v = m_spriteModifierY * tile_y;
			float ue = m_spriteModifierX * tile_x_extend;
			float ve = m_spriteModifierY * tile_y_extend;
			return glm::vec4(u, v, ue, ve);
		}
		return glm::vec4();
	}

	void Sheet::AddSprite(int tile_x, int tile_y, float tile_x_extend, float tile_y_extend, const glm::vec3 &pos1, const glm::vec3 &pos2, const glm::vec3 &pos3, const glm::vec3 &pos4, const glm::vec4 &color)
	{
		if (m_created)
		{
			float u = m_spriteModifierX * tile_x;
			float v = m_spriteModifierY * tile_y;
			float ue = m_spriteModifierX * tile_x_extend;
			float ve = m_spriteModifierY * tile_y_extend;
			Rendering::VertexBatcher *batcher = RenderManager::GetBatcher();

			batcher->AddVertex(pos1.x, pos1.y, pos1.z, u, v, color.x, color.y, color.z, color.w);
			batcher->AddVertex(pos2.x, pos2.y, pos2.z, u + ue, v, color.x, color.y, color.z, color.w);
			batcher->AddVertex(pos3.x, pos3.y, pos3.z, u + ue, v + ve, color.x, color.y, color.z, color.w);

			batcher->AddVertex(pos3.x, pos3.y, pos3.z, u + ue, v + ve, color.x, color.y, color.z, color.w);
			batcher->AddVertex(pos4.x, pos4.y, pos4.z, u, v + ve, color.x, color.y, color.z, color.w);
			batcher->AddVertex(pos1.x, pos1.y, pos1.z, u, v, color.x, color.y, color.z, color.w);
		}
	}

	void Sheet::AddSprite(int tile_x, int tile_y, float tile_x_extend, float tile_y_extend, const glm::vec2 &pos, const glm::vec2 &size, const glm::vec4 &color, bool mirror)
	{
		if (m_created)
		{
			float u = m_spriteModifierX * tile_x;
			float v = m_spriteModifierY * tile_y;
			float ue = m_spriteModifierX * tile_x_extend;
			float ve = m_spriteModifierY * tile_y_extend;
			Rendering::VertexBatcher *batcher = RenderManager::GetBatcher();

			if (mirror)
			{
				batcher->AddVertex(pos.x, pos.y, 0, u + ue, v, color.x, color.y, color.z, color.w);
				batcher->AddVertex(pos.x + size.x, pos.y, 0, u, v, color.x, color.y, color.z, color.w);
				batcher->AddVertex(pos.x + size.x, pos.y + size.y, 0, u, v + ve, color.x, color.y, color.z, color.w);

				batcher->AddVertex(pos.x + size.x, pos.y + size.y, 0, u, v + ve, color.x, color.y, color.z, color.w);
				batcher->AddVertex(pos.x, pos.y + size.y, 0, u + ue, v + ve, color.x, color.y, color.z, color.w);
				batcher->AddVertex(pos.x, pos.y, 0, u + ue, v, color.x, color.y, color.z, color.w);
			}
			else
			{
				batcher->AddVertex(pos.x, pos.y, 0, u, v, color.x, color.y, color.z, color.w);
				batcher->AddVertex(pos.x + size.x, pos.y, 0, u + ue, v, color.x, color.y, color.z, color.w);
				batcher->AddVertex(pos.x + size.x, pos.y + size.y, 0, u + ue, v + ve, color.x, color.y, color.z, color.w);

				batcher->AddVertex(pos.x + size.x, pos.y + size.y, 0, u + ue, v + ve, color.x, color.y, color.z, color.w);
				batcher->AddVertex(pos.x, pos.y + size.y, 0, u, v + ve, color.x, color.y, color.z, color.w);
				batcher->AddVertex(pos.x, pos.y, 0, u, v, color.x, color.y, color.z, color.w);
			}
		}
	}

	void Sheet::AddSprite(int tile_x, int tile_y, float tile_x_extend, float tile_y_extend, const glm::vec3 &pos, const glm::vec2 &size, const glm::vec4 &color, bool mirror)
	{
		if (m_created)
		{
			float u = m_spriteModifierX * tile_x;
			float v = m_spriteModifierY * tile_y;
			float ue = m_spriteModifierX * tile_x_extend;
			float ve = m_spriteModifierY * tile_y_extend;
			Rendering::VertexBatcher *batcher = RenderManager::GetBatcher();

			if (mirror)
			{
				batcher->AddVertex(pos.x, pos.y, pos.z, u + ue, v, color.x, color.y, color.z, color.w);
				batcher->AddVertex(pos.x + size.x, pos.y, pos.z, u, v, color.x, color.y, color.z, color.w);
				batcher->AddVertex(pos.x + size.x, pos.y + size.y, pos.z, u, v + ve, color.x, color.y, color.z, color.w);

				batcher->AddVertex(pos.x + size.x, pos.y + size.y, pos.z, u, v + ve, color.x, color.y, color.z, color.w);
				batcher->AddVertex(pos.x, pos.y + size.y, pos.z, u + ue, v + ve, color.x, color.y, color.z, color.w);
				batcher->AddVertex(pos.x, pos.y, pos.z, u + ue, v, color.x, color.y, color.z, color.w);
			}
			else
			{
				batcher->AddVertex(pos.x, pos.y, pos.z, u, v, color.x, color.y, color.z, color.w);
				batcher->AddVertex(pos.x + size.x, pos.y, pos.z, u + ue, v, color.x, color.y, color.z, color.w);
				batcher->AddVertex(pos.x + size.x, pos.y + size.y, pos.z, u + ue, v + ve, color.x, color.y, color.z, color.w);

				batcher->AddVertex(pos.x + size.x, pos.y + size.y, pos.z, u + ue, v + ve, color.x, color.y, color.z, color.w);
				batcher->AddVertex(pos.x, pos.y + size.y, pos.z, u, v + ve, color.x, color.y, color.z, color.w);
				batcher->AddVertex(pos.x, pos.y, pos.z, u, v, color.x, color.y, color.z, color.w);
			}
		}
	}

	void Sheet::AddSpriteScaled(int tile_x, int tile_y, float tile_x_extend, float tile_y_extend, const glm::vec2 &pos, const glm::vec2 &scale, const glm::vec4 &color, bool mirror)
	{
		if (m_created)
		{
			float u = m_spriteModifierX * tile_x;
			float v = m_spriteModifierY * tile_y;
			float ue = m_spriteModifierX * tile_x_extend;
			float ve = m_spriteModifierY * tile_y_extend;
			Rendering::VertexBatcher *batcher = RenderManager::GetBatcher();

			if (mirror)
			{
				batcher->AddVertex(pos.x - scale.x, pos.y - scale.y, 0, u + ue, v, color.x, color.y, color.z, color.w);
				batcher->AddVertex(pos.x + scale.x, pos.y - scale.y, 0, u, v, color.x, color.y, color.z, color.w);
				batcher->AddVertex(pos.x + scale.x, pos.y + scale.y, 0, u, v + ve, color.x, color.y, color.z, color.w);

				batcher->AddVertex(pos.x + scale.x, pos.y + scale.y, 0, u, v + ve, color.x, color.y, color.z, color.w);
				batcher->AddVertex(pos.x - scale.x, pos.y + scale.y, 0, u + ue, v + ve, color.x, color.y, color.z, color.w);
				batcher->AddVertex(pos.x - scale.x, pos.y - scale.y, 0, u + ue, v, color.x, color.y, color.z, color.w);
			}
			else
			{
				batcher->AddVertex(pos.x - scale.x, pos.y - scale.y, 0, u, v, color.x, color.y, color.z, color.w);
				batcher->AddVertex(pos.x + scale.x, pos.y - scale.y, 0, u + ue, v, color.x, color.y, color.z, color.w);
				batcher->AddVertex(pos.x + scale.x, pos.y + scale.y, 0, u + ue, v + ve, color.x, color.y, color.z, color.w);

				batcher->AddVertex(pos.x + scale.x, pos.y + scale.y, 0, u + ue, v + ve, color.x, color.y, color.z, color.w);
				batcher->AddVertex(pos.x - scale.x, pos.y + scale.y, 0, u, v + ve, color.x, color.y, color.z, color.w);
				batcher->AddVertex(pos.x - scale.x, pos.y - scale.y, 0, u, v, color.x, color.y, color.z, color.w);
			}
		}
	}

	Rendering::Texture *Sheet::GetTexture()
	{
		return &m_texture;
	}
}
