#include "player_prep.hpp"

#include "../../rendering/render_manager.hpp"

#include "../../container/player_container.hpp"

#include "../menu_system.hpp"
#include "../../game/stage/stage.hpp"

#include "song_start.hpp"

#include "../../game/modifiers.hpp"

namespace Rhythmic
{
	WidgetPlayerPrep::WidgetPlayerPrep(Chart *chart, int playerIndex, WidgetSongStart *songStart) :
		WidgetBase(),
		m_frame(0,0, RenderManager::GetSheet("ui_elements")),
		m_playerIndex(playerIndex),
		m_chart(chart),
		m_page(-1),
		m_songStart(songStart)
	{

		m_buttons.SetPosition(glm::vec2((0.25f * playerIndex) + 0.125f, 0.4f));
		m_buttons.SetScale(glm::vec2(0.12f, 0.4f));
		m_buttons.SetFocusPlayer(playerIndex);
		m_buttons.SetAllowButtonScroll(false);

		m_frame.SetPosition(m_buttons.GetPosition() + glm::vec2(0, 0.2f));
		m_frame.SetScale(glm::vec2(0.12f, 0.24f));

		SetCanvasChangeCallback([this](Canvas *canvas) { m_buttons.SetCanvas(canvas); m_frame.SetCanvas(canvas); });
	}
	WidgetPlayerPrep::~WidgetPlayerPrep()
	{

	}

	void WidgetPlayerPrep::Update(float delta)
	{
		ContainerPlayer *player = PlayerContainer::GetPlayerByIndex(m_playerIndex);

		if (player->m_playerID != 5)
		{
			if (m_page == -1)
				ChangePage(0);
			if (m_page == -2)
				ChangePage(1);
			if (m_page == -3)
				ChangePage(2);
			if (m_page == -4)
				ChangePage(3);

			m_buttons.Update(delta);
		}
	}
	void WidgetPlayerPrep::Render(float interpolation)
	{
		ContainerPlayer *player = PlayerContainer::GetPlayerByIndex(m_playerIndex);

		if (player->m_playerID != 5)
		{
			m_frame.Render(interpolation);
			m_buttons.Render(interpolation);
		}
	}
	void WidgetPlayerPrep::ProcessInput(InputEventData *input, int playerIndex)
	{
		if (playerIndex != m_playerIndex)
			return;

		if (input->m_buttonsPressed & (1 << INPUT_BUTTON_RED))
		{
			int newPage = m_page - 1;

			if (newPage >= 0)
				ChangePage(newPage);
			else if (m_playerIndex == 0)
			{
				StageSystem::GetInstance()->Unload();
				MenuSystem::GetInstance()->QueuePopMenu();
			}

			return;
		}

		m_buttons.ProcessInput(input, playerIndex);
	}

	void WidgetPlayerPrep::OnScroll(float dx, float dy)
	{
		m_buttons.OnScroll(dx, dy);
	}
	void WidgetPlayerPrep::OnMouseMove(float x, float y)
	{
		m_buttons.OnMouseMove(x, y);
	}

	void WidgetPlayerPrep::OnMouseClick(int mouse, float x, float y)
	{
		m_buttons.OnMouseClick(mouse, x, y);
	}


	void WidgetPlayerPrep::ChangePage(int page)
	{
		if (m_page == page)
			return;
		m_buttons.ClearButtons();

		if (page == 0)
			SetPageInstrument();
		if (page == 1)
			SetPageDifficulty();
		if (page == 2)
			SetPageModifiers();
		if (page == 3)
		{
			m_songStart->IncreasePlayersReady();
		}
		m_page = page;
	}
	void WidgetPlayerPrep::SetPageInstrument()
	{
		ContainerPlayer *player = PlayerContainer::GetPlayerByIndex(m_playerIndex);

		if (m_chart->supports[INSTRUMENT_TYPE_GUITAR])
			m_buttons.AddButton(new WidgetButton("Guitar", [this, player]() { player->m_instrument = INSTRUMENT_TYPE_GUITAR; player->m_drumsUseCymbals = false; m_page = -2; return BUTTON_FINISHED; }));

		if (m_chart->supports[INSTRUMENT_TYPE_BASS])
			m_buttons.AddButton(new WidgetButton("Bass", [this, player]() { player->m_instrument = INSTRUMENT_TYPE_BASS; player->m_drumsUseCymbals = false; m_page = -2; return BUTTON_FINISHED; }));

		if (m_chart->supports[INSTRUMENT_TYPE_6FRET])
			m_buttons.AddButton(new WidgetButton("6 Fret Guitar", [this, player]() { player->m_instrument = INSTRUMENT_TYPE_6FRET; player->m_drumsUseCymbals = false; m_page = -2; return BUTTON_FINISHED; }));

		if (m_chart->supports[INSTRUMENT_TYPE_6FRETBASS])
			m_buttons.AddButton(new WidgetButton("6 Fret Bass", [this, player]() { player->m_instrument = INSTRUMENT_TYPE_6FRETBASS; player->m_drumsUseCymbals = false; m_page = -2; return BUTTON_FINISHED; }));

		if (m_chart->supports[INSTRUMENT_TYPE_DRUMS])
		{
			m_buttons.AddButton(new WidgetButton("Drums", [this, player]() { player->m_instrument = INSTRUMENT_TYPE_DRUMS; player->m_drumsUseCymbals = false; m_page = -2; return BUTTON_FINISHED; }));
			if (m_chart->songData.supportsProDrums)
				m_buttons.AddButton(new WidgetButton("Pro Drums", [this, player]() { player->m_instrument = INSTRUMENT_TYPE_DRUMS; player->m_drumsUseCymbals = true; m_page = -2; return BUTTON_FINISHED; }));
		}
	}
	void WidgetPlayerPrep::SetPageDifficulty()
	{
		ContainerPlayer *player = PlayerContainer::GetPlayerByIndex(m_playerIndex);

		if (!m_chart->notes[player->m_instrument][NOTES_EXPERT].empty())
			m_buttons.AddButton(new WidgetButton("Expert", [this, player]() { player->m_difficulty = NOTES_EXPERT; m_page = -3; return BUTTON_FINISHED; }));
		if (!m_chart->notes[player->m_instrument][NOTES_HARD].empty())
			m_buttons.AddButton(new WidgetButton("Hard", [this, player]() { player->m_difficulty = NOTES_HARD; m_page = -3; return BUTTON_FINISHED; }));
		if (!m_chart->notes[player->m_instrument][NOTES_MEDIUM].empty())
			m_buttons.AddButton(new WidgetButton("Medium", [this, player]() { player->m_difficulty = NOTES_MEDIUM; m_page = -3; return BUTTON_FINISHED; }));
		if (!m_chart->notes[player->m_instrument][NOTES_EASY].empty())
			m_buttons.AddButton(new WidgetButton("Easy", [this, player]() { player->m_difficulty = NOTES_EASY; m_page = -3; return BUTTON_FINISHED; }));
	}

	void WidgetPlayerPrep::SetPageModifiers() {
		ContainerPlayer *player = PlayerContainer::GetPlayerByIndex(m_playerIndex);

		m_buttons.AddButton(new WidgetButton("Ready", [this]() { m_page = -4; return BUTTON_FINISHED; }));

		// Guitar
		if (player->m_instrument == INSTRUMENT_TYPE_GUITAR || player->m_instrument == INSTRUMENT_TYPE_BASS || player->m_instrument == INSTRUMENT_TYPE_6FRET || player->m_instrument == INSTRUMENT_TYPE_6FRETBASS) {
			// All note types
			{
				WidgetButton *button = new WidgetButton("All Strums");
				if (player->m_modifiers & ALL_STRUMS) button->SetForceOn(true);

				button->SetOnClick([this, player, button]() {
					if (player->m_modifiers & ALL_STRUMS) player->m_modifiers &= ~ALL_STRUMS;
					else player->m_modifiers |= ALL_STRUMS;

					button->SetForceOn(player->m_modifiers & ALL_STRUMS);
					return BUTTON_FINISHED;
				});
				m_buttons.AddButton(button);

				button = new WidgetButton("All HOPOs");
				if (player->m_modifiers & ALL_HOPOS) button->SetForceOn(true);

				button->SetOnClick([this, player, button]() {
					if (player->m_modifiers & ALL_HOPOS) player->m_modifiers &= ~ALL_HOPOS;
					else player->m_modifiers |= ALL_HOPOS;

					button->SetForceOn(player->m_modifiers & ALL_HOPOS);
					return BUTTON_FINISHED;
				});
				m_buttons.AddButton(button);

				button = new WidgetButton("All Taps");
				if (player->m_modifiers & ALL_TAPS) button->SetForceOn(true);

				button->SetOnClick([this, player, button]() {
					if (player->m_modifiers & ALL_TAPS) player->m_modifiers &= ~ALL_TAPS;
					else player->m_modifiers |= ALL_TAPS;

					button->SetForceOn(player->m_modifiers & ALL_TAPS);
					return BUTTON_FINISHED;
				});
				m_buttons.AddButton(button);

				button = new WidgetButton("All Opens");
				if (player->m_modifiers & ALL_OPENS) button->SetForceOn(true);

				button->SetOnClick([this, player, button]() {
					if (player->m_modifiers & ALL_OPENS) player->m_modifiers &= ~ALL_OPENS;
					else player->m_modifiers |= ALL_OPENS;

					button->SetForceOn(player->m_modifiers & ALL_OPENS);
					return BUTTON_FINISHED;
				});
				m_buttons.AddButton(button);
			}
			// Other modifiers
			{
				WidgetButton *button = new WidgetButton("HOPOs To Taps");
				if (player->m_modifiers & HOPOS_TO_TAPS) button->SetForceOn(true);

				button->SetOnClick([this, player, button]() {
					if (player->m_modifiers & HOPOS_TO_TAPS) player->m_modifiers &= ~HOPOS_TO_TAPS;
					else player->m_modifiers |= HOPOS_TO_TAPS;

					button->SetForceOn(player->m_modifiers & HOPOS_TO_TAPS);
					return BUTTON_FINISHED;
				});
				m_buttons.AddButton(button);

				button = new WidgetButton("Note Shuffle");
				if (player->m_modifiers & NOTE_SHUFFLE) button->SetForceOn(true);

				button->SetOnClick([this, player, button]() {
					if (player->m_modifiers & NOTE_SHUFFLE) player->m_modifiers &= ~NOTE_SHUFFLE;
					else player->m_modifiers |= NOTE_SHUFFLE;

					button->SetForceOn(player->m_modifiers & NOTE_SHUFFLE);
					return BUTTON_FINISHED;
				});
				m_buttons.AddButton(button);

				button = new WidgetButton("Mirror Mode");
				if (player->m_modifiers & MIRROR_MODE) button->SetForceOn(true);

				button->SetOnClick([this, player, button]() {
					if (player->m_modifiers & MIRROR_MODE) player->m_modifiers &= ~MIRROR_MODE;
					else player->m_modifiers |= MIRROR_MODE;

					button->SetForceOn(player->m_modifiers & MIRROR_MODE);
					return BUTTON_FINISHED;
				});
				m_buttons.AddButton(button);

				button = new WidgetButton("Double Notes");
				if (player->m_modifiers & DOUBLE_NOTES) button->SetForceOn(true);

				button->SetOnClick([this, player, button]() {
					if (player->m_modifiers & DOUBLE_NOTES) player->m_modifiers &= ~DOUBLE_NOTES;
					else player->m_modifiers |= DOUBLE_NOTES;

					button->SetForceOn(player->m_modifiers & DOUBLE_NOTES);
					return BUTTON_FINISHED;
				});
				m_buttons.AddButton(button);
			}
		}
		// Drums
		if (player->m_instrument == INSTRUMENT_TYPE_DRUMS) {
			if (!player->m_drumsUseCymbals) { // Normal Drums
				WidgetButton *button = new WidgetButton("Note Shuffle");
				if (player->m_modifiers & NOTE_SHUFFLE) button->SetForceOn(true);

				button->SetOnClick([this, player, button]() {
					if (player->m_modifiers & NOTE_SHUFFLE) player->m_modifiers &= ~NOTE_SHUFFLE;
					else player->m_modifiers |= NOTE_SHUFFLE;

					button->SetForceOn(player->m_modifiers & NOTE_SHUFFLE);
					return BUTTON_FINISHED;
				});
				m_buttons.AddButton(button);

				button = new WidgetButton("Mirror Mode");
				if (player->m_modifiers & MIRROR_MODE) button->SetForceOn(true);

				button->SetOnClick([this, player, button]() {
					if (player->m_modifiers & MIRROR_MODE) player->m_modifiers &= ~MIRROR_MODE;
					else player->m_modifiers |= MIRROR_MODE;

					button->SetForceOn(player->m_modifiers & MIRROR_MODE);
					return BUTTON_FINISHED;
				});
				m_buttons.AddButton(button);

				button = new WidgetButton("Double Notes");
				if (player->m_modifiers & DOUBLE_NOTES) button->SetForceOn(true);

				button->SetOnClick([this, player, button]() {
					if (player->m_modifiers & DOUBLE_NOTES) player->m_modifiers &= ~DOUBLE_NOTES;
					else player->m_modifiers |= DOUBLE_NOTES;

					button->SetForceOn(player->m_modifiers & DOUBLE_NOTES);
					return BUTTON_FINISHED;
				});
				m_buttons.AddButton(button);
			}
			else { // Pro Drums
				WidgetButton *button = new WidgetButton("All Cymbals");
				if (player->m_modifiers & ALL_CYMBALS) button->SetForceOn(true);

				button->SetOnClick([this, player, button]() {
					if (player->m_modifiers & ALL_CYMBALS) player->m_modifiers &= ~ALL_CYMBALS;
					else player->m_modifiers |= ALL_CYMBALS;

					button->SetForceOn(player->m_modifiers & ALL_CYMBALS);
					return BUTTON_FINISHED;
				});
				m_buttons.AddButton(button);

				button = new WidgetButton("Cymbal Shuffle");
				if (player->m_modifiers & SHUFFLE_CYMBALS) button->SetForceOn(true);

				button->SetOnClick([this, player, button]() {
					if (player->m_modifiers & SHUFFLE_CYMBALS) player->m_modifiers &= ~SHUFFLE_CYMBALS;
					else player->m_modifiers |= SHUFFLE_CYMBALS;

					button->SetForceOn(player->m_modifiers & SHUFFLE_CYMBALS);
					return BUTTON_FINISHED;
				});
				m_buttons.AddButton(button);

				button = new WidgetButton("Note Shuffle");
				if (player->m_modifiers & NOTE_SHUFFLE) button->SetForceOn(true);

				button->SetOnClick([this, player, button]() {
					if (player->m_modifiers & NOTE_SHUFFLE) player->m_modifiers &= ~NOTE_SHUFFLE;
					else player->m_modifiers |= NOTE_SHUFFLE;

					button->SetForceOn(player->m_modifiers & NOTE_SHUFFLE);
					return BUTTON_FINISHED;
				});
				m_buttons.AddButton(button);

				button = new WidgetButton("Mirror Mode");
				if (player->m_modifiers & MIRROR_MODE) button->SetForceOn(true);

				button->SetOnClick([this, player, button]() {
					if (player->m_modifiers & MIRROR_MODE) player->m_modifiers &= ~MIRROR_MODE;
					else player->m_modifiers |= MIRROR_MODE;

					button->SetForceOn(player->m_modifiers & MIRROR_MODE);
					return BUTTON_FINISHED;
				});
				m_buttons.AddButton(button);

				button = new WidgetButton("Double Notes");
				if (player->m_modifiers & DOUBLE_NOTES) button->SetForceOn(true);

				button->SetOnClick([this, player, button]() {
					if (player->m_modifiers & DOUBLE_NOTES) player->m_modifiers &= ~DOUBLE_NOTES;
					else player->m_modifiers |= DOUBLE_NOTES;

					button->SetForceOn(player->m_modifiers & DOUBLE_NOTES);
					return BUTTON_FINISHED;
				});
				m_buttons.AddButton(button);
			}
		}
	}
}