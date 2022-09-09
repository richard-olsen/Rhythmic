#include "catcher_renderer.hpp"

#include "../color.hpp"

namespace Rhythmic
{
	CatcherRenderer::CatcherRenderer()
	{}
	CatcherRenderer::~CatcherRenderer()
	{}

	void CatcherRenderer::Init()
	{
		m_batcher = RenderManager::GetBatcher();
		m_billboard = RenderManager::GetShader(SHADER_TYPE_BILLBOARD);
		m_highwayElements = RenderManager::GetSheet("highway_elements");
	}

	void CatcherRenderer::RenderCatchers(CatcherManager &manager, bool renderHits, bool flip)
	{
		unsigned int size;
		manager.GetCatcherAmount(&size);

		if (size == 5)
		{
			//bool draw_joint = !manager.IsDrums();
			if ((manager[0]->hit_height > 0) == renderHits) RenderCatcher(*manager[0], glm::vec2(CATCHER_POS_5_1 * (flip ? -1 : 1), 0), 0, flip, false);
			if ((manager[1]->hit_height > 0) == renderHits) RenderCatcher(*manager[1], glm::vec2(CATCHER_POS_5_2 * (flip ? -1 : 1), 0), 1, flip, false);
			if ((manager[2]->hit_height > 0) == renderHits) RenderCatcher(*manager[2], glm::vec2(CATCHER_POS_5_3 * (flip ? -1 : 1), 0), 2, false, false);
			if ((manager[3]->hit_height > 0) == renderHits) RenderCatcher(*manager[3], glm::vec2(CATCHER_POS_5_4 * (flip ? -1 : 1), 0), 1, !flip, false);
			if ((manager[4]->hit_height > 0) == renderHits) RenderCatcher(*manager[4], glm::vec2(CATCHER_POS_5_5 * (flip ? -1 : 1), 0), 0, !flip, false);
		}
		if (size == 4) // Sometimes, drums just don't have that orange pad
		{
			if ((manager[0]->hit_height > 0) == renderHits) RenderCatcher(*manager[0], glm::vec2(CATCHER_POS_4_1 * (flip ? -1 : 1), 0), 0, flip, false);
			if ((manager[1]->hit_height > 0) == renderHits) RenderCatcher(*manager[1], glm::vec2(CATCHER_POS_4_2 * (flip ? -1 : 1), 0), 1, flip, false);
			if ((manager[2]->hit_height > 0) == renderHits) RenderCatcher(*manager[2], glm::vec2(CATCHER_POS_4_3 * (flip ? -1 : 1), 0), 1, !flip, false);
			if ((manager[3]->hit_height > 0) == renderHits) RenderCatcher(*manager[3], glm::vec2(CATCHER_POS_4_4 * (flip ? -1 : 1), 0), 0, !flip, false);
		}
	}

	void CatcherRenderer::RenderCatcher(const Catcher &catcher, const glm::vec2 &pos, int index, bool flip, bool add_joint)
	{
		m_billboard->SetPointCenter(glm::vec3(pos.x, g_themeVars.highway.catchers_offset, 0));

		m_batcher->Clear();


		glm::vec2 xy = glm::vec2(-0.2f, 0.2f);
		glm::vec2 size = glm::vec2(0.4f, -0.2f);

		glm::vec2 hit_height = glm::vec2(0, catcher.hit_height);
		const glm::vec4 &color = g_colors[catcher.type];
		glm::vec4 bright = color * 2.0f;
		int i = index * 2;

		// Renders the top portion (usually not shown with drum catchers)
		if (add_joint)
			m_highwayElements.data->AddSprite(i, 0, 2, 1, xy, size, g_white, flip);

		// Renders the inlet of the catcher
		if (!catcher.active || catcher.hit_height > 0)
		{
			m_highwayElements.data->AddSprite(i, 3, 2, 1, xy, size, g_white, flip);
		}
		else
		{
			m_highwayElements.data->AddSprite(i, 4, 2, 1, xy, size, g_white, flip);
			// Adds color overlay
			m_highwayElements.data->AddSprite(i, 8, 2, 1, xy, size, bright, flip);
		}

		// Render ring
		m_highwayElements.data->AddSprite(i, 1, 2, 1, xy, size, color, flip);

		// Renders top
		if (catcher.hit_height > 0)
		{
			glm::vec2 half_sizeY = glm::vec2(0, size.y * 0.5f);
			m_highwayElements.data->AddSprite(8, 0, 2, 1, xy + hit_height + half_sizeY, size - hit_height - (half_sizeY * 1.2f), g_white, false);
			m_highwayElements.data->AddSprite(i, 5, 2, 1, xy + hit_height, size, g_white, flip);
			if (catcher.active)
				m_highwayElements.data->AddSprite(i, 7, 2, 1, xy + hit_height, size, bright, flip);
			else
				m_highwayElements.data->AddSprite(i, 6, 2, 1, xy + hit_height, size, color, flip);
		}
		else
		{
			if (!catcher.active)
			{
				m_highwayElements.data->AddSprite(i, 5, 2, 1, xy, size, g_white, flip);
				m_highwayElements.data->AddSprite(i, 6, 2, 1, xy, size, color, flip);
			}
		}
		
		// Render ring cover
		m_highwayElements.data->AddSprite(i, 2, 2, 1, xy, size, color, flip);

		m_batcher->Update();
		m_batcher->Draw();
	}
}