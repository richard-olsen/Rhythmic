#include "theme.hpp"
#include "theme.hpp"

#include <iostream>
#include <filesystem>

#include <fstream>
#include "../util/misc.hpp"

#include <wge/core/logging.hpp>

#include "../rendering/font.hpp"

namespace Rhythmic
{
	Theme g_activeTheme;

	ThemeVariables g_themeVars = {
		{
			(1.0f / 2560.0f),					// Element pixel y size
			230.0f,								// Value stops sliding along the measure textures
			30.0f,								// Y correction for notes
			-0.05f,								// Time offset for notes
			30.0f,								// Y correction for open notes
			-0.025f,								// Time offset for open notes
			-0.1f,
			glm::vec2(0.5f, 0.25f),					// Note size
			glm::vec2(2.0f, 0.25f),					// Open note size
			//glm::vec2(0.45f, 0.225f),					// Note size
			//glm::vec2(2.0f, 0.225f),					// Open note size
			{// The score counter (the circle that counts up)
				glm::vec2(11,4),
				glm::vec2(14,4),
				glm::vec2(17,4),
				glm::vec2(20,4),
				glm::vec2(23,4),
				glm::vec2(26,4),
				glm::vec2(11,6),
				glm::vec2(14,6),
				glm::vec2(17,6),
				glm::vec2(20,6),
				glm::vec2(23,6)
			},
			{ // Score Modifier
				glm::vec2(8,8),
				glm::vec2(11,8),
				glm::vec2(14,8),
				glm::vec2(17,8),
				glm::vec2(20,8),
				glm::vec2(23,8),
				glm::vec2(26,8),
				glm::vec2(29,8),
				glm::vec2(32,8),
			},
			{ // FC Animation tiles
				glm::vec2(14,4),
				glm::vec2(8,10),
				glm::vec2(11,10),
				glm::vec2(14,10),
				glm::vec2(17,10),
				glm::vec2(20,10),
				glm::vec2(23,10),
				glm::vec2(26,10),
				glm::vec2(29,10),
				glm::vec2(32,10),
			},
			glm::vec2(8,6),
			glm::vec2(8,12)
		},
		//{ // Original
		//	-65.0f,		// Rotation
		//	1.808f,		// Height
		//	//1.818f,		// Offset
		//	1.968f,		// Offset
		//	1.6f		// Fog Position
		//},
		//{ // Vertical Highway
		//	-0.0f,		// Rotation
		//	2.94f,		// Height
		//	//1.818f		// Offset
		//	-1.1f,		// Offset
		//	1.7f		// Fog Position
		//},
		{ // New
			-60.0f,		// Rotation
			1.94f,		// Height
			//1.818f		// Offset
			1.81f,		// Offset
			1.7f		// Fog Position
		},
		{
			FONT_TTF_RASTER_SCALE		// Font y offset
		}
		//{
		//	-52.0f,		// Rotation
		//	2.12f,		// Height
		//	1.5f		// Offset
		//}
	};

	void GetInstalledThemes(std::vector<Theme> &themes)
	{
	}

	bool LoadThemeJSON(Theme &theme)
	{
		return false;
	}

	void SetActiveTheme(Theme theme)
	{
		g_activeTheme = theme;
	}

	Theme *GetActiveTheme()
	{
		return &g_activeTheme;
	}
} // namespace Rhythmic

