#include "tile.hpp"

#include "../../rendering/render_manager.hpp"

#include "../widget_util.hpp"

#include "../../io/io_window.hpp"

namespace Rhythmic
{
	WidgetTile::WidgetTile(int tile_x, int tile_y, const SheetReference &sheet, const glm::vec4 &color) :
		m_sheet(sheet),
		m_tileX(tile_x),
		m_tileY(tile_y),
		m_color(color)
	{
	}

	WidgetTile::~WidgetTile()
	{ }

	void WidgetTile::Update(float delta)
	{

	}
	void WidgetTile::Render(float interpolation)
	{
		Rendering::VertexBatcher *batcher = RenderManager::GetBatcher();

		batcher->Clear();

		m_sheet->Bind();

		//glm::vec2 correctedScale;
		//WidgetUtil::AdjustRatio(correctedScale, (float)IO_Window_GetWidth(), (float)IO_Window_GetHeight());
		//correctedScale *= m_scale;

		m_sheet->AddSpriteScaled(m_tileX, m_tileY, 1, 1, m_position, m_scale, m_color);

		batcher->Update();
		batcher->Draw();
	}
}

