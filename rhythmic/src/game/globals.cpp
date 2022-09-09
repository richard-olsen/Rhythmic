#include "globals.hpp"

namespace Rhythmic
{
	int g_songButtonSelected;
	int g_bgImage;

	void Global_SetSongButtonSelected(int selected) { g_songButtonSelected = selected; }
	int Global_SongButtonSelected() { return g_songButtonSelected; }

	void Global_SetBGImage(int image)
	{
		g_bgImage = image;
	}
	int Global_GetBGImage()
	{
		return g_bgImage;
	}
}

