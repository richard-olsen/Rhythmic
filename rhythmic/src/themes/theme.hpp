#ifndef _THEME_H_
#define _THEME_H_

#include <string>
#include <vector>

#include "theme_structs.hpp"

#include <glm/vec2.hpp>

namespace Rhythmic
{
	struct ThemeVariables
	{
		struct
		{
			float element_y_pixel_size;		// The size of the pixel of the texture (determined internally)
			float element_correction_beatline;		// This value is used to stop sliding issues
			float element_correction_note;
			float element_correction_note_time_offset;
			float element_correction_note_open;
			float element_correction_note_open_time_offset;
			float catchers_offset;
			glm::vec2 note_size;							// The size of the note
			glm::vec2 open_note_size;						// The size of an open note
			glm::vec2 notecounter_tiles[11];
			glm::vec2 scoremod_tiles[9];
			glm::vec2 fcAnim_tiles[10];
			glm::vec2 scoremod_base_tile;
			glm::vec2 hud_cover_tile;
		} highway;
		struct
		{
			float rotation;
			float height;
			float offset;
			float fogPos;
		} highway_camera;
		struct
		{
			float text_y_offset;						// Some fonts tend to want to offset. This varies font from font
		} misc;
	};
	
	extern ThemeVariables g_themeVars;

	struct Theme
	{
		// Is the directory of the theme
		std::string themePath;

		// Below are values that are defined in the "theme.json"

		// Name of the theme
		std::string themeName;

		// Structure that contains the name of sprites
		struct
		{
			// Sprites for the catchers
			std::string highway_elements;
			index2 highway_elements_tiles;
		} sprites;

		// Structure that contains the fonts
		struct
		{
			std::string menus;
			std::string score;
		} fonts;

		// Structure for Catchers-Guitar
		ThemeCatchers catchers;
	};

	/*
	Searches through the themes folder and adds any themes that are currently installed and have a "theme.json"
	*/
	void GetInstalledThemes(std::vector<Theme> &themes);
	
	/*
	Loads in the theme details.
	Reads entire theme.json but doesn't actually create sprites, sounds, scripts, and other gameplay components.
	*/
	bool LoadThemeJSON(Theme &theme);

	/*
	Loads in the rest of the theme, creating sprites, sounds, scripts, and other gameplay components.
	Called usually when the theme is selected, AFTER LoadThemePartial has been called.
	*/
	//void LoadThemeFull(Theme &theme);

	/*
	Sets the active theme.
	*/
	void SetActiveTheme(Theme theme);
	
	/*
	Retrieves the active theme.
	*/
	Theme *GetActiveTheme();
} // namespace Rhythmic

#endif //_THEME_H_
