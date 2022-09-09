#ifndef CANVAS_FACTORY_SECRET_UNLOCK_HPP_
#define CANVAS_FACTORY_SECRET_UNLOCK_HPP_

#include <vector>
#include <random>

#include "../canvas.hpp"
#include "../canvas_data.hpp"

#include "../widget.hpp"

#include "../widgets/image.hpp"
#include "../widgets/player_display.hpp"
#include "../widgets/text.hpp"
#include "../widgets/tile.hpp"

#include <wge/core/logging.hpp>

#include "../../rendering/render_manager.hpp"

#include "../menu_system.hpp"

#include "../../game/color.hpp"

#include "../../util/settings.hpp"
#include "../../util/thread_pool.hpp"
#include "../../util/misc.hpp"

#include "../../container/secret/secret_container.hpp"

#include "../../container/secret/secret.hpp"

#include "../../io/io_window.hpp"

namespace Rhythmic
{
	namespace FactoryCanvas
	{
		class WidgetUnlockSecret : public WidgetBase
		{
		public:
			WidgetUnlockSecret()
			{
				m_unlockText = new WidgetText("Unlock", RenderManager::GetFontRef("menus"), 1);
				m_unlockedSecret = false;

				m_padsPressed = 0;

				m_greenTile = new WidgetTile(0, 0, RenderManager::GetSheet("ui_elements"), g_colors[0]);
				m_redTile = new WidgetTile(0, 0, RenderManager::GetSheet("ui_elements"), g_colors[1]);
				m_yellowTile = new WidgetTile(0, 0, RenderManager::GetSheet("ui_elements"), g_colors[2]);
				m_blueTile = new WidgetTile(0, 0, RenderManager::GetSheet("ui_elements"), g_colors[3]);
				m_orangeTile = new WidgetTile(0, 0, RenderManager::GetSheet("ui_elements"), g_colors[4]);

				m_greenTile->SetPosition(glm::vec2(0.1f, 0.5f));
				m_greenTile->SetScale(glm::vec2(0.075f, 0.1f));

				m_redTile->SetPosition(glm::vec2(0.3f, 0.5f));
				m_redTile->SetScale(glm::vec2(0.075f, 0.1f));

				m_yellowTile->SetPosition(glm::vec2(0.5f, 0.5f));
				m_yellowTile->SetScale(glm::vec2(0.075f, 0.1f));

				m_blueTile->SetPosition(glm::vec2(0.7f, 0.5f));
				m_blueTile->SetScale(glm::vec2(0.075f, 0.1f));

				m_orangeTile->SetPosition(glm::vec2(0.9f, 0.5f));
				m_orangeTile->SetScale(glm::vec2(0.075f, 0.1f));

				m_unlockText->SetPosition(glm::vec2(0.5f, 0.9f));
				m_unlockText->SetScale(glm::vec2(0.035f, 0.035f));
			}
			virtual ~WidgetUnlockSecret()
			{
				delete m_greenTile;
				delete m_redTile;
				delete m_yellowTile;
				delete m_blueTile;
				delete m_orangeTile;
			}
			virtual void Update(float delta)
			{
				if (m_unlockedSecret && m_timeRemaining > 0)
				{
					m_timeRemaining -= delta;

					if (m_timeRemaining <= 0)
					{
						m_unlockedSecret = false;
						m_unlockText->SetText("");
					}
				}
			}
			virtual void Render(float interpolation)
			{
				if (m_padsPressed & (1 << INPUT_BUTTON_GREEN)) { m_greenTile->Render(interpolation); }
				if (m_padsPressed & (1 << INPUT_BUTTON_RED)) { m_redTile->Render(interpolation); }
				if (m_padsPressed & (1 << INPUT_BUTTON_YELLOW)) { m_yellowTile->Render(interpolation); }
				if (m_padsPressed & (1 << INPUT_BUTTON_BLUE)) { m_blueTile->Render(interpolation); }
				if (m_padsPressed & (1 << INPUT_BUTTON_ORANGE)) { m_orangeTile->Render(interpolation); }

				if (m_unlockedSecret) { m_unlockText->Render(interpolation); }
			}
			virtual void ProcessInput(InputEventData* input, int playerIndex)
			{
				if (playerIndex != 0)
					return;

				InputGameField buttonsPressed = input->m_buttonsPressed;
				InputGameField buttonsReleased = input->m_buttonsReleased;

				// Green
				if (buttonsPressed & ButtonToFlag(INPUT_BUTTON_GREEN))
				{
					m_padsPressed |= ButtonToFlag(INPUT_BUTTON_GREEN);
				}
				else if (buttonsReleased & ButtonToFlag(INPUT_BUTTON_GREEN))
				{
					m_padsPressed &= ~ButtonToFlag(INPUT_BUTTON_GREEN);
				}

				// Red
				if (buttonsPressed & ButtonToFlag(INPUT_BUTTON_RED))
				{
					m_padsPressed |= ButtonToFlag(INPUT_BUTTON_RED);
				}
				else if (buttonsReleased & ButtonToFlag(INPUT_BUTTON_RED))
				{
					m_padsPressed &= ~ButtonToFlag(INPUT_BUTTON_RED);
				}

				// Yellow
				if (buttonsPressed & ButtonToFlag(INPUT_BUTTON_YELLOW))
				{
					m_padsPressed |= ButtonToFlag(INPUT_BUTTON_YELLOW);
				}
				else if (buttonsReleased & ButtonToFlag(INPUT_BUTTON_YELLOW))
				{
					m_padsPressed &= ~ButtonToFlag(INPUT_BUTTON_YELLOW);
				}

				// Blue
				if (buttonsPressed & ButtonToFlag(INPUT_BUTTON_BLUE))
				{
					m_padsPressed |= ButtonToFlag(INPUT_BUTTON_BLUE);
				}
				else if (buttonsReleased & ButtonToFlag(INPUT_BUTTON_BLUE))
				{
					m_padsPressed &= ~ButtonToFlag(INPUT_BUTTON_BLUE);
				}

				// Orange
				if (buttonsPressed & ButtonToFlag(INPUT_BUTTON_ORANGE))
				{
					m_padsPressed |= ButtonToFlag(INPUT_BUTTON_ORANGE);
				}
				else if (buttonsReleased & ButtonToFlag(INPUT_BUTTON_ORANGE))
				{
					m_padsPressed &= ~ButtonToFlag(INPUT_BUTTON_ORANGE);
				}

				if (buttonsPressed & ButtonToFlag(INPUT_BUTTON_DOWN) || buttonsPressed & ButtonToFlag(INPUT_BUTTON_UP))
				{
					std::string output = "";
					if (m_padsPressed & (1 << INPUT_BUTTON_GREEN)) output += "Green. ";
					if (m_padsPressed & (1 << INPUT_BUTTON_RED)) output += "Red. ";
					if (m_padsPressed & (1 << INPUT_BUTTON_YELLOW)) output += "Yellow. ";
					if (m_padsPressed & (1 << INPUT_BUTTON_BLUE)) output += "Blue. ";
					if (m_padsPressed & (1 << INPUT_BUTTON_ORANGE)) output += "Orange. ";

					output += std::to_string(m_padsPressed);

					m_inputs.push_back(m_padsPressed);
					CheckForUnlock();
				}
			}

			bool find(std::vector<InputGameField> secretSequence)
			{
				if (secretSequence.size() > m_inputs.size()) return false;

				for (int i = 0; i < m_inputs.size() - secretSequence.size() + 1; i++)
				{
					bool match = true;
					for (int j = 0; j < secretSequence.size(); j++)
					{
						if (m_inputs[i + j] != secretSequence[j])
						{
							match = false;
							break;
						}
					}
					if (match)
					{
						return true;
					}
				}
				return false;
			}

			void CheckForUnlock()
			{
				std::vector<Secret*> *secrets = SecretContainer::GetSecrets();

				for (auto it = secrets->begin(); it != secrets->end(); it++) // Loop every secret
				{
					Secret *secret = *it;

					if (find(secret->m_buttonSequence))
					{
						if (!secret->m_isUnlocked)
						{
							LOG_INFO("Secret unlocked!");
							UnlockSecret(secret);
						}
						m_inputs.clear();
					}
				}
			}

			void UnlockSecret(Secret *secret)
			{
				m_timeRemaining = m_timeShowText;
				m_unlockedSecret = true;
				secret->Unlock();

				switch (secret->m_type)
				{
				case INFINITE_NOTESPEED:
					m_unlockText->SetText("Unlocked Infinite Notespeed");
					break;
				case NEGATIVE_NOTESPEED:
					m_unlockText->SetText("Unlocked Negative Notespeed");
					break;
				case INFINITE_ENGINE:
					m_unlockText->SetText("Unlocked Infinite Engine Values");
					break;
				case NEGATIVE_ENGINE:
					m_unlockText->SetText("Unlocked Negative Engine Values");
					break;
				default:
					m_unlockText->SetText("Unlocked Secret");
					break;
				}
			}
		private:
			InputGameField m_padsPressed;
			std::vector<InputGameField> m_inputs;

			WidgetText* m_unlockText;

			WidgetTile *m_greenTile;
			WidgetTile *m_redTile;
			WidgetTile *m_yellowTile;
			WidgetTile *m_blueTile;
			WidgetTile *m_orangeTile;

			const float m_timeShowText = 5;
			float m_timeRemaining;
			bool m_unlockedSecret;
		};

		void CanvasCreateSecretUnlock(Canvas* canvas, void* canvasData)
		{
			IO_Image background; // Image variable
			IO_LoadImage(Util::GetExecutablePath() + "/game_data/images/backgrounds/space_01.png", &background); // Load background image into background var reference
			WidgetImage* bg = new WidgetImage(background, Rendering::TEXTURE_FLAG_CLAMP | Rendering::TEXTURE_FLAG_FILTER_LINEAR, true);
			IO_FreeImageData(&background);

			canvas->Add("background", bg); // Adds background widget to canvas

			canvas->Add("unlock", new WidgetUnlockSecret());

			WidgetButton* buttonBack = new WidgetButton("<", []() { MenuSystem::GetInstance()->QueuePopMenu(); return BUTTON_FINISHED; }, 1);
			buttonBack->SetExtents(glm::vec2(0.015f, 0.05f), glm::vec2(0.05f, 0.14f));
			canvas->Add("button_back", buttonBack);
		}
	}
}

#endif