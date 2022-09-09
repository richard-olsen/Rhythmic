#ifndef CANVAS_FACTORY_SONG_SELECT_HPP_
#define CANVAS_FACTORY_SONG_SELECT_HPP_

#include "../canvas.hpp"

#include "../widgets/image.hpp"
#include "../widgets/button.hpp"
#include "../widgets/text.hpp"
#include "../widgets/tile.hpp"
#include "../widgets/controller_back_menu.hpp"

#include "../widget_util.hpp"

#include "../../rendering/render_manager.hpp"

#include "../../util/misc.hpp"

#include "../../container/song_container.hpp"
#include "../../container/player_container.hpp"

#include "../../io/io_window.hpp"

#include "../menu_system.hpp"

#include "../../game/stage/stage.hpp"
#include "../../game/globals.hpp"
#include "../../util/settings.hpp"
#include "../../util/timer.hpp"
#include "../../util/ini_parser.hpp"
#include "../../util/misc.hpp"

#include "../../io/io_image.hpp"

#include "../../game/music_player.hpp"

#include <algorithm>
#include <filesystem>

#include <wge/io/filesystem.hpp>
#include <wge/core/time.hpp>

/*
This controls how long of a delay before streaming music, and loading
other details of the song from the song.ini and album image
*/
#define SELECTION_DELAY_TIMER 0.5

/*
The speed at which the volume changes
*/
#define VOLUME_CHANGE_SCALE 1.0f

namespace Rhythmic
{
	class WidgetSearch : public WidgetBase
	{
	public:
		WidgetSearch(WidgetButtonField *songButtons) :
			m_searchBox("", 1, false),
			m_allowSearching(false)
		{
			m_searchBox.SetScale(glm::vec2(0.2f, 0.0275f));
			m_searchBox.SetPosition(glm::vec2(0.5f, 0.1f));

			m_searchBox.SetOnTextChanged([songButtons](const std::string &str)
					{
						auto songs = SongContainer::GetSongs();
						auto song = std::find_if(songs.begin(), songs.end(), [str](const Rhythmic::ContainerSong &song)
							{
								if (song.name.size() < str.size())
									return false;

								std::string comp = std::string(song.name.c_str(), str.size());
								return Util::compareStringCaseInsensitive(comp, str) == 0;
							});
						if (song == songs.end())
							return;

						int selection = std::distance(songs.begin(), song);
						songButtons->Select(selection);
					});

			m_searchBox.SetOnEnter([this](std::string &text)
				{
					text.clear();
					m_allowSearching = false;
				});
		}
		virtual ~WidgetSearch() {}

		void Update(float delta) override
		{
			if (m_allowSearching)
			{
				m_searchBox.Select();
			}
			else
			{
				m_searchBox.Deselect();
			}
		}
		void Render(float interpolate) override
		{
			if (m_allowSearching)
				m_searchBox.Render(interpolate);
		}
		void ProcessInput(InputEventData *input, int playerIndex) override
		{
			if (playerIndex != 0)
				return;

			if (input->m_buttonsPressed & ButtonToFlag(INPUT_BUTTON_BLUE))
			{
				m_allowSearching = !m_allowSearching;
				if (!m_allowSearching)
					m_searchBox.SetText("");
			}
		}

		void OnCanvasDeactivated() override
		{
			m_searchBox.Deselect();
			m_allowSearching = false;
		}
	private:
		WidgetTextBox m_searchBox;
		bool m_allowSearching;
	};

	enum WMP_State
	{
		WMP_STOPPING,
		WMP_STARTING,
		WMP_PLAYING,
		WMP_STOPPED
	};

	class WidgetMusicPlayer : public WidgetBase
	{
	public:
		WidgetMusicPlayer(WidgetButtonField *songButtons) :
			m_volume(0),
			m_state(WMP_STOPPED)
		{ 
			SettingValue value;
			value.type = SETTING_AUDIO_MENU_MUSIC_VOLUME;
			SettingsGet(&value);

			m_volume = value.m_iValue;

			m_music.SetLocalVolume((float)m_volume / 100.0f);
		}
		virtual ~WidgetMusicPlayer()
		{
			m_music.Destroy();
		}

		void Update(float delta) override
		{
			switch (m_state)
			{
			case WMP_STOPPING:
				m_volume -= VOLUME_CHANGE_SCALE * WGE::Core::TimeStep.m_timeStep;
				if (m_volume <= 0.0f)
				{
					m_volume = 0.0f;
					m_state = WMP_STOPPED;
					ForceStop();
				}
				else
					m_music.SetLocalVolume(m_volume);
				break;
			case WMP_STOPPED:
				break;
			case WMP_PLAYING:
				m_timeLeft -= WGE::Core::TimeStep.m_timeStep;
				if (m_timeLeft <= 0.0)
					Stop();
				break;
			case WMP_STARTING:
				m_volume += VOLUME_CHANGE_SCALE * WGE::Core::TimeStep.m_timeStep;
				if (m_volume >= 1.0f)
				{
					m_volume = 1.0f;
					m_state = WMP_PLAYING;
				}
				m_music.SetLocalVolume(m_volume);
				break;
			}
		}
		void Play(const std::string &name)
		{
			m_music.Destroy();
			m_music.Create(AudioSystem::GetInstance(), name);

			m_music.SetDuration(m_music.GetSyncLength() * 0.3f);

			m_volume = 0;
			m_state = WMP_STARTING;
			m_timeLeft = 15.0f;

			m_music.SetLocalVolume(m_volume);
			m_music.Play();
		}
		void Stop()
		{
			m_state = WMP_STOPPING;
		}
		void ForceStop()
		{
			m_music.Pause();
			m_music.Destroy();

			m_state = WMP_STOPPED;
		}

		WMP_State GetState() { return m_state; }
	private:
		WMP_State m_state;

		float m_volume;
		float m_timeLeft;

		MusicPlayer m_music;
	};

	std::string GetFromIndex(int index)
	{
		SettingValue value;
		value.type = SETTING_GAME_SONGLIST_DIRECTORY;
		SettingsGet(&value);

		if (value.m_sValue[value.m_sValue.size() - 1] != '\\' || value.m_sValue[value.m_sValue.size() - 1] != '/')
			value.m_sValue.append("/");

		ContainerSong& song = SongContainer::GetSong(index);

		return value.m_sValue + song.songFolder + "/";
	}

	static unsigned char g_imageEmpty[4] = { 0, 0, 0, 0 };
	static IO_Image g_image;

	static void InitImage()
	{
		g_image.width = 1;
		g_image.height = 1;
		g_image.component = 4;
		g_image.data = &g_imageEmpty[0];
		g_image.fileName = "";
	}

	class WidgetSongDisplays : public WidgetBase
	{
	public:
		WidgetSongDisplays(WidgetButtonField *buttonField, WidgetTile *albumFrame) :
			m_timer(SELECTION_DELAY_TIMER, [this]()
			{
				if (SongContainer::GetSongs().empty())
				{
					MenuSystem::GetInstance()->DisplayMessage("No songs installed!");
					return;
				}

				m_timerFinished = true;

				std::string location = GetFromIndex(m_lastSelected);
				m_music.Play(location);

				IniFile songIni;
				IniSuccess iniStatus = IniRead(songIni, location + "song.ini");
				if (iniStatus.success == 0)
				{
					m_songName.SetText(songIni.m_sections["song"]["name"]);
					m_songArtist.SetText(songIni.m_sections["song"]["artist"]);
					m_songCharter.SetText(songIni.m_sections["song"]["charter"]);
				}

				IO_Image album;

				if (std::filesystem::is_regular_file(location + "album.png"))
					location += "album.png";
				else if (std::filesystem::is_regular_file(location + "album.jpg"))
					location += "album.jpg";
				else if (std::filesystem::is_regular_file(location + "album.jpeg"))
					location += "album.jpeg";
				else
					return;

				IO_LoadImage(location, &album);
				if (album.data)
				{
					m_albumImage.SetImage(album, Rendering::TEXTURE_FLAG_CLAMP | Rendering::TEXTURE_FLAG_FILTER_LINEAR);
					IO_FreeImageData(&album);
				}
			}),
			m_timerFinished(false),
			m_music(0),
			m_lastSelected(buttonField->CurrentlySelected()),
			m_songName("", RenderManager::GetFontRef("menus"), 1, glm::vec4(0.2f, 0.2f, 0.2f, 1.0f)),
			m_songArtist("", RenderManager::GetFontRef("menus"), 1, glm::vec4(0.2f, 0.2f, 0.2f, 1.0f)),
			m_songCharter("", RenderManager::GetFontRef("menus"), 1, glm::vec4(0.2f, 0.2f, 0.2f, 1.0f)),
			m_buttonField(buttonField),
			m_albumImage(g_image, Rendering::TEXTURE_FLAG_CLAMP | Rendering::TEXTURE_FLAG_FILTER_LINEAR, false, false),
			m_hasDoneNoSongTimer(false)
		{
			m_albumImage.SetPosition(albumFrame->GetPosition());
			m_albumImage.SetScale(albumFrame->GetScale());

			m_songName.SetPosition(albumFrame->GetPosition() + glm::vec2(0, albumFrame->GetScale().y + 0.025f));
			m_songName.SetScale(glm::vec2(0.015f));

			m_songArtist.SetPosition(m_songName.GetPosition() + glm::vec2(0, 0.025f));
			m_songArtist.SetScale(glm::vec2(0.015f));

			m_songCharter.SetPosition(m_songArtist.GetPosition() + glm::vec2(0, 0.025f));
			m_songCharter.SetScale(glm::vec2(0.015f));
		}
		~WidgetSongDisplays()
		{ }

		void OnCanvasDeactivated() override
		{
			m_timer.Stop(); // Stop the timer. Don't want more music playing while playing a song

			m_music.ForceStop();
		}

		void OnCanvasActivated() override
		{
			if (!m_hasDoneNoSongTimer)
			{
				if (SongContainer::GetSongs().empty())
					m_hasDoneNoSongTimer = true;
				m_timer.time = SELECTION_DELAY_TIMER;
				m_timer.StartTimer();
				m_timerFinished = false;
			}
		}

		void Render(float interpolation) override
		{
			m_albumImage.Render(interpolation);

			m_songName.Render(interpolation);
			m_songArtist.Render(interpolation);
			m_songCharter.Render(interpolation);
		}

		void Update(float delta) override
		{
			m_music.Update(delta);
			int newSelection = m_buttonField->CurrentlySelected();
			WMP_State state = m_music.GetState();
			if (newSelection != m_lastSelected)
			{
				if (state == WMP_STOPPED)
				{
					m_timer.time = SELECTION_DELAY_TIMER;
					m_timer.StartTimer();
					m_timerFinished = false;

					m_lastSelected = newSelection;
				}
				if (state == WMP_PLAYING || state == WMP_STARTING)
				{
					m_music.Stop();
					m_albumImage.SetImage(g_image, Rendering::TEXTURE_FLAG_CLAMP | Rendering::TEXTURE_FLAG_FILTER_LINEAR);
					m_songName.SetText("");
					m_songArtist.SetText("");
					m_songCharter.SetText("");
				}
			}
			else
			{
				if (state == WMP_STOPPED && m_timerFinished)
					m_music.Play(GetFromIndex(m_lastSelected));
			}
		}
	private:
		Timer m_timer;
		bool m_hasDoneNoSongTimer;

		WidgetMusicPlayer m_music;
		WidgetImage m_albumImage;
		WidgetText m_songName;
		WidgetText m_songArtist;
		WidgetText m_songCharter;
		int m_lastSelected;
		bool m_timerFinished;

		WidgetButtonField *m_buttonField;
	};

	namespace FactoryCanvas
	{
		void CanvasCreateSongSelect(Canvas *canvas, void *canvasData)
		{
			InitImage();
			IO_Image image;
			IO_LoadImage(Util::GetExecutablePath() + "/game_data/images/backgrounds/space_01.png", &image);
			WidgetImage *bg = new WidgetImage(image, Rendering::TEXTURE_FLAG_CLAMP | Rendering::TEXTURE_FLAG_FILTER_LINEAR, true);
			IO_FreeImageData(&image);

			canvas->Add("background", bg);

			WidgetTile *albumImgFrame = new WidgetTile(0, 0, RenderManager::GetSheet("ui_elements"));
			glm::vec2 topLeft = glm::vec2(0.05f, 0.05f);
			glm::vec2 botRight;
			WidgetUtil::AdjustRatio(botRight, (float)IO_Window_GetWidth(), (float)IO_Window_GetHeight());
			botRight *= 0.25f;

			albumImgFrame->SetExtents(topLeft, topLeft + botRight);
			canvas->Add("frame", albumImgFrame);

			WidgetTile *infoBar = new WidgetTile(0, 0, RenderManager::GetSheet("ui_elements"));
			infoBar->SetPosition(glm::vec2(albumImgFrame->GetPosition().x, 0.5f));
			infoBar->SetScale(glm::vec2(albumImgFrame->GetScale().x, 0.4f));
			canvas->Add("bar", infoBar);

			WidgetTile *songButtonsBG = new WidgetTile(0, 0, RenderManager::GetSheet("ui_elements"));
			float temp_x = albumImgFrame->GetPosition().x + albumImgFrame->GetScale().x + 0.01f;
			songButtonsBG->SetExtents(glm::vec2(temp_x, 0.15f), glm::vec2(0.95f, 0.90f));
			canvas->Add("frame2", songButtonsBG);

			WidgetTile *topFrame = new WidgetTile(0, 0, RenderManager::GetSheet("ui_elements"));
			topFrame->SetExtents(glm::vec2(0.05f, 0.05f), glm::vec2(0.95f, 0.14f));
			canvas->Add("frame3", topFrame);

			WidgetButtonField *buttonField = new WidgetButtonField();
			buttonField->SetPosition(glm::vec2(songButtonsBG->GetPosition().x, 0.19f));
			buttonField->SetScale(glm::vec2(songButtonsBG->GetScale().x, 0.67f));

			IO_LoadImage(WGE::FileSystem::GetFileInDirectory("/game_data/images/album.png"), &image);
			WidgetImage *albumImage = new WidgetImage(image, Rendering::TEXTURE_FLAG_CLAMP, false, false);
			IO_FreeImageData(&image);
			albumImage->SetPosition(albumImgFrame->GetPosition());
			albumImage->SetScale(albumImgFrame->GetScale());
			canvas->Add("image", albumImage);

			std::sort(SongContainer::GetSongs().begin(), SongContainer::GetSongs().end(), [](ContainerSong first, ContainerSong second) {
				return first.name.compare(second.name) < 0;
			});

			//WidgetMusicPlayer *musicPlayer = new WidgetMusicPlayer(buttonField);

			for (int i = 0; i < SongContainer::Size(); i++)
			{
				ContainerSong &song = SongContainer::GetSong(i);

				buttonField->AddButton(new WidgetButton(song.name, [&song]() {
						SettingValue value;
						value.type = SETTING_GAME_SONGLIST_DIRECTORY;
						SettingsGet(&value);
					
						if (value.m_sValue[value.m_sValue.size() - 1] != '\\' || value.m_sValue[value.m_sValue.size() - 1] != '/')
							value.m_sValue.append("/");

						if (!StageSystem::GetInstance()->LoadInSong(value.m_sValue + song.songFolder + "/"))
						{
							MenuSystem::GetInstance()->DisplayMessage("Failed to load song!");
						}
						else
						{
							MenuSystem::GetInstance()->QueuePushMenu(CanvasFactory::CreateCanvas(CANVAS_FACTORY_SONG_PREP));
							//musicPlayer->Stop();
						}

						return BUTTON_FINISHED;
					}, 0));
			}
			canvas->Add("button_field", buttonField);
			buttonField->ForceSelect(Global_SongButtonSelected());

			canvas->Add("mmm", new WidgetSongDisplays(buttonField, albumImgFrame));

			WidgetButton *buttonBack = new WidgetButton("<", []() { MenuSystem::GetInstance()->QueuePopMenu(); return BUTTON_FINISHED; }, 1);
			buttonBack->SetExtents(glm::vec2(0.015f, 0.05f), glm::vec2(0.05f, 0.14f));
			canvas->Add("button_back", buttonBack);
			canvas->Add("p10", new WidgetControllerMenuBack(0));

			canvas->Add("search", new WidgetSearch(buttonField));

			canvas->Add("p4", new WidgetPlayerDisplay());
		}
	}
}

#endif

