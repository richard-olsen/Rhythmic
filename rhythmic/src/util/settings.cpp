#include "settings.hpp"

#include "misc.hpp"

#include "../themes/theme.hpp"

#include <wge/core/logging.hpp>

#include "ini_parser.hpp"

#include <fstream>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace Rhythmic
{

	IniFile g_settingsIni;

	void SettingsGet(SettingValue *value)
	{
		switch (value->type) // Switch value's type
		{
		case SETTING_AUDIO_MASTER:
			value->m_iValue = std::stoi(g_settingsIni.m_sections["audio"]["master_volume"]); // value's int value is set to one in INI. Same for rest below
			break;
		case SETTING_AUDIO_MUSIC:
			value->m_iValue = std::stoi(g_settingsIni.m_sections["audio"]["music_volume"]);
			break;
		case SETTING_AUDIO_SFX:
			value->m_iValue = std::stoi(g_settingsIni.m_sections["audio"]["sfx_volume"]);
			break;
		case SETTING_AUDIO_MENU_MUSIC_VOLUME:
			value->m_iValue = std::stoi(g_settingsIni.m_sections["audio"]["menu_music_volume"]);
			break;

		case SETTING_GAME_CALIBRATION_AUDIO:
			value->m_iValue = std::stoi(g_settingsIni.m_sections["game"]["calibration_audio"]);
			break;
		case SETTING_GAME_CALIBRATION_VIDEO:
			value->m_iValue = std::stoi(g_settingsIni.m_sections["game"]["calibration_video"]);
			break;
		case SETTING_GAME_SONGLIST_DIRECTORY:
			value->m_sValue = g_settingsIni.m_sections["game"]["song_directory"];
			break;
		case SETTING_GAME_ACTIVE_THEME:
			value->m_sValue = g_settingsIni.m_sections["game"]["active_theme"];
			break;
		case SETTING_GAME_ENGINE:
			value->m_sValue = g_settingsIni.m_sections["game"]["engine"];
			break;
		case SETTING_GAME_MENU_MUSIC:
			value->m_iValue = std::stoi(g_settingsIni.m_sections["game"]["menu_music"]);
			break;
		case SETTING_GAME_SHOW_MISSED_NOTES:
			value->m_iValue = std::stoi(g_settingsIni.m_sections["game"]["show_missed_notes"]);
			break;

		case SETTING_WINDOW_FULLSCREEN:
			value->m_iValue = std::stoi(g_settingsIni.m_sections["window"]["fullscreen"]);
			break;
		case SETTING_WINDOW_HEIGHT:
			value->m_iValue = std::stoi(g_settingsIni.m_sections["window"]["height"]);
			break;
		//case SETTING_WINDOW_REFRESH_RATE:
		//	value->m_iValue = std::stoi(g_settingsIni.m_sections["window"]["refresh_rate"]);
		//	break;
		case SETTING_WINDOW_VSYNC:
			value->m_bValue = Util::compareStringCaseInsensitive(g_settingsIni.m_sections["window"]["vsync"], "true") == 0;
			break;
		case SETTING_WINDOW_WIDTH:
			value->m_iValue = std::stoi(g_settingsIni.m_sections["window"]["width"]);
			break;
		}
	}
	void SettingsSet(const SettingValue &value)
	{
		switch (value.type)
		{
		// Audio
		case SETTING_AUDIO_MASTER:
			g_settingsIni.m_sections["audio"]["master_volume"] = std::to_string(value.m_iValue);
			break;
		case SETTING_AUDIO_MUSIC:
			g_settingsIni.m_sections["audio"]["music_volume"] = std::to_string(value.m_iValue);
			break;
		case SETTING_AUDIO_SFX:
			g_settingsIni.m_sections["audio"]["sfx_volume"] = std::to_string(value.m_iValue);
			break;
		case SETTING_AUDIO_MENU_MUSIC_VOLUME:
			g_settingsIni.m_sections["audio"]["menu_music_volume"] = std::to_string(value.m_iValue);
			break;

		// Game
		case SETTING_GAME_CALIBRATION_AUDIO:
			g_settingsIni.m_sections["game"]["calibration_audio"] = std::to_string(value.m_iValue);
			break;
		case SETTING_GAME_CALIBRATION_VIDEO:
			g_settingsIni.m_sections["game"]["calibration_video"] = std::to_string(value.m_iValue);
			break;
		case SETTING_GAME_SONGLIST_DIRECTORY:
			g_settingsIni.m_sections["game"]["song_directory"] = value.m_sValue;
			break;
		case SETTING_GAME_ACTIVE_THEME:
			g_settingsIni.m_sections["game"]["active_theme"] = value.m_sValue;
			break;
		case SETTING_GAME_ENGINE:
			g_settingsIni.m_sections["game"]["engine"] = value.m_sValue;
			break;
		case SETTING_GAME_MENU_MUSIC:
			g_settingsIni.m_sections["game"]["menu_music"] = std::to_string(value.m_iValue);
			break;
		case SETTING_GAME_SHOW_MISSED_NOTES:
			g_settingsIni.m_sections["game"]["show_missed_notes"] = std::to_string(value.m_iValue);

		// Window
		case SETTING_WINDOW_FULLSCREEN:
			g_settingsIni.m_sections["window"]["fullscreen"] = std::to_string(value.m_iValue);
			break;
		case SETTING_WINDOW_HEIGHT:
			g_settingsIni.m_sections["window"]["height"] = std::to_string(value.m_iValue);
			break;
		//case SETTING_WINDOW_REFRESH_RATE:
		//	g_settingsIni.m_sections["window"]["refresh_rate"] = std::to_string(value.m_iValue);
		//	break;
		case SETTING_WINDOW_VSYNC:
			g_settingsIni.m_sections["window"]["vsync"] = value.m_bValue ? "True" : "False";
			break;
		case SETTING_WINDOW_WIDTH:
			g_settingsIni.m_sections["window"]["width"] = std::to_string(value.m_iValue);
			break;
		}
	}
	void SettingsGenerateFresh()
	{
		g_settingsIni.m_sections.clear();
		g_settingsIni.m_global.clear();

		IniMap audio;
		IniMap video;
		IniMap game;
		IniMap window;

		audio.insert(IniEntry("master_volume", "100"));
		audio.insert(IniEntry("music_volume", "100"));
		audio.insert(IniEntry("sfx_volume", "100"));
		audio.insert(IniEntry("menu_music_volume", "100"));

		game.insert(IniEntry("calibration_audio", "0"));
		game.insert(IniEntry("calibration_video", "0"));
		game.insert(IniEntry("song_directory", Util::GetExecutablePath() + "/songs"));
		game.insert(IniEntry("active_theme", "Rhythmic Default"));
		game.insert(IniEntry("engine", "Rhythmic"));
		game.insert(IniEntry("menu_music", "1"));
		game.insert(IniEntry("show_missed_notes", "0"));

		const GLFWvidmode *currentDisplay = glfwGetVideoMode(glfwGetPrimaryMonitor());

		window.insert(IniEntry("width", std::to_string(currentDisplay->width)));
		window.insert(IniEntry("height", std::to_string(currentDisplay->height)));
		window.insert(IniEntry("fullscreen", "1"));
		//window.insert(IniEntry("refresh_rate", std::to_string(currentDisplay->refreshRate)));
		window.insert(IniEntry("vsync", "False"));

		g_settingsIni.m_sections.insert(IniSectionEntry("audio", audio));
		g_settingsIni.m_sections.insert(IniSectionEntry("window", window));
		g_settingsIni.m_sections.insert(IniSectionEntry("game", game));
	}
	void SettingsSave()
	{
		IniWrite(g_settingsIni, Util::GetExecutablePath() + "/settings.ini");
	}

	void SettingsVerify()
	{
		/*
		Audio Section
		*/
		if (g_settingsIni.m_sections["audio"]["master_volume"].empty())
			g_settingsIni.m_sections["audio"]["master_volume"] = "100";
		if (g_settingsIni.m_sections["audio"]["music_volume"].empty())
			g_settingsIni.m_sections["audio"]["music_volume"] = "100";
		if (g_settingsIni.m_sections["audio"]["sfx_volume"].empty())
			g_settingsIni.m_sections["audio"]["sfx_volume"] = "100";
		if (g_settingsIni.m_sections["audio"]["menu_music_volume"].empty())
			g_settingsIni.m_sections["audio"]["menu_music_volume"] = "100";
		/*
		Game Section
		*/
		if (g_settingsIni.m_sections["game"]["calibration_audio"].empty())
			g_settingsIni.m_sections["game"]["calibration_audio"] = "0";
		if (g_settingsIni.m_sections["game"]["calibration_video"].empty())
			g_settingsIni.m_sections["game"]["calibration_video"] = "0";
		if (g_settingsIni.m_sections["game"]["song_directory"].empty())
			g_settingsIni.m_sections["game"]["song_directory"] = Util::GetExecutablePath() + "/songs";
		if (g_settingsIni.m_sections["game"]["active_theme"].empty())
			g_settingsIni.m_sections["game"]["active_theme"] = "Rhythmic Default";
		if (g_settingsIni.m_sections["game"]["engine"].empty())
			g_settingsIni.m_sections["game"]["engine"] = "Rhythmic";
		if (g_settingsIni.m_sections["game"]["menu_music"].empty())
			g_settingsIni.m_sections["game"]["menu_music"] = "1";
		if (g_settingsIni.m_sections["game"]["show_missed_notes"].empty())
			g_settingsIni.m_sections["game"]["show_missed_notes"] = "0";
		/*
		Window Section
		*/
		const GLFWvidmode *currentDisplay = glfwGetVideoMode(glfwGetPrimaryMonitor());
		if (g_settingsIni.m_sections["window"]["width"].empty())
			g_settingsIni.m_sections["window"]["width"] = std::to_string(currentDisplay->width);
		if (g_settingsIni.m_sections["window"]["height"].empty())
			g_settingsIni.m_sections["window"]["height"] = std::to_string(currentDisplay->width);
		if (g_settingsIni.m_sections["window"]["fullscreen"].empty())
			g_settingsIni.m_sections["window"]["fullscreen"] = "1";
		//if (g_settingsIni.m_sections["window"]["refresh_rate"].empty())
		//	g_settingsIni.m_sections["window"]["refresh_rate"] = std::to_string(currentDisplay->refreshRate);
		if (g_settingsIni.m_sections["window"]["vsync"].empty())
			g_settingsIni.m_sections["window"]["vsync"] = "False";
	}

	void SettingsLoad(void *)
	{
		if (IniRead(g_settingsIni, Util::GetExecutablePath() + "/settings.ini").success != 0)
		{
			SettingsGenerateFresh();
			SettingsSave();
		}

		SettingsVerify();
	}

	void SettingsSystemInit()
	{
		SettingsLoad(0);
	}
	void SettingsSystemDestroy()
	{
	}
}