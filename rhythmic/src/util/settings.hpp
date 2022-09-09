#ifndef SETTINGS_H_
#define SETTINGS_H_

#include <string>

namespace Rhythmic
{
	enum SettingType
	{
		SETTING_GAME_CALIBRATION_AUDIO,
		SETTING_GAME_CALIBRATION_VIDEO,
		SETTING_GAME_SONGLIST_DIRECTORY,
		SETTING_GAME_ACTIVE_THEME,
		SETTING_GAME_ENGINE,
		SETTING_GAME_MENU_MUSIC,
		SETTING_GAME_SHOW_MISSED_NOTES,

		SETTING_WINDOW_WIDTH,
		SETTING_WINDOW_HEIGHT,
		SETTING_WINDOW_FULLSCREEN,
		SETTING_WINDOW_VSYNC,
		SETTING_WINDOW_REFRESH_RATE,

		SETTING_AUDIO_MASTER,
		SETTING_AUDIO_MUSIC,
		SETTING_AUDIO_SFX,
		SETTING_AUDIO_MENU_MUSIC_VOLUME

	};

	struct SettingValue
	{
		SettingType type; // Value type

		int m_iValue; // Int type
		float m_fValue; // Float type
		bool m_bValue; // Bool type
		std::string m_sValue; // String type
	};

	void SettingsGet(SettingValue *value);
	void SettingsSet(const SettingValue &value);
	void SettingsSave();
	void SettingsSystemInit();
	void SettingsSystemDestroy();
}

#endif
