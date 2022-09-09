#include "../util/player_settings.hpp"

#ifndef PROFILES_HPP
#define PROFILES_HPP

namespace Rhythmic
{
	enum ProfileType
	{
		PROFILE_NOTESPEED,
		PROFILE_FRETBOARD,
		PROFILE_LEFTY,
		PROFILE_BOT,
		PROFILE_RANGE,
		PROFILE_NO_STRUM,
		PROFILE_AUTO_KICK,
		PROFILE_USE_CYMBAL_1,
		PROFILE_USE_CYMBAL_2,
		PROFILE_USE_CYMBAL_3,
		PROFILE_4_LANE_DRUMS,
	};

	struct ProfileValue
	{
		ProfileType type; // Value type

		int m_iValue; // Int type
		float m_fValue; // Float type
		bool m_bValue; // Bool type
		std::string m_sValue; // String type
	};

	PlayerSettings GetPlayerSettingsFromIni(int playerIndex);

	void ProfileListenerGet(int playerIndex, ProfileValue* value);
	void ProfileListenerSet(int playerIndex, const ProfileValue& value);
	void ProfilesListenerSave();
	void ProfilesSystemInit();
	void ProfilesSystemDestroy();
}

#endif