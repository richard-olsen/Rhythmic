#ifndef PLAYER_SETTINGS_H_
#define PLAYER_SETTINGS_H_

#include <string>

#define PLAYER_SETTINGS_COUNT 11

namespace Rhythmic
{
	struct PlayerSetting
	{
		bool isBool;
		int data;

		inline PlayerSetting() :
			isBool(false),
			data(0)
		{ }
		inline PlayerSetting(int i) :
			isBool(false),
			data(i)
		{ }
		inline PlayerSetting(bool b) :
			isBool(true),
			data(b)
		{ }

		inline void operator=(int i)
		{
			isBool = false;
			data = i;
		}
		inline void operator=(bool b)
		{
			isBool = true;
			data = b;
		}

		inline std::string str()
		{
			if (isBool) // The only reason for isBool is for this function
			{
				if (data)
					return "Yes";
				else
					return "No";
			}
			// Otherwise, it'll return an integer
			return std::to_string(data);
		}

		inline void operator+=(int i) { isBool = false; data += i; }
		inline void operator-=(int i) { isBool = false; data -= i; }

		inline operator int() { return data; }
		inline operator bool() { return (bool)data; }
		inline operator float() { return (float)data; }
	};

	struct PlayerSettings
	{
		//int			noteSpeed;		// Controls highway speed
		//int			fretboard;		// Determines the fretboard the player will have
		//bool		useBot;			// Determines whether the player will be using a bot or not (this setting can not be changed while a song is loaded)
		//bool		noStrum;		// Determines whether the player is required to strum (for guitar/bass only)
		//bool		autoKick;		// Determines whether the player needs to hit kick (for drums only)
		inline PlayerSettings() :
			noteSpeed(5),
			fretboard(),
			flip(false),
			useBot(false),
			noStrum(false),
			fourLaneDrums(true),
			useCymbal1(true),
			useCymbal2(true),
			useCymbal3(true),
			autoKick(false),
			showHitRange(false)
		{}
		inline ~PlayerSettings() {}
		
		PlayerSetting	noteSpeed;
		PlayerSetting	fretboard;
		PlayerSetting	flip;
		PlayerSetting	useBot;
		PlayerSetting	noStrum;
		PlayerSetting	fourLaneDrums;
		PlayerSetting	useCymbal1;
		PlayerSetting	useCymbal2;
		PlayerSetting	useCymbal3;
		PlayerSetting	autoKick;
		PlayerSetting	showHitRange;
	};
}

#endif

