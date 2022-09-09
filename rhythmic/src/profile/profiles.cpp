#include "../util/misc.hpp"
#include "../util/player_settings.hpp"

#include "profiles.hpp"

#include "../container/fretboard/fretboard_container.hpp"

#include <wge/core/logging.hpp>

#include "../util/ini_parser.hpp"

#include <fstream>

namespace Rhythmic
{

	IniFile g_profilesIni;

	PlayerSettings GetPlayerSettingsFromIni(int playerIndex)
	{
		std::string player = "player" + std::to_string(playerIndex);

		PlayerSettings settings;

		settings.noteSpeed = std::stoi(g_profilesIni.m_sections[player]["notespeed"]);
		settings.fretboard = FretboardContainer::FindFretboard(g_profilesIni.m_sections[player]["fretboard"]);
		settings.flip = std::stoi(g_profilesIni.m_sections[player]["lefty"]);
		settings.useBot = std::stoi(g_profilesIni.m_sections[player]["bot"]);
		settings.showHitRange = std::stoi(g_profilesIni.m_sections[player]["range"]);
		settings.noStrum = std::stoi(g_profilesIni.m_sections[player]["no_strum"]);
		settings.autoKick = std::stoi(g_profilesIni.m_sections[player]["auto_kick"]);

		return settings;
	}

	void ProfileListenerGet(int playerIndex, ProfileValue *value)
	{
		std::string player = "player" + std::to_string(playerIndex);

		switch (value->type) // Switch value's type
		{
		case PROFILE_NOTESPEED:
			value->m_iValue = std::stoi(g_profilesIni.m_sections[player]["notespeed"]); // value's int value is set to one in INI. Same for rest below
			break;
		case PROFILE_FRETBOARD:
			value->m_iValue = std::stoi(g_profilesIni.m_sections[player]["fretboard"]);
			break;
		case PROFILE_LEFTY:
			value->m_iValue = std::stoi(g_profilesIni.m_sections[player]["lefty"]);
			break;
		case PROFILE_BOT:
			value->m_iValue = std::stoi(g_profilesIni.m_sections[player]["bot"]);
			break;
		case PROFILE_RANGE:
			value->m_iValue = std::stoi(g_profilesIni.m_sections[player]["range"]);
			break;
		case PROFILE_NO_STRUM:
			value->m_sValue = std::stoi(g_profilesIni.m_sections[player]["no_strum"]);
			break;
		case PROFILE_AUTO_KICK:
			value->m_sValue = std::stoi(g_profilesIni.m_sections[player]["auto_kick"]);
			break;
		case PROFILE_USE_CYMBAL_1:
			value->m_sValue = std::stoi(g_profilesIni.m_sections[player]["cymbal_1"]);
			break;
		case PROFILE_USE_CYMBAL_2:
			value->m_sValue = std::stoi(g_profilesIni.m_sections[player]["cymbal_2"]);
			break;
		case PROFILE_USE_CYMBAL_3:
			value->m_sValue = std::stoi(g_profilesIni.m_sections[player]["cymbal_3"]);
			break;
		case PROFILE_4_LANE_DRUMS:
			value->m_sValue = std::stoi(g_profilesIni.m_sections[player]["4_lane_drums"]);
			break;
		}
	}
	void ProfileListenerSet(int playerIndex, const ProfileValue &value)
	{
		std::string player = "player" + std::to_string(playerIndex);

		switch (value.type)
		{
		case PROFILE_NOTESPEED:
			g_profilesIni.m_sections[player]["notespeed"] = std::to_string(value.m_iValue);
			break;
		case PROFILE_FRETBOARD:
			g_profilesIni.m_sections[player]["fretboard"] = value.m_sValue;
			break;
		case PROFILE_LEFTY:
			g_profilesIni.m_sections[player]["lefty"] = std::to_string(value.m_iValue);
			break;
		case PROFILE_BOT:
			g_profilesIni.m_sections[player]["bot"] = std::to_string(value.m_iValue);
			break;
		case PROFILE_RANGE:
			g_profilesIni.m_sections[player]["range"] = std::to_string(value.m_iValue);
			break;
		case PROFILE_NO_STRUM:
			g_profilesIni.m_sections[player]["no_strum"] = std::to_string(value.m_iValue);
			break;
		case PROFILE_AUTO_KICK:
			g_profilesIni.m_sections[player]["auto_kick"] = std::to_string(value.m_iValue);
			break;
		case PROFILE_USE_CYMBAL_1:
			g_profilesIni.m_sections[player]["cymbal_1"] = std::to_string(value.m_iValue);
			break;
		case PROFILE_USE_CYMBAL_2:
			g_profilesIni.m_sections[player]["cymbal_2"] = std::to_string(value.m_iValue);
			break;
		case PROFILE_USE_CYMBAL_3:
			g_profilesIni.m_sections[player]["cymbal_3"] = std::to_string(value.m_iValue);
			break;
		case PROFILE_4_LANE_DRUMS:
			g_profilesIni.m_sections[player]["4_lane_drums"] = std::to_string(value.m_iValue);
			break;
		}
	}
	void ProfilesGenerateFresh()
	{
		g_profilesIni.m_sections.clear();
		g_profilesIni.m_global.clear();

		IniMap player1;
		IniMap player2;
		IniMap player3;
		IniMap player4;

		IniMap maps[] = { player1, player2, player3, player4 };

		for (int i = 0; i < 4; i++)
		{
			maps[i].insert(IniEntry("notespeed", "5"));
			Fretboard *fretboard = FretboardContainer::GetFretboard(FRETBOARD_DEFAULT);
			maps[i].insert(IniEntry("fretboard", fretboard ? fretboard->m_name : ""));
			maps[i].insert(IniEntry("lefty", "0"));
			maps[i].insert(IniEntry("bot", "0"));
			maps[i].insert(IniEntry("range", "0"));
			maps[i].insert(IniEntry("no_strum", "0"));
			maps[i].insert(IniEntry("auto_kick", "0"));
			maps[i].insert(IniEntry("cymbal_1", "1"));
			maps[i].insert(IniEntry("cymbal_2", "1"));
			maps[i].insert(IniEntry("cymbal_3", "1"));
			maps[i].insert(IniEntry("4_lane_drums", "1"));

			g_profilesIni.m_sections.insert(IniSectionEntry("player" + std::to_string(i+1), maps[i]));
		}
	}
	void ProfilesListenerSave()
	{
		IniWrite(g_profilesIni, Util::GetExecutablePath() + "/profiles.ini");
	}
	void ProfilesListenerLoad(void*)
	{
		if (IniRead(g_profilesIni, Util::GetExecutablePath() + "/profiles.ini").success != 0)
		{
			ProfilesGenerateFresh();
			ProfilesListenerSave();
		}
	}

	void ProfilesSystemInit()
	{
		ProfilesListenerLoad(0);
	}
	void ProfilesSystemDestroy()
	{
	}
}