#include "player_stats.hpp"

#include "../../rendering/glad/glad.h"
#include "../../rendering/render_manager.hpp"

#include "../../io/io_window.hpp"

#include "../../container/player_container.hpp"
#include "../../container/engine/engine_container.hpp"

#include <sstream>

#include "../../game/stage/stage.hpp"

namespace Rhythmic
{
	const glm::vec2 text_scale = glm::vec2(0.015f);
	const glm::vec2 text_scale_Y = glm::vec2(0, text_scale.y * 2.0f);
	const glm::vec4 color = glm::vec4(0.2f, 0.2f, 0.2f, 0.9f);


	WidgetPlayerStats::WidgetPlayerStats(int player) :
		m_frame(0,0,RenderManager::GetSheet("ui_elements")),
		m_drawStats(false),
		m_scrollOffset(),
		m_focusPlayer(player),
		m_scroll(0),
		m_maxContentYSize(0)
	{
		ContainerPlayer *playerC = PlayerContainer::GetPlayerByIndex(player);
		if (playerC->m_playerID == 5)
			return;

		m_drawStats = true;
		
		m_frame.SetPosition(glm::vec2((0.25f * player) + 0.125f, 0.5f));
		m_frame.SetScale(glm::vec2(0.12f, 0.3f));

		SetCanvasChangeCallback([this](Canvas *canvas) { m_frame.SetCanvas(canvas); for (int i = 0; i < m_texts.size(); i++) m_texts[i].SetCanvas(canvas); });

		Instrument *instrument = playerC->m_player->GetInstrument();
		Chart *chart = StageSystem::GetInstance()->GetChart();

		std::string identifier = "";
		switch (playerC->m_difficulty)
		{
		case NOTES_EXPERT:
			identifier.append("Expert ");
			break;
		case NOTES_HARD:
			identifier.append("Hard ");
			break;
		case NOTES_MEDIUM:
			identifier.append("Medium ");
			break;
		case NOTES_EASY:
			identifier.append("Easy ");
		};
		switch (playerC->m_instrument)
		{
		case INSTRUMENT_TYPE_GUITAR:
			identifier.append("Guitar");
			break;
		case INSTRUMENT_TYPE_BASS:
			identifier.append("Bass");
			break;
		case INSTRUMENT_TYPE_DRUMS:
			if (playerC->m_drumsUseCymbals)
				identifier.append("Pro ");
			identifier.append("Drums");
			break;
		}
		glm::vec2 position = m_frame.GetPosition() - glm::vec2(0, m_frame.GetScale().y);

		m_texts.push_back(WidgetText(instrument->IsBotControlling() ? "Bot Enabled!" : 
			(instrument->IsFC() ? "Full Combo!" : instrument->GetNotesHit() >= (instrument->GetAmountOfNotesInChart() - 1) ? "Nice Choke!" : ""), 
			RenderManager::GetFontRef("menus"), 1, 
			instrument->IsBotControlling() ? 
				glm::vec4(0.5f, 0.8f, 0.8f, 1.0f) : 
				glm::vec4(1.0f, 0.8f, 0.0f, 1.0f))
		);
		m_texts[0].SetPosition(position + text_scale_Y);
		m_texts[0].SetScale(text_scale);
		
		// Difficulty & Instrument
		m_texts.push_back(WidgetText(identifier, RenderManager::GetFontRef("menus"), 1, color));
		m_texts[1].SetPosition(position + (text_scale_Y * 2.0f));
		m_texts[1].SetScale(text_scale);

		std::string engineText = "Engine: " + EngineContainer::GetCurrentEngine()->m_name;

		if (EngineContainer::GetCurrentEngine()->m_isCustom) engineText.append(" (Custom)");

		m_texts.push_back(WidgetText(engineText, RenderManager::GetFontRef("menus"), 1, color));
		m_texts[2].SetPosition(position + (text_scale_Y * 3.0f));
		m_texts[2].SetScale(text_scale);

		m_texts.push_back(WidgetText("Score: " + std::to_string(instrument->GetScore()), RenderManager::GetFontRef("menus"), 1, color));
		m_texts[3].SetPosition(position + (text_scale_Y * 4.0f));
		m_texts[3].SetScale(text_scale);

		m_texts.push_back(WidgetText(std::to_string(instrument->GetNotesHit()) + "/" + std::to_string(instrument->GetAmountOfNotesInChart()), RenderManager::GetFontRef("menus"), 1, color));
		m_texts[4].SetPosition(position + (text_scale_Y * 5.0f));
		m_texts[4].SetScale(text_scale);
		
		m_texts.push_back(WidgetText(std::to_string((int)std::floor(((float)instrument->GetNotesHit() / (float)instrument->GetAmountOfNotesInChart()) * 100)) + "%", RenderManager::GetFontRef("menus"), 1, color));
		m_texts[5].SetPosition(position + (text_scale_Y * 6.0f));
		m_texts[5].SetScale(text_scale);

		m_texts.push_back(WidgetText("Highest Streak: " + std::to_string(instrument->GetHighestStreak()), RenderManager::GetFontRef("menus"), 1, color));
		m_texts[6].SetPosition(position + (text_scale_Y * 7.0f));
		m_texts[6].SetScale(text_scale);

		std::stringstream multiplierStringStream = std::stringstream();
		multiplierStringStream.write("Average Multiplier: x", 22);
		multiplierStringStream.precision(2);
		multiplierStringStream << std::fixed << ((float) instrument->GetScore() / (float) instrument->GetBaseScore());
		m_texts.push_back(WidgetText(multiplierStringStream.str(), RenderManager::GetFontRef("menus"), 1, color));
		m_texts[7].SetPosition(position + (text_scale_Y * 8.0f));
		m_texts[7].SetScale(text_scale);

		float max = text_scale_Y.y * 8.0f;
		m_maxContentYSize = (text_scale_Y.y * 0.5f) + max;

		std::vector<Section> *sections = instrument->GetSections();

		for (int i = 0; i < chart->listSection.size(); i++)
		{
			int percentage = (int)std::floor(((float)(*sections)[i].notesHitInSection / (float)(*sections)[i].notesInSection) * 100);

			m_texts.push_back(WidgetText(chart->listSection[i].name + ": " + ((*sections)[i].notesInSection == 0 ? "N/A" : std::to_string(percentage) + "%"), 
				RenderManager::GetFontRef("menus"), 1, color));
			m_texts[8 + i].SetPosition(position + (text_scale_Y * (9.0f + (float)i)));
			m_texts[8 + i].SetScale(text_scale);

			m_maxContentYSize += (text_scale_Y.y);
		}
	}
	WidgetPlayerStats::~WidgetPlayerStats()
	{ }

	void WidgetPlayerStats::SetScrollOffset(glm::vec2 scrollOffset)
	{
		if (scrollOffset.y < 0)
			scrollOffset.y = 0;
		if (m_scrollOffset != scrollOffset)
		{
			m_scrollOffset = glm::clamp(scrollOffset, glm::vec2(0), glm::vec2(0, glm::max(0.0f, m_maxContentYSize - (m_frame.GetScale().y * 2))));

			glm::vec2 position = m_frame.GetPosition() - glm::vec2(0, m_frame.GetScale().y);

			for (int i = 0; i < m_texts.size(); i++)
			{
				m_texts[i].SetPosition(position + (text_scale_Y * (float)(i + 1)) - m_scrollOffset);
			}
		}
	}

	void WidgetPlayerStats::Update(float delta)
	{
		ContainerPlayer *player = PlayerContainer::GetPlayerByIndex(m_focusPlayer);

		if (player->m_playerID != 5)
		{
			if (m_scroll & (1 << 0))
				SetScrollOffset(m_scrollOffset + glm::vec2(0, delta * 0.7f));
			if (m_scroll & (1 << 1))
				SetScrollOffset(m_scrollOffset + glm::vec2(0, -delta * 0.7f));

			//if (player->m_inputDevice.hitDataButton[Input::BUTTON_DOWN].held)
			//if (player->m_inputDevice.hitDataButton[Input::BUTTON_UP].held)
		}
	}
	void WidgetPlayerStats::Render(float interpolation)
	{
		if (!m_drawStats)
			return;

		glEnable(GL_SCISSOR_TEST);

		float width = (float)IO_Window_GetWidth();
		float height = (float)IO_Window_GetHeight();

		glm::vec2 pos = m_frame.GetPosition();
		glm::vec2 sca = m_frame.GetScale();

		glScissor(
			(GLsizei)((pos.x - sca.x) * width), (GLsizei)(height - ((pos.y + sca.y) * height)),
			(GLsizei)((sca.x * 2) * width), (GLsizei)((sca.y * 2) * height)
		);

		m_frame.Render(interpolation);

		for (int i = 0; i < m_texts.size(); i++)
			m_texts[i].Render(interpolation);

		glDisable(GL_SCISSOR_TEST);
	}
	void WidgetPlayerStats::ProcessInput(InputEventData *input, int playerIndex)
	{
		if (playerIndex != m_focusPlayer)
			return;

		// Pressed buttons
		if (input->m_buttonsPressed & (1 << INPUT_BUTTON_DOWN))
			m_scroll |= (1 << 0);
		if (input->m_buttonsPressed & (1 << INPUT_BUTTON_UP))
			m_scroll |= (1 << 1);

		// Released buttons
		if (input->m_buttonsReleased & (1 << INPUT_BUTTON_UP))
			m_scroll &= ~(1 << 1);
		if (input->m_buttonsReleased & (1 << INPUT_BUTTON_DOWN))
			m_scroll &= ~(1 << 0);
	}
}

