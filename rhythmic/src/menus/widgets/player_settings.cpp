#include "player_settings.hpp"

#include "../../profile/profiles.hpp"

#include "../../engine/include/wge/core/logging.hpp"

#include "../../container/fretboard/fretboard.hpp"
#include "../../container/fretboard/fretboard_container.hpp"

#include "../../container/secret/secret_container.hpp"

#include "../../container/player_container.hpp"
#include "../menu_system.hpp"

namespace Rhythmic
{
	constexpr int PAGE_CLEARED = 0;
	constexpr int PAGE_OPTIONS = 1;
	constexpr int PAGE_INSTRUMENT_SELECT = 2;

	WidgetPlayerSettings::WidgetPlayerSettings()
	{
		for (int i = 0; i < 4; i++)
		{
			m_buttonFields[i].buttonField.SetFocusPlayer(i);
			m_buttonFields[i].buttonField.ClearButtons();
			m_buttonFields[i].buttonField.SetAllowButtonScroll(false);
			m_buttonFields[i].page = 0;

			m_buttonFields[i].buttonField.SetPosition(glm::vec2((0.25f * i) + 0.125f, 0.6f));
			m_buttonFields[i].buttonField.SetScale(glm::vec2(0.12f, 0.25f));
			m_buttonFields[i].buttonField.SetButtonYScale(0.015f);
		}

		SetCanvasChangeCallback(
			[this](Canvas *canvas) 
			{
				for (int i = 0; i < 4; i++)
					m_buttonFields[i].buttonField.SetCanvas(canvas);
			}
		);
	}
	WidgetPlayerSettings::~WidgetPlayerSettings()
	{ }

	void WidgetPlayerSettings::Update(float delta)
	{
		int playersNotEditingScreens = 0;
		for (int i = 0; i < 4; i++)
		{
			ContainerPlayer *player = PlayerContainer::GetPlayerByIndex(i);

			if (!player->m_isEditingPersonalSettings)
			{
				playersNotEditingScreens++;
				ClearButtons(i);
				continue;
			}

			/*if (player->m_inputDevice.hitDataButton[Input::ButtonsGame::BUTTON_BACK].pressed)
			{
				player->m_isEditingPersonalSettings = false;
				playersNotEditingScreens++;
				ClearButtons(i);
				continue;
			}*/

			if (player->m_settingsPage == PLAYER_SETTINGS_PAGE_BINDING)
				SetButtonsSelectInstrument(i);

			else if (player->m_settingsPage == PLAYER_SETTINGS_PAGE_MAIN)
				SetButtonsSettings(i);

			else
				ClearButtons(i);

			m_buttonFields[i].buttonField.Update(delta);
		}
		if (playersNotEditingScreens == 4)
			MenuSystem::GetInstance()->QueuePopMenu();
	}
	void WidgetPlayerSettings::ProcessInput(InputEventData *input, int playerIndex)
	{
		if (playerIndex < 0 || playerIndex >= 4) // Invalid player, return
			return;

		ContainerPlayer *containerPlayer = PlayerContainer::GetPlayerByIndex(playerIndex);

		if (input->m_buttonsPressed & (1 << INPUT_BUTTON_RED))
		{
			containerPlayer->m_isEditingPersonalSettings = false;
			ClearButtons(playerIndex);
			ProfilesListenerSave();
			return;
		}

		m_buttonFields[playerIndex].buttonField.ProcessInput(input, playerIndex);
	}
	void WidgetPlayerSettings::Render(float interpolation)
	{
		for (int i = 0; i < 4; i++)
			m_buttonFields[i].buttonField.Render(interpolation);
	}


	static std::string nullValue = "NULL"; // For the fretboard in case there are none installed
	void WidgetPlayerSettings::SetButtonsSettings(int index)
	{
		if (m_buttonFields[index].page == PAGE_OPTIONS)
			return;

		m_buttonFields[index].page = PAGE_OPTIONS;
		m_buttonFields[index].buttonField.ClearButtons();

		ContainerPlayer *player = PlayerContainer::GetPlayerByIndex(index);
		PlayerSettings &settings = player->m_settings;

		m_buttonFields[index].buttonField.AddButton(new WidgetButton("Note Speed: ", [index, &settings]() {
				if (settings.noteSpeed.data <= 20 && settings.noteSpeed.data >= 1)
				{
					ProfileValue value;
					value.type = PROFILE_NOTESPEED;
					value.m_iValue = settings.noteSpeed;
					ProfileListenerSet(index + 1, value);
				}
				return BUTTON_TOGGLE_SCROLL;	// Tells the button field that the button wants access to the scroll values
			}, 0, [index,&settings](int scroll) {
				// Scroll func
				settings.noteSpeed += scroll;

				if (settings.noteSpeed.data > 20 && !SecretContainer::IsSecretUnlocked(INFINITE_NOTESPEED)) settings.noteSpeed -= scroll;
				if (settings.noteSpeed.data < 1 && !SecretContainer::IsSecretUnlocked(NEGATIVE_NOTESPEED)) settings.noteSpeed -= scroll;

				return BUTTON_FINISHED;
			}));

		Fretboard* fretboard = FretboardContainer::GetFretboard(player->m_fretboard);

		m_buttonFields[index].buttonField.AddButton(new WidgetButton("Fretboard: ", []() {
			return BUTTON_TOGGLE_SCROLL; 
		}, 0, [player, &settings, index, this](int scroll) {
				if (scroll > 0)
					player->m_fretboard = FretboardContainer::GetNextValidHandle(player->m_fretboard);
				if (scroll < 0)
					player->m_fretboard = FretboardContainer::GetPrevValidHandle(player->m_fretboard);

				player->m_player->SetFretboardTexture(FretboardContainer::GetFretboard(player->m_fretboard));

				Fretboard *fretboard = FretboardContainer::GetFretboard(player->m_fretboard);

				ProfileValue value;
				value.type = PROFILE_FRETBOARD;
				value.m_sValue = fretboard ? fretboard->m_name : "";
				ProfileListenerSet(index + 1, value);

				if (fretboard == 0)
					m_buttonFields[index].buttonField.Get(1)->GetLabel().TagStringAtEnd(&nullValue);
				else
					m_buttonFields[index].buttonField.Get(1)->GetLabel().TagStringAtEnd(&fretboard->m_name);
			return BUTTON_FINISHED;
		}));

		m_buttonFields[index].buttonField.AddButton(new WidgetButton("Lefty Flip: ", [index, &settings]() {
				settings.flip = !(bool)settings.flip;

				ProfileValue value;
				value.type = PROFILE_LEFTY;
				value.m_iValue = settings.flip;
				ProfileListenerSet(index + 1, value);

				return BUTTON_FINISHED;
			}, 0));

		m_buttonFields[index].buttonField.AddButton(new WidgetButton("Use Bot: ", [index,&settings]() {
				settings.useBot = !(bool)settings.useBot;

				ProfileValue value;
				value.type = PROFILE_BOT;
				value.m_iValue = settings.useBot;
				ProfileListenerSet(index + 1, value);

				return BUTTON_FINISHED;
			}, 0));
		m_buttonFields[index].buttonField.AddButton(new WidgetButton("Show Range: ", [index, &settings]() {
				settings.showHitRange = !(bool)settings.showHitRange;

				ProfileValue value;
				value.type = PROFILE_RANGE;
				value.m_iValue = settings.showHitRange;
				ProfileListenerSet(index + 1, value);

				return BUTTON_FINISHED;
			}, 0));
		m_buttonFields[index].buttonField.AddButton(new WidgetButton("No Strum: ", [index, &settings]() {
				settings.noStrum = !(bool)settings.noStrum;

				ProfileValue value;
				value.type = PROFILE_NO_STRUM;
				value.m_iValue = settings.noStrum;
				ProfileListenerSet(index + 1, value);

				return BUTTON_FINISHED;
			}, 0));
		m_buttonFields[index].buttonField.AddButton(new WidgetButton("Auto Kick: ", [index, &settings]()
			{
				settings.autoKick = !(bool)settings.autoKick;

				ProfileValue value;
				value.type = PROFILE_AUTO_KICK;
				value.m_iValue = settings.autoKick;
				ProfileListenerSet(index + 1, value);

				return BUTTON_FINISHED;
			}, 0));

		m_buttonFields[index].buttonField.AddButton(new WidgetButton("Cymbal Yellow: ", [index, &settings]()
			{
				settings.useCymbal1 = !(bool)settings.useCymbal1;

				ProfileValue value;
				value.type = PROFILE_USE_CYMBAL_1;
				value.m_iValue = settings.useCymbal1;
				ProfileListenerSet(index + 1, value);

				return BUTTON_FINISHED;
			}, 0));

		m_buttonFields[index].buttonField.AddButton(new WidgetButton("Cymbal Blue: ", [index, &settings]()
			{
				settings.useCymbal2 = !(bool)settings.useCymbal2;

				ProfileValue value;
				value.type = PROFILE_USE_CYMBAL_2;
				value.m_iValue = settings.useCymbal2;
				ProfileListenerSet(index + 1, value);

				return BUTTON_FINISHED;
			}, 0));

		m_buttonFields[index].buttonField.AddButton(new WidgetButton("Cymbal Green: ", [index, &settings]()
			{
				settings.useCymbal3 = !(bool)settings.useCymbal3;

				ProfileValue value;
				value.type = PROFILE_USE_CYMBAL_3;
				value.m_iValue = settings.useCymbal3;
				ProfileListenerSet(index + 1, value);

				return BUTTON_FINISHED;
			}, 0));

		m_buttonFields[index].buttonField.AddButton(new WidgetButton("4 Lane Drums: ", [index, &settings]()
			{
				settings.fourLaneDrums = !(bool)settings.fourLaneDrums;

				ProfileValue value;
				value.type = PROFILE_4_LANE_DRUMS;
				value.m_iValue = settings.fourLaneDrums;
				ProfileListenerSet(index + 1, value);

				return BUTTON_FINISHED;
			}, 0));

		m_buttonFields[index].buttonField.AddButton(new WidgetButton("Leave", [index, player]() {
				PlayerContainer::UnassignPlayer(player);
				return BUTTON_FINISHED; 
			}, 0));

		m_buttonFields[index].buttonField.Get(0)->GetLabel().TagIntAtEnd(&settings.noteSpeed.data);
		m_buttonFields[index].buttonField.Get(1)->GetLabel().TagStringAtEnd(&fretboard->m_name); // fix this it doesnt update
		m_buttonFields[index].buttonField.Get(2)->GetLabel().TagBoolAtEnd(&settings.flip.data);
		m_buttonFields[index].buttonField.Get(3)->GetLabel().TagBoolAtEnd(&settings.useBot.data);
		m_buttonFields[index].buttonField.Get(4)->GetLabel().TagBoolAtEnd(&settings.showHitRange.data);
		m_buttonFields[index].buttonField.Get(5)->GetLabel().TagBoolAtEnd(&settings.noStrum.data);
		m_buttonFields[index].buttonField.Get(6)->GetLabel().TagBoolAtEnd(&settings.autoKick.data);
		m_buttonFields[index].buttonField.Get(7)->GetLabel().TagBoolAtEnd(&settings.useCymbal1.data);
		m_buttonFields[index].buttonField.Get(8)->GetLabel().TagBoolAtEnd(&settings.useCymbal2.data);
		m_buttonFields[index].buttonField.Get(9)->GetLabel().TagBoolAtEnd(&settings.useCymbal3.data);
		m_buttonFields[index].buttonField.Get(10)->GetLabel().TagBoolAtEnd(&settings.fourLaneDrums.data);

	}
	void WidgetPlayerSettings::SetButtonsSelectInstrument(int index)
	{
		if (m_buttonFields[index].page == PAGE_INSTRUMENT_SELECT)
			return;

		m_buttonFields[index].page = PAGE_INSTRUMENT_SELECT;
		m_buttonFields[index].buttonField.ClearButtons();

		/*m_buttonFields[index].buttonField.AddButton(new WidgetButton("RB2 Drums 360", [index]() {
				ContainerPlayer *player = PlayerContainer::GetPlayerByIndex(index);
				player->m_handleInput = handleInputDrumsRB;
				player->m_inputDevice.bindings = Input::GetDefaultBinding(Input::INPUT_TYPE_DRUMS_RB_360);
				player->m_settingsPage = PLAYER_SETTINGS_PAGE_MAIN;
				return BUTTON_FINISHED;
			}, 0));

		m_buttonFields[index].buttonField.AddButton(new WidgetButton("RB2 Drums WII", [index]() {
				ContainerPlayer *player = PlayerContainer::GetPlayerByIndex(index);
				player->m_handleInput = handleInputDrumsRB;
				player->m_inputDevice.bindings = Input::GetDefaultBinding(Input::INPUT_TYPE_DRUMS_RB_WII);
				player->m_settingsPage = PLAYER_SETTINGS_PAGE_MAIN;
				return BUTTON_FINISHED;
			}, 0));

		m_buttonFields[index].buttonField.AddButton(new WidgetButton("Guitar 360", [index]() {
				ContainerPlayer *player = PlayerContainer::GetPlayerByIndex(index);
				player->m_handleInput = handleInputGuitar5;
				player->m_inputDevice.bindings = Input::GetDefaultBinding(Input::INPUT_TYPE_GUITAR_5);
				player->m_settingsPage = PLAYER_SETTINGS_PAGE_MAIN;
				player->m_isEditingPersonalSettings = false;
				return BUTTON_FINISHED;
			}, 0));*/
	}

	void WidgetPlayerSettings::OnMouseMove(float x, float y)
	{
		for (int i = 0; i < 4; i++)
			m_buttonFields[i].buttonField.OnMouseMove(x, y);
	}
	void WidgetPlayerSettings::OnMouseClick(int button, float x, float y)
	{
		for (int i = 0; i < 4; i++)
			m_buttonFields[i].buttonField.OnMouseClick(button, x, y);
	}
	void WidgetPlayerSettings::OnScroll(float x, float y)
	{
		for (int i = 0; i < 4; i++)
			m_buttonFields[i].buttonField.OnScroll(x, y);
	}

	void WidgetPlayerSettings::ClearButtons(int index)
	{
		if (m_buttonFields[index].page == PAGE_CLEARED)
			return;

		m_buttonFields[index].page = PAGE_CLEARED;
		m_buttonFields[index].buttonField.ClearButtons();
	}
}