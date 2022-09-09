#ifndef CANVAS_FACTORY_OPTIONS_HPP_
#define CANVAS_FACTORY_OPTIONS_HPP_

#include <vector>

#include "../canvas.hpp"

#include "../../chart/song.hpp"

#include "../widgets/button_field.hpp"
#include "../widgets/image.hpp"
#include "../widgets/tile.hpp"
#include "../widgets/controller_back_menu.hpp"
#include "../widgets/player_display.hpp"

#include <wge/core/logging.hpp>

#include "../../rendering/render_manager.hpp"

#include "../menu_system.hpp"

#include "../../util/settings.hpp"
#include "../../util/thread_pool.hpp"
#include "../../util/misc.hpp"

#include "../../container/song_container.hpp"
#include "../../container/engine/engine_container.hpp"
#include "../../container/bg_image_container.hpp"
#include "../../container/engine/engine.hpp"

#include "../../io/io_window.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#ifdef WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#endif

#include "../../game/menu_music_player.hpp"

#define CATEGORY_PAGE 0

namespace Rhythmic
{
	namespace FactoryCanvas
	{
		struct Resolution
		{
			int width;
			int height;
		};

		class WidgetOptionsPage : public WidgetBase
		{
		public:
			WidgetOptionsPage(WidgetButtonField* buttonField) :
				WidgetBase(),
				m_buttonField(buttonField),
				m_page(-1)
			{

			}

			int GetPage() { return m_page; }

			void ChangePage(int page)
			{
				if (m_page == page)
					return;

				SettingsSave();

				m_buttonField->ClearButtons();

				if (page == 0) SetPageCategories(m_buttonField, &m_page);
				if (page == 1) SetPageGeneral(m_buttonField);
				if (page == 2) SetPageAudio(m_buttonField);
				if (page == 3) SetPageVideo(m_buttonField);
				if (page == 4) SetPageGameplay(m_buttonField);

				m_page = page;
			}

			virtual void Update(float delta)
			{
				if (m_page == -10) MenuSystem::GetInstance()->QueuePopMenu();

				if (m_page == -1)
					ChangePage(0); // Categories
				if (m_page == -2)
					ChangePage(1); // General
				if (m_page == -3)
					ChangePage(2); // Audio
				if (m_page == -4)
					ChangePage(3); // Video
				if (m_page == -5)
					ChangePage(4); // Gameplay
			}
			void Render(float interpolation) override
			{
			}

			void ProcessInput(InputEventData* input, int playerIndex) override
			{
				if (input->m_buttonsPressed & (1 << INPUT_BUTTON_RED))
				{
					if(m_page == CATEGORY_PAGE)
						MenuSystem::GetInstance()->QueuePopMenu();
					else
						ChangePage(CATEGORY_PAGE);

					return;
				}
			}

			void OnCanvasActivated() override
			{
				SettingValue value;

				value.type = SETTING_GAME_CALIBRATION_VIDEO;
				SettingsGet(&value);
				m_calVideo = value.m_iValue;

				value.type = SETTING_GAME_CALIBRATION_AUDIO;
				SettingsGet(&value);
				m_calAudio = value.m_iValue;
			}

			void SetPageCategories(WidgetButtonField* buttonField, int* page)
			{
				buttonField->AddButton(new WidgetButton("General", [page]()
					{
						*page = -2;
						return BUTTON_FINISHED;
					}, 0));
				buttonField->AddButton(new WidgetButton("Audio", [page]()
					{
						*page = -3;
						return BUTTON_FINISHED;
					}, 0));
				buttonField->AddButton(new WidgetButton("Video", [page]()
					{
						*page = -4;
						return BUTTON_FINISHED;
					}, 0));
				buttonField->AddButton(new WidgetButton("Gameplay", [page]()
					{
						*page = -5;
						return BUTTON_FINISHED;
					}, 0));
			}

			int m_calVideo;
			int m_calAudio;
			void SetPageGeneral(WidgetButtonField* buttonField)
			{
				SettingValue value;

				value.type = SETTING_GAME_CALIBRATION_VIDEO;
				SettingsGet(&value);
				m_calVideo = value.m_iValue;

				value.type = SETTING_GAME_CALIBRATION_AUDIO;
				SettingsGet(&value);
				m_calAudio = value.m_iValue;

				buttonField->AddButton(new WidgetButton("Recache Songs", []()
					{
						if (!SongContainer::IsScanning())
						{
							ThreadPool::QueueJob(SongContainer::CacheSongs);
						}
						return BUTTON_FINISHED;
					}, 0));

				buttonField->AddButton(new WidgetButton("Video Calibration: ", []() { return BUTTON_TOGGLE_SCROLL; }, 0, [this](int i)
					{
						m_calVideo += i;

						SettingValue value;
						value.type = SETTING_GAME_CALIBRATION_VIDEO;
						value.m_iValue = m_calVideo;
						SettingsSet(value);

						return BUTTON_FINISHED;
					}));
				buttonField->AddButton(new WidgetButton("Audio Calibration: ", []() { return BUTTON_TOGGLE_SCROLL; }, 0, [this](int i)
					{
						m_calAudio += i;

						SettingValue value;
						value.type = SETTING_GAME_CALIBRATION_AUDIO;
						value.m_iValue = m_calAudio;
						SettingsSet(value);

						return BUTTON_FINISHED;
					}));

				buttonField->AddButton(new WidgetButton("Calibrate...", []()
					{
						MenuSystem *system = MenuSystem::GetInstance();
						if (PlayerContainer::GetPlayerByIndex(0)->m_playerID == 5)
						{
							system->DisplayMessage("There must be a player assigned!");
							return BUTTON_FINISHED;
						}

						system->DisplayConfirmationBox([]()
							{
								MusicPlayerMenu::GetInstance()->Stop();

								SettingValue value;
								value.m_iValue = 0;

								value.type = SETTING_GAME_CALIBRATION_VIDEO;
								SettingsSet(value);

								value.type = SETTING_GAME_CALIBRATION_AUDIO;
								SettingsSet(value);

								MusicPlayerMenu::GetInstance()->Stop();

								MenuSystem::GetInstance()->QueuePushMenu(CanvasFactory::CreateCanvas(CANVAS_FACTORY_CALIBRATION));
							}, "This will reset your current calibration! Are you sure?");

						return BUTTON_FINISHED;
					}, 0, 0));

				buttonField->Get(1)->GetLabel().TagIntAtEnd(&m_calVideo);
				buttonField->Get(2)->GetLabel().TagIntAtEnd(&m_calAudio);
			}

			void SetPageAudio(WidgetButtonField* buttonField)
			{
				static int volumeMaster;
				static int volumeMenu;
				static int volumeEffects;

				SettingValue value;

				value.type = SETTING_AUDIO_MASTER;
				SettingsGet(&value);
				volumeMaster = value.m_iValue;

				value.type = SETTING_AUDIO_MENU_MUSIC_VOLUME;
				SettingsGet(&value);
				volumeMenu = value.m_iValue;

				value.type = SETTING_AUDIO_SFX;
				SettingsGet(&value);
				volumeEffects = value.m_iValue;

				buttonField->AddButton(new WidgetButton("Master Volume: ",
					[]()
					{
						return BUTTON_TOGGLE_SCROLL;
					}, 0,
					[](int volume)
					{
						volumeMaster += volume * 5;

						volumeMaster = glm::clamp(volumeMaster, 0, 100);

						SettingValue value;
						value.type = SETTING_AUDIO_MASTER;
						value.m_iValue = volumeMaster;
						SettingsSet(value);

						AudioSystem::GetInstance()->SetMasterVolume((float)volumeMaster / 100.0f);

						return BUTTON_FINISHED;
					}));

				buttonField->AddButton(new WidgetButton("Sound Effects: ",
					[]()
					{
						return BUTTON_TOGGLE_SCROLL;
					}, 0,
					[](int volume)
					{
						volumeEffects += volume * 5;

						volumeEffects = glm::clamp(volumeEffects, 0, 100);

						SettingValue value;
						value.type = SETTING_AUDIO_SFX;
						value.m_iValue = volumeEffects;
						SettingsSet(value);

						AudioSystem::GetInstance()->SetVolume(AUDIO_TYPE_SFX, (float)volumeEffects / 100.0f);

						return BUTTON_FINISHED;
					}));

				buttonField->AddButton(new WidgetButton("Menu Music: ",
					[]()
					{
						return BUTTON_TOGGLE_SCROLL;
					}, 0,
					[](int volume)
					{
						volumeMenu += volume * 5;

						volumeMenu = glm::clamp(volumeMenu, 0, 100);

						SettingValue value;
						value.type = SETTING_AUDIO_MENU_MUSIC_VOLUME;
						value.m_iValue = volumeMenu;
						SettingsSet(value);

						return BUTTON_FINISHED;
					}));

				// Guitar
				// Bass
				// Rhythm
				// Drums
				// Keys
				// Song
				// Vocals
				// Crowd
				// Sound Effects
				// Menu Music

				buttonField->Get(0)->GetLabel().TagIntAtEnd(&volumeMaster);
				buttonField->Get(1)->GetLabel().TagIntAtEnd(&volumeEffects);
				buttonField->Get(2)->GetLabel().TagIntAtEnd(&volumeMenu);

				// TODO OTHER VOLUME TYPES
			}

			void SetPageVideo(WidgetButtonField* buttonField)
			{
				static bool buttonEnabled = false;

				static bool fullscreen;
				static int framerate;

				static std::vector<Resolution> resolutions;
				static int resIndex = 0;

				static int settingsWidth;
				static int settingsHeight;

				SettingValue value;

				value.type = SETTING_WINDOW_FULLSCREEN;
				SettingsGet(&value);
				fullscreen = value.m_iValue != 0;

				value.type = SETTING_WINDOW_REFRESH_RATE;
				SettingsGet(&value);
				framerate = glm::clamp(value.m_iValue, 60, 1000);

				value.type = SETTING_WINDOW_WIDTH;
				SettingsGet(&value);
				settingsWidth = value.m_iValue;

				value.type = SETTING_WINDOW_HEIGHT;
				SettingsGet(&value);
				settingsHeight = value.m_iValue;

				buttonField->AddButton(new WidgetButton("Resolution: ", []()
					{
						buttonEnabled = !buttonEnabled;

						if (buttonEnabled && resolutions.empty())
						{
							int count;

							GLFWmonitor* monitor = glfwGetPrimaryMonitor();
							const GLFWvidmode* modes = glfwGetVideoModes(monitor, &count);

							int lastWidth = 0;
							int lastHeight = 0;

							for (auto it = 0; it < count; it++)
							{
								if (modes[it].width == lastWidth || modes[it].height == lastHeight || modes[it].refreshRate < 60) continue;

								lastWidth = modes[it].width;
								lastHeight = modes[it].height;

								resolutions.push_back({ lastWidth, lastHeight });
							}

							std::sort(resolutions.begin(), resolutions.end(), [](Resolution first, Resolution second)
								{
									return first.width * first.height < second.width * second.height;
								});

							for (Resolution res : resolutions)
							{
								if (res.width == IO_Window_GetWidth() && res.height == IO_Window_GetHeight())
									break;

								resIndex++;
							}
						}
						else
						{
							IO_Window_RefreshVars();
						}
						return BUTTON_TOGGLE_SCROLL;
					}, 0, [buttonField](int i)
					{
						resIndex += i;

						if (resIndex < 0) resIndex = 0;
						if (resIndex > resolutions.size() - 1) resIndex = resolutions.size() - 1;

						Resolution res = resolutions[resIndex];

						SettingValue value;
						value.type = SETTING_WINDOW_WIDTH;
						value.m_iValue = res.width;
						SettingsSet(value);

						value.type = SETTING_WINDOW_HEIGHT;
						value.m_iValue = res.height;
						SettingsSet(value);

						buttonField->Get(0)->GetLabel().SetText("Resolution: " + std::to_string(res.width) + "x" + std::to_string(res.height));

						return BUTTON_FINISHED;
					}));

				buttonField->AddButton(new WidgetButton("Fullscreen: ", [buttonField]()
					{
						fullscreen = !fullscreen;

						SettingValue value;
						value.type = SETTING_WINDOW_FULLSCREEN;
						value.m_iValue = fullscreen ? 1 : 0;
						SettingsSet(value);

						buttonField->Get(0)->SetDisabled(fullscreen);
						buttonField->Get(1)->GetLabel().SetText(fullscreen ? "Fullscreen: Yes" : "Fullscreen: No");

						IO_Window_RefreshVars();

						return BUTTON_FINISHED;
					}, 0));

				/*buttonField->AddButton(new WidgetButton("Framerate: ", []() 
					{
						buttonEnabled = !buttonEnabled;

						if(!buttonEnabled)
							IO_Window_RefreshVars();

						return BUTTON_TOGGLE_SCROLL; 
					}, 0, [](int i)
					{
						framerate += i * 10;
						framerate = glm::clamp(framerate, 60, 1000);

						SettingValue value;
						value.type = SETTING_WINDOW_REFRESH_RATE;
						value.m_iValue = framerate;
						SettingsSet(value);

						return BUTTON_FINISHED;
					}));*/

				buttonField->Get(0)->GetLabel().SetText("Resolution: " + std::to_string(settingsWidth) + "x" +
					std::to_string(settingsHeight));

				buttonField->Get(1)->GetLabel().SetText(fullscreen ? "Fullscreen: Yes" : "Fullscreen: No");

				//buttonField->Get(0)->GetLabel().TagIntAtEnd(&video);
				//buttonField->Get(1)->GetLabel().TagBoolAtEnd((int*)&fullscreen); // Doesn't work and I have no fucking clue why it's the exact same as every other one.
				//buttonField->Get(2)->GetLabel().TagIntAtEnd(&framerate);

#ifdef WGE_DEBUG
				buttonField->Get(0)->SetDisabled(true);
				buttonField->Get(1)->SetDisabled(true);
#endif
			}

			void SetPageGameplay(WidgetButtonField* buttonField)
			{
				static int menuMusic;
				static int missedNotes;
				static int bgImage = 0; // By default

				static int songSpeed = GamePlayVariables::g_chartModifiers.m_songSpeed;

				SettingValue value;

				value.type = SETTING_GAME_MENU_MUSIC;
				SettingsGet(&value);
				menuMusic = value.m_iValue;

				value.type = SETTING_GAME_SHOW_MISSED_NOTES;
				SettingsGet(&value);
				missedNotes = value.m_iValue;

				buttonField->AddButton(new WidgetButton("Song Speed: ", []()
					{
						return BUTTON_TOGGLE_SCROLL;
					}, 0, [](int i)
					{
						GamePlayVariables::g_chartModifiers.m_songSpeed += i * 5;
						GamePlayVariables::g_chartModifiers.m_songSpeed = glm::clamp(GamePlayVariables::g_chartModifiers.m_songSpeed, 5, 5000);

						songSpeed = GamePlayVariables::g_chartModifiers.m_songSpeed;

						return BUTTON_FINISHED;
					}));

				buttonField->AddButton(new WidgetButton("Show Missed Notes: ", []()
					{
						missedNotes = !missedNotes;

						SettingValue value;
						value.type = SETTING_GAME_SHOW_MISSED_NOTES;
						value.m_iValue = missedNotes ? 1 : 0;
						SettingsSet(value);

						return BUTTON_FINISHED;
					}, 0));

				buttonField->AddButton(new WidgetButton("Menu Music: ", []()
					{
						menuMusic = !menuMusic;

						SettingValue value;
						value.type = SETTING_GAME_MENU_MUSIC;
						value.m_iValue = menuMusic ? 1 : 0;
						SettingsSet(value);

						if (menuMusic)
							MusicPlayerMenu::GetInstance()->Start();
						else
							MusicPlayerMenu::GetInstance()->Stop();

						return BUTTON_FINISHED;
					}, 0));

				buttonField->AddButton(new WidgetButton("Background: ", []()
					{
						return BUTTON_TOGGLE_SCROLL;
					}, 0, [buttonField](int scroll)
					{
						if (scroll > 0)
							bgImage = BGContainer::GetNextValidHandle(bgImage);
						if (scroll < 0)
							bgImage = BGContainer::GetPrevValidHandle(bgImage);

						Global_SetBGImage(bgImage);

						buttonField->Get(3)->GetLabel().TagStringAtEnd(&BGContainer::GetBackground(bgImage)->m_name);

						return BUTTON_FINISHED;
					}));

				buttonField->Get(0)->GetLabel().TagIntAtEnd(&songSpeed);
				buttonField->Get(1)->GetLabel().TagBoolAtEnd(&missedNotes);
				buttonField->Get(2)->GetLabel().TagBoolAtEnd(&menuMusic);
				buttonField->Get(3)->GetLabel().TagStringAtEnd(&BGContainer::GetBackground(bgImage)->m_name);
			}
		private:
			WidgetButtonField *m_buttonField;

			int m_page;
		};

		void CanvasCreateOptions(Canvas *canvas, void *canvasData)
		{
			IO_Image background; // Image variable
			IO_LoadImage(Util::GetExecutablePath() + "/game_data/images/backgrounds/space_01.png", &background); // Load background image into background var reference
			WidgetImage *bg = new WidgetImage(background, Rendering::TEXTURE_FLAG_CLAMP | Rendering::TEXTURE_FLAG_FILTER_LINEAR, true);
			IO_FreeImageData(&background);

			canvas->Add("background", bg); // Adds background widget to canvas

			WidgetTile* frame = new WidgetTile(0, 0, RenderManager::GetSheet("ui_elements"), glm::vec4(1, 1, 1, 0.7f));
			frame->SetScale(glm::vec2(0.2f, 0.35f));
			frame->SetPosition(glm::vec2(0.25f, 0.5f));
			canvas->Add("frame", frame);

			WidgetButtonField* buttonField = new WidgetButtonField(glm::vec4(0, 0, 0, 0.6f));
			buttonField->SetPosition(frame->GetPosition() - glm::vec2(0, frame->GetScale().y - 0.04f));
			buttonField->SetScale((frame->GetScale() * glm::vec2(1, 1.88f)) - glm::vec2(0, 0.04f));//frame->GetScale() - glm::vec2(0,-0.04f));
			canvas->Add("buttonField", buttonField);

			WidgetOptionsPage* options = new WidgetOptionsPage(buttonField);
			canvas->Add("options", options);

			canvas->Add("player_display", new WidgetPlayerDisplay());

			WidgetButton *buttonBack = new WidgetButton("<", [options]() { SettingsSave(); options->ChangePage(options->GetPage() == 0 ? -10 : 0); return BUTTON_FINISHED; }, 1);
			buttonBack->SetExtents(glm::vec2(0.015f, 0.05f), glm::vec2(0.05f, 0.14f));
			canvas->Add("button_back", buttonBack);
		}
	}
}

#endif