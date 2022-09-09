#ifndef CANVAS_FACTORY_PAUSE_OPTIONS_HPP_
#define CANVAS_FACTORY_PAUSE_OPTIONS_HPP_
// A watered down version of the options menu for in game

#include "../canvas.hpp"

#include "../widgets/button_field.hpp"
#include "../widgets/tile.hpp"

namespace Rhythmic
{
	namespace FactoryCanvas
	{
		void CanvasCreatePauseOptions(Canvas *canvas, void *canvasData)
		{
			WidgetTile *frame = new WidgetTile(0, 0, RenderManager::GetSheet("ui_elements"), glm::vec4(1, 1, 1, 0.7f));
			frame->SetScale(glm::vec2(0.2f, 0.35f));
			frame->SetPosition(glm::vec2(0.25f, 0.5f));
			canvas->Add("frame", frame);

			WidgetButtonField *buttonField = new WidgetButtonField(glm::vec4(0, 0, 0, 0.6f));
			buttonField->SetPosition(frame->GetPosition() - glm::vec2(0, frame->GetScale().y - 0.04f));
			buttonField->SetScale(frame->GetScale() - glm::vec2(0, -0.04f));

			static int video;
			static int audio;
			static int volMaster;
			static int volSFX;

			SettingValue value;

			value.type = SETTING_GAME_CALIBRATION_VIDEO;
			SettingsGet(&value);
			video = value.m_iValue;

			value.type = SETTING_GAME_CALIBRATION_AUDIO;
			SettingsGet(&value);
			audio = value.m_iValue;

			value.type = SETTING_AUDIO_MASTER;
			SettingsGet(&value);
			volMaster = value.m_iValue;

			value.type = SETTING_AUDIO_MENU_MUSIC_VOLUME;
			SettingsGet(&value);
			volSFX = value.m_iValue;


			buttonField->AddButton(new WidgetButton("Video Calibration: ", []() { return BUTTON_TOGGLE_SCROLL; }, 0, [](int i) {
					video += i;

					SettingValue value;
					value.type = SETTING_GAME_CALIBRATION_VIDEO;
					value.m_iValue = video;
					SettingsSet(value);

					return BUTTON_FINISHED;
				}));
			buttonField->AddButton(new WidgetButton("Audio Calibration: ", []() { return BUTTON_TOGGLE_SCROLL; }, 0, [](int i) {
					audio += i;

					SettingValue value;
					value.type = SETTING_GAME_CALIBRATION_AUDIO;
					value.m_iValue = audio;
					SettingsSet(value);

					return BUTTON_FINISHED;
				}));
			buttonField->AddButton(new WidgetButton("Master Volume: ",
				[]()
				{
					return BUTTON_TOGGLE_SCROLL;
				}, 0,
				[](int volume)
				{
					volMaster += volume * 5;

					volMaster = glm::clamp(volMaster, 0, 100);

					SettingValue value;
					value.type = SETTING_AUDIO_MASTER;
					value.m_iValue = volMaster;
					SettingsSet(value);

					AudioSystem::GetInstance()->SetMasterVolume((float)volMaster / 100.0f);

					return BUTTON_FINISHED;
				}));

			buttonField->AddButton(new WidgetButton("Sound Effects: ",
				[]()
				{
					return BUTTON_TOGGLE_SCROLL;
				}, 0,
				[](int volume)
				{
					volSFX += volume * 5;

					volSFX = glm::clamp(volSFX, 0, 100);

					SettingValue value;
					value.type = SETTING_AUDIO_SFX;
					value.m_iValue = volSFX;
					SettingsSet(value);

					AudioSystem::GetInstance()->SetVolume(AUDIO_TYPE_SFX, (float)volSFX / 100.0f);

					return BUTTON_FINISHED;
				}));
			buttonField->Get(0)->GetLabel().TagIntAtEnd(&video);
			buttonField->Get(1)->GetLabel().TagIntAtEnd(&audio);
			buttonField->Get(2)->GetLabel().TagIntAtEnd(&volMaster);
			buttonField->Get(3)->GetLabel().TagIntAtEnd(&volSFX);

			canvas->Add("buttons", buttonField);

			canvas->Add("back", new WidgetControllerMenuBack(0, []() { SettingsSave(); }));

			WidgetButton *buttonBack = new WidgetButton("<", []() { SettingsSave(); MenuSystem::GetInstance()->QueuePopMenu(); return BUTTON_FINISHED; }, 1);
			buttonBack->SetExtents(glm::vec2(0.015f, 0.05f), glm::vec2(0.05f, 0.14f));
			canvas->Add("button_back", buttonBack);
		}
	}
}

#endif

