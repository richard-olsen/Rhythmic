#ifndef CANVAS_FACTORY_MAIN_HPP_
#define CANVAS_FACTORY_MAIN_HPP_

#include "../../engine/include/wge/core/logging.hpp"

#include "../../util/settings.hpp"

#include "../canvas.hpp"

#include "../widgets/image.hpp"
#include "../widgets/button.hpp"
#include "../widgets/text.hpp"
#include "../widgets/tile.hpp"
#include "../widgets/button_field.hpp"
#include "../widgets/player_display.hpp"

#include "../../rendering/render_manager.hpp"

#include "../../util/misc.hpp"

#include "../canvas_factory.hpp"
#include "../menu_system.hpp"

#include "../../container/song_container.hpp"

#include "../../game/globals.hpp"

#include "../../game/stage/stage.hpp"

#include "../../events/event_system.hpp"
#include "../../events/events_common.hpp"

#include "../../util/thread_pool.hpp"
#include "../../game/version_control.hpp"
#include "../../io/io_window.hpp"

#include <random>

#ifdef WIN32
#define NOMINMAX
#include <windows.h>
#endif

#include "../../util/version.hpp"

#include "../widgets/text_box.hpp"

#include "../../game/menu_music_player.hpp"
#include "../widgets/menu_music.hpp"

namespace Rhythmic
{
	namespace FactoryCanvas
	{
		class DoubleClickWidget : public WidgetBase
		{
		public:
			DoubleClickWidget() : m_time(0)
			{
			}
			~DoubleClickWidget()
			{
			}


			virtual void Update(float delta) {}
			virtual void Render(float interpolation) {}

			virtual void OnMouseClick(int button, float x, float y)
			{
				if (button != 0)
					return;

				float time = Util::GetTime();

				if (time - m_time < 0.2f)
				{
					MenuSystem::GetInstance()->QueuePushMenu(CanvasFactory::CreateCanvas(CANVAS_FACTORY_BINDINGS));
				}

				m_time = time;
			}
		private:
			EventListener m_listener;

			float m_time;
		};

		class WidgetPlayButtonDisabler : public WidgetBase
		{
		public:
			WidgetPlayButtonDisabler(WidgetButton *button1, WidgetButton *button2) :
				m_button1(button1),
				m_button2(button2)
			{ }
			virtual ~WidgetPlayButtonDisabler()
			{ }

			virtual void Update(float delta)
			{
				if (SongContainer::IsScanning())
				{
					m_button1->SetDisabled(true);
					m_button2->SetDisabled(true);
				}
				else
				{
					m_button1->SetDisabled(false);
					m_button2->SetDisabled(false);
				}
			}
			virtual void Render(float interpolation) { }
		private:
			WidgetButton *m_button1;
			WidgetButton *m_button2;
		};

		class WidgetMenuMusicPlayer : public WidgetBase
		{
		public:
			WidgetMenuMusicPlayer() :
				m_songText("", RenderManager::GetFontRef("menus")),
				m_artistText("", RenderManager::GetFontRef("menus"))
			{
				m_songText.SetPosition(glm::vec2(0.01f, 0.03f));
				m_songText.SetScale(glm::vec2(0.015f));

				m_artistText.SetPosition(glm::vec2(0.01f, 0.08f));
				m_artistText.SetScale(glm::vec2(0.015f));
			}
			virtual ~WidgetMenuMusicPlayer() { }

			void Update(float delta) override
			{
				const std::string &name = MusicPlayerMenu::GetInstance()->GetSongName();
				const std::string &artist = MusicPlayerMenu::GetInstance()->GetSongArtist();

				if (m_songText.GetText() != name)
					m_songText.SetText(name);
				if (m_artistText.GetText() != name)
					m_artistText.SetText(artist);
			}
			void Render(float interpolation) override
			{
				m_songText.Render(interpolation);
				m_artistText.Render(interpolation);
			}
		private:
			WidgetText m_songText;
			WidgetText m_artistText;
		};

		class WidgetCheckForUpdates : public WidgetBase
		{
		public:
			WidgetCheckForUpdates() :
				m_text("", RenderManager::GetFontRef("menus"), 2)
			{
				m_text.SetPosition(glm::vec2(0.99f, 0.1f));
				m_text.SetScale(glm::vec2(0.015f));
			}

			~WidgetCheckForUpdates()
			{
			}

			virtual void Update(float delta) {}

			virtual void Render(float interpolation)
			{
				if(m_isOutOfDate)
					m_text.Render(interpolation);
			}

			void OnCanvasActivated() override
			{
				if (!VersionControl::GetInstance()->IsLatestVersion())
				{
#ifdef RHYTHMIC_TESTER_BUILD
					MenuSystem::GetInstance()->DisplayMessage("This Tester Build is out of date. Please upgrade to the latest version!");
#endif
					m_text.SetText("New Update Available!");
					m_isOutOfDate = true;
				}
			}
		private:
			bool m_isOutOfDate = false;
			WidgetText m_text;
		};

		void CanvasCreateMain(Canvas *canvas, void *canvasData)
		{
			IO_Image background; // Image variable
			IO_LoadImage(Util::GetExecutablePath() + "/game_data/images/backgrounds/space_01.png", &background); // Load background image into background var reference
			WidgetImage *bg = new WidgetImage(background, Rendering::TEXTURE_FLAG_CLAMP | Rendering::TEXTURE_FLAG_FILTER_LINEAR, true);
			IO_FreeImageData(&background);

			IO_LoadImage(Util::GetExecutablePath() + "/game_data/images/rhythmic.png", &background); // Load logo image into background var reference
			WidgetImage *logo = new WidgetImage(background, Rendering::TEXTURE_FLAG_CLAMP | Rendering::TEXTURE_FLAG_FILTER_LINEAR);
			logo->SetPosition(glm::vec2(0.5f, 0.3f)); // Set position to center-ish
			logo->SetScale(glm::vec2(0.15f));
			IO_FreeImageData(&background);

			canvas->Add("background", bg); // Adds background widget to canvas

			WidgetTile *frame = new WidgetTile(0, 0, RenderManager::GetSheet("ui_elements"), glm::vec4(1,1,1,0.7f));
			frame->SetScale(glm::vec2(0.2f, 0.35f));
			frame->SetPosition(glm::vec2(0.5f));
			canvas->Add("frame", frame);

			canvas->Add("logo", logo);

			//WidgetMenuMusicPlayer* menuMusic = new WidgetMenuMusicPlayer(canvas);
			canvas->Add("ff", new WidgetMenuMusic());
			canvas->Add("fff", new WidgetMenuMusicPlayer());

			//canvas->Add("player", menuMusic);

			WidgetButtonField *buttonField = new WidgetButtonField();
			buttonField->SetScale(glm::vec2(0.15f, 0.07f * 4));
			buttonField->SetPosition(glm::vec2(0.5f));

			buttonField->AddButton(new WidgetButton("Play", []() {
					if (!SongContainer::IsScanning())
					{
						MenuSystem::GetInstance()->QueuePushMenu(CanvasFactory::CreateCanvas(CANVAS_FACTORY_SONG_SELECTION));
						StageSystem::GetInstance()->GetModulePractice()->SetPracticeMode(false);
						MusicPlayerMenu::GetInstance()->Stop();
					}
					return BUTTON_FINISHED;
				}, 1, 0, false)); // The true/false part here determines whether or not a confirmation is required

			buttonField->AddButton(new WidgetButton("Practice", []() {
					if (!SongContainer::IsScanning())
					{
						MenuSystem::GetInstance()->QueuePushMenu(CanvasFactory::CreateCanvas(CANVAS_FACTORY_SONG_SELECTION));
						StageSystem::GetInstance()->GetModulePractice()->SetPracticeMode(true);
						MusicPlayerMenu::GetInstance()->Stop();
					}
				return BUTTON_FINISHED;
				}));
			buttonField->AddButton(new WidgetButton("Options", []() {
					MenuSystem::GetInstance()->QueuePushMenu(CanvasFactory::CreateCanvas(CANVAS_FACTORY_OPTIONS));
					return BUTTON_FINISHED;
				}));
			buttonField->AddButton(new WidgetButton("Extras", []() {
					MenuSystem::GetInstance()->QueuePushMenu(CanvasFactory::CreateCanvas(CANVAS_FACTORY_EXTRAS));
					return BUTTON_FINISHED;
				}));

			buttonField->AddButton(new WidgetButton("Exit", []() { 
					MenuSystem::GetInstance()->DisplayConfirmationBox([]() { Util::RequestGameToClose(); }, 
						"Are you sure you want to quit?"); 
					return BUTTON_FINISHED;
				}));

			buttonField->SetAllowScroll(false);

			canvas->Add("button_field", buttonField);
			canvas->Add("disabler", new WidgetPlayButtonDisabler(buttonField->Get(0), buttonField->Get(1)));

			canvas->Add("player_display", new WidgetPlayerDisplay());

			WidgetText *info = new WidgetText("Double click to edit bindings", RenderManager::GetFontRef("menus"));
			info->SetPosition(glm::vec2(0.01f, 0.9f));
			info->SetScale(glm::vec2(0.015f));
			canvas->Add("tip", info);

			canvas->Add("dc", new DoubleClickWidget());

			canvas->Add("update", new WidgetCheckForUpdates());
		}
	}
}

#endif

