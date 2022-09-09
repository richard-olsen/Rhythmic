#ifndef PLAYER_CONTAINER_H_
#define PLAYER_CONTAINER_H_

#include "../input/input.hpp"
#include "../game/player.hpp"

namespace Rhythmic
{
	enum PlayerSettingsPage
	{
		PLAYER_SETTINGS_PAGE_PROFILE,			// Allows player to select profile
		PLAYER_SETTINGS_PAGE_PROFILE_CREATION,	// Allows player to name and create a profile

		PLAYER_SETTINGS_PAGE_BINDING,			// After creating a profile, players are met with this menu
		PLAYER_SETTINGS_PAGE_BINDING_CUSTOM,	// Binding menu with player defined bindings
		PLAYER_SETTINGS_PAGE_BINDING_STOCK,		// Binding menu with stock bindings

		PLAYER_SETTINGS_PAGE_MAIN,				// The main menu
		PLAYER_SETTINGS_PAGE_COLOR,				// Colors editing menu
		PLAYER_SETTINGS_PAGE_PROFILE_OPTIONS,	// Options for the player's profile
		PLAYER_SETTINGS_PAGE_DRUMS,				// Menu for drums
		PLAYER_SETTINGS_PAGE_GUITAR				// Menu for guitar
	};
	struct ContainerPlayer
	{
		unsigned int				m_playerID;
		InputDeviceHandle		m_inputHandle;

		Player							*m_player;
		PlayerSettings			m_settings;
		bool								m_hasPlayerLostConnection;
		bool								m_isEditingPersonalSettings;
		PlayerSettingsPage	m_settingsPage;

		FretboardHandle			m_fretboard;
		int									m_modifiers;
		int									m_instrument;
		bool								m_drumsUseCymbals;
		int									m_difficulty;
		// Will contain player profile
	};

	class Stage;
	namespace PlayerContainer
	{
		void Initialize();
		void Dispose();

		/*
		Returns a pointer to the assigned player. 

		Players are not allocated on the heap!
		*/
		ContainerPlayer *AssignNewPlayer(InputDevice *device);
		
		/*
		Tells the PlayerContainer that a player has lost connection to their controller. Next assign call will take it's place

		This is done to not remove the player from the game, if the controller happens to die on them
		*/
		void PlayerLostConnection(ContainerPlayer *player);

		/*
		Returns the amount of active players
		*/
		int GetActivePlayers();

		/*
		Returns an active player using the requested device
		*/
		ContainerPlayer *GetPlayerByInputDevice(InputDevice *device);
		/*
		Returns an active player's index using the requested device
		*/
		int GetPlayerIndexByInputDevice(InputDevice *device);
		/*
		Returns an active player
		*/
		ContainerPlayer *GetPlayerByID(int id);
		/*
		Returns a container regardless if it's active or not
		*/
		ContainerPlayer *GetPlayerByIndex(int index);

		/*
		Passing back the container, PlayerContainer will unassign the player
		*/
		void UnassignPlayer(ContainerPlayer *player);

		/*
		Updates all players that are currently active
		*/
		void UpdateActivePlayers(const float &delta, const float &timeOffset);

		/*
		Establishes all active players
		*/
		void EstablishActivePlayers(Stage *stage);

		/*
		Disestablishes all active players
		*/
		void DisestablishActivePlayers();

		/*
		Updates active players controllers
		*/
		void UpdateAllControllers();

		/*
		Returns true if a player has been assigned
		*/
		bool PlayerAssigned();
		/*
		Returns true if a player has been unassigned
		*/
		bool PlayerUnassigned();

		/*
		Returns the first player in the list of ones that lost controller connection
		*/
		int GetPlayerLostConnection();
	}
}

#endif

