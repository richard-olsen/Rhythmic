#include "menu_music.hpp"

#include "../../game/menu_music_player.hpp"

namespace Rhythmic
{
	WidgetMenuMusic::WidgetMenuMusic() {}
	WidgetMenuMusic::~WidgetMenuMusic() {}

	void WidgetMenuMusic::Update(float delta) {}
	void WidgetMenuMusic::Render(float interpolation) {}

	void WidgetMenuMusic::OnCanvasActivated()
	{
		MusicPlayerMenu::GetInstance()->Start();
	}
}