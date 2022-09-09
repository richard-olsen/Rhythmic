#include "player_container.hpp"

#include "fretboard/fretboard_container.hpp"
#include "fretboard/fretboard.hpp"

#include "../util/misc.hpp"
#include "../util/settings.hpp"

#include "../profile/profiles.hpp"

#include "../events/event_system.hpp"
#include "../events/events_common.hpp"

#include "../input/input_structures.hpp"

#include "../game/stage/stage.hpp"

#include <wge/core/logging.hpp>

#include <algorithm>

// Check to see if the player pointer is valid
#define assertValidController(player)\
{\
bool valid = false;\
for (int i = 0; i < 4; i++)\
{\
	if (player == &g_players[i])\
	{\
		valid = true;\
		break;\
	}\
}\
assert(valid);\
}

struct _key_container_player
{
	bool operator()(const Rhythmic::ContainerPlayer &player1, const Rhythmic::ContainerPlayer &player2)
	{
		return (player1.m_playerID < player2.m_playerID);
	}
};

#define sortPlayers(p) std::sort(&p[0], &p[3], _key_container_player());\
for (unsigned int i = 0; i < 4; i++) \
{\
	p[i].m_player->SetSettings(&p[i].m_settings);\
}


namespace Rhythmic
{

	namespace PlayerContainer
	{
		ContainerPlayer		g_players[4];
		unsigned int		g_numberActivePlayers;
		std::vector<int>	g_playersLostConnection;
		bool				g_playerAdded;
		bool				g_playerRemoved;

		EventListener g_lostInputListener;

		void Initialize()
		{
			g_numberActivePlayers = 0;
			for (int i = 0; i < 4; i++)
			{
				g_players[i].m_settings = GetPlayerSettingsFromIni(i + 1);

				g_players[i].m_inputHandle = INPUT_HANDLE_INVALID;

				// Set default settings

				g_players[i].m_fretboard = g_players[i].m_settings.fretboard;


				//g_players[i].m_settings.fretboard = 0;

				g_players[i].m_playerID = 5;

				g_players[i].m_player = new Player(&g_players[i], i);
			}

			g_lostInputListener = EventAddListener(ET_INPUT_DEVICE_LOST, [](void *data)
				{
					InputLostEventData *iData = (InputLostEventData *)data;

					PlayerLostConnection(GetPlayerByInputDevice(InputGetDevice(iData->lostHandle)));
				});
		}
		void Dispose()
		{
			EventRemoveListener(g_lostInputListener);
			for (int i = 0; i < 4; i++)
			{
				delete g_players[i].m_player;
			}
		}

		/*
		Returns a pointer to the assigned player.
		*/
		ContainerPlayer *AssignNewPlayer(InputDevice *device)
		{
			ContainerPlayer *player = GetPlayerByInputDevice(device);
			if (player)
				return player;

			// Unfortunately, if more than one controller loses contact, someone's fucked!
			if (!g_playersLostConnection.empty()) // Make sure the players who've lost their controllers can get them back
			{
				int i = g_playersLostConnection[0];
				
				g_players[i - 1].m_inputHandle = device->handle;
				g_players[i - 1].m_hasPlayerLostConnection = false;
				g_playersLostConnection.erase(g_playersLostConnection.begin());
				return &g_players[i - 1];
			}
			else
			{
				if (g_numberActivePlayers >= 4)
					return 0;						// There obviously can't be anymore than 4 players
				g_numberActivePlayers++;

				// Find an empty player
				for (int i = 0; i < 4; i++)
				{
					player = &g_players[i];
					if (player->m_playerID == 5)
					{
						// Cool, it's found one
						player->m_isEditingPersonalSettings = false;
						player->m_settingsPage = PLAYER_SETTINGS_PAGE_MAIN;
						player->m_hasPlayerLostConnection = false;
						player->m_playerID = g_numberActivePlayers;
						player->m_settings = GetPlayerSettingsFromIni(i + 1);
						player->m_difficulty = 3;
						player->m_inputHandle = device->handle;
						//player->m_settings.fretboard = FretboardContainer::DefaultIndex();
						player->m_fretboard = player->m_settings.fretboard;

						//if (GetGlobalStage()->IsCreated())
						//{
						//	player->m_player->EstablishPlayer();
							//player->m_player.SetFretboardTexture((*FretboardContainer::GetFretboards())[(int)player->m_settings.fretboard]);
						//}

						sortPlayers(g_players);

						g_playerAdded = true;

						return player;
					}
				}
			}
			return 0;
		}

		int GetActivePlayers()
		{
			return g_numberActivePlayers;
		}


		ContainerPlayer *GetPlayerByInputDevice(InputDevice *device)
		{
			for (int i = 0; i < 4; i++)
				if (g_players[i].m_inputHandle == device->handle)
					return &g_players[i];
			return 0;
		}
		int GetPlayerIndexByInputDevice(InputDevice *device)
		{
			for (int i = 0; i < 4; i++)
				if (g_players[i].m_inputHandle == device->handle)
					return i;
			return -1;
		}
		/*
		Returns an active player
		*/
		ContainerPlayer *GetPlayerByID(int id)
		{
			assert(id >= 0 && id < 4);

			return g_numberActivePlayers > id ? &g_players[id] : 0;
		}
		/*
		Returns a container regardless if it's active or not
		Will return player by ID as well, unless the player isn't an active player
		*/
		ContainerPlayer *GetPlayerByIndex(int index)
		{
			assert(index >= 0 && index < 4);

			return &g_players[index];
		}

		/*
		Passing back the container, PlayerContainer will unassign the player
		*/
		void UnassignPlayer(ContainerPlayer *player)
		{
			assertValidController(player);

			int pID = player->m_playerID;

			//player->m_inputDevice.device = 0;
			//if (GetGlobalStage()->IsCreated())
			if (player->m_player->IsPlayerEstablished())
				player->m_player->DisestablishPlayer();

			// Subtract 1 from almost all lost connection players
			for (auto it = g_playersLostConnection.begin(); it != g_playersLostConnection.end(); it++)
			{
				if ((*it) > pID)
					(*it)--;
				else if ((*it) == pID)
					it = g_playersLostConnection.erase(it);
			}

			// Just to make sure there isn't a fourth player if there isn't a third player
			for (int i = 0; i < 4; i++)
			{
				if (g_players[i].m_playerID > pID && g_players[i].m_playerID != 5)
					g_players[i].m_playerID--;
			}

			player->m_settings = PlayerSettings(); // Reset settings
			player->m_isEditingPersonalSettings = false;
			player->m_hasPlayerLostConnection = false;
			player->m_inputHandle = INPUT_HANDLE_INVALID;

			player->m_playerID = 5;

			sortPlayers(g_players);

			g_numberActivePlayers--;

			g_playerRemoved = true;
		}

		void PlayerLostConnection(ContainerPlayer *player)
		{
			for (int i = 0; i < 4; i++)
			{
				if (player == (&g_players[i]))
				{
					player->m_inputHandle = INPUT_HANDLE_INVALID;
					player->m_hasPlayerLostConnection = true;
					StageSystem *stage = StageSystem::GetInstance();
					if (stage->IsPlayerEstablished(player->m_player))
					{
						stage->SetPause(true);
					}
					g_playersLostConnection.push_back(i + 1);
					break;
				}
			}
		}

		/*
		Updates all players that are currently active
		*/
		void UpdateActivePlayers(const float &delta, const float &timeOffset)
		{
			for (int i = 0; i < 4; i++)
			{
				if (g_players[i].m_playerID > 0 && g_players[i].m_playerID < 5)
					g_players[i].m_player->Update(delta, timeOffset);
			}
		}

		void EstablishActivePlayers(Stage *stage)
		{
			SettingValue value;
			value.type = SETTING_GAME_CALIBRATION_AUDIO;
			SettingsGet(&value);
			float audioCalibration = value.m_iValue * 0.001f;

			value.type = SETTING_GAME_CALIBRATION_VIDEO;
			SettingsGet(&value);
			float videoCalibration = value.m_iValue * 0.001f;

			for (int i = 0; i < 4; i++)
			{
				if (g_players[i].m_playerID > 0 && g_players[i].m_playerID < 5)
				{
					g_players[i].m_player->EstablishPlayer();
				}
			}
		}

		void DisestablishActivePlayers()
		{
			for (int i = 0; i < 4; i++)
			{
				if (g_players[i].m_playerID > 0 && g_players[i].m_player->IsPlayerEstablished())
					g_players[i].m_player->DisestablishPlayer();
			}
		}

		void UpdateAllControllers()
		{
			g_playerAdded = false;
			g_playerRemoved = false;
			for (int i = 0; i < 4; i++)
			{
				//if (g_players[i].m_playerID > 0)
				//{
				//	//g_players[i].m_controller.UpdateAllButtons();
				//	/*for (int j = 0; j < Input::BUTTON_size; j++)
				//	{
				//		g_players[i].m_inputDevice.hitDataButton[j].pressed = false;
				//		g_players[i].m_inputDevice.hitDataButton[j].released = false;
				//	}
				//	for (int j = 0; j < Input::AXIS_size; j++)
				//	{
				//		g_players[i].m_inputDevice.hitDataAxis[j].pressed = false;
				//	}*/
				//}
			}
		}
		bool PlayerAssigned()
		{
			return g_playerAdded;
		}
		bool PlayerUnassigned()
		{
			return g_playerRemoved;
		}
		int GetPlayerLostConnection()
		{
			if (g_playersLostConnection.size() != 0)
				return g_playersLostConnection[0];
			return 5;
		}
	}
}