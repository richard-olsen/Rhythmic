#ifndef CANVAS_FACTORY_BINDING_OPTIONS_HPP_
#define CANVAS_FACTORY_BINDING_OPTIONS_HPP_

#include "../menu_system.hpp"

#include "../widgets/button.hpp"
#include "../widgets/canvas_preview.hpp"
#include "../widgets/tile.hpp"

#include "../../rendering/render_manager.hpp"

#include "../../input/input_game.hpp"

#include "../canvas_data.hpp"

#include "canvas_bindings.hpp" // This has the Widgets this menu requires

#include "../widgets/progress_bar.hpp"

namespace Rhythmic
{
	extern int g_canvasBindingSelectedPlayer;

	class WidgetAxisCalibrator : public WidgetBase
	{
	public:
		WidgetAxisCalibrator(int axis) :
			WidgetBase(),
			m_calibrating(false),
			m_axis(axis),
			m_calibrateState(-1),
			m_lastState(-1),
			m_text("", RenderManager::GetFontRef("menus"), 1, glm::vec4(0.3, 0.3, 0.3, 1.0)),
			m_information("", RenderManager::GetFontRef("menus"), 1, glm::vec4(0.3, 0.3, 0.3, 1.0)),
			m_rawValue(0),
			m_calValue(0)
		{
			m_information.SetPosition(glm::vec2(0.5f, 0.5f));
			m_information.SetScale(glm::vec2(0.015f, 0.015f));
			m_text.SetPosition(glm::vec2(0.5f, 0.45f));
			m_text.SetScale(glm::vec2(0.03f));

			m_rawValue.SetPosition(glm::vec2(0.5f, 0.55f));
			m_rawValue.SetScale(glm::vec2(0.25f, 0.02f));
			m_calValue.SetPosition(glm::vec2(0.5f, 0.6f));
			m_calValue.SetScale(glm::vec2(0.25f, 0.02f));

			m_calValue.SetProgress(0);
			m_rawValue.SetProgress(0);
		}
		~WidgetAxisCalibrator()
		{ }

		virtual void Update(float delta)
		{
			if (m_axis < 0)
				return;

			InputDevice *device = InputGetDevice(PlayerContainer::GetPlayerByIndex(g_canvasBindingSelectedPlayer)->m_inputHandle);
			// Determine state
			if (m_calibrateState != m_lastState)
			{
				m_lastState = m_calibrateState;
				switch (m_calibrateState)
				{
				case 0:
					m_information.SetText("Please leave the axis at a resting state...");
					StartCountdown();
					break;
				case 1:
					m_information.SetText("Now hold the axis to it's maximum...");
					StartCountdown();
					break;
				case 2:
					m_information.SetText("Calibration Completed");
					m_text.SetText("");
					m_calibrating = false;
					break;
				}
			}
			if (m_calibrateState >= 0 && m_calibrateState < 2)
			{
				m_text.SetText(std::to_string((int)m_timer));
				m_timer -= delta;
				if (m_timer <= 0)
				{
					if (device == 0)
						return;

					switch (m_calibrateState)
					{
					case 0:
						device->axisCalibrations[m_axis].min = InputGetRawAxis(device, m_axis);
						break;
					case 1:
						device->axisCalibrations[m_axis].max = InputGetRawAxis(device, m_axis);
						break;
					}

					m_calibrateState++;
				}
			}

			int rawAxis = InputGetRawAxis(device, m_axis);

			m_calValue.SetProgress(InputHandlerGetCalibratedState(rawAxis, device->axisCalibrations[m_axis]));
			InputAxisCalibration rawCal = { SHRT_MIN, SHRT_MAX };
			m_rawValue.SetProgress(InputHandlerGetCalibratedState(rawAxis, rawCal));
			
			m_rawValue.Update(delta);
			m_calValue.Update(delta);
		}
		virtual void Render(float interpolation)
		{
			if (m_axis < 0)
				return;

			m_text.Render(interpolation);
			m_information.Render(interpolation);
			m_rawValue.Render(interpolation);
			m_calValue.Render(interpolation);
		}

		void StartCountdown()
		{
			m_timer = 5.0f;
		}

		bool IsCalibrating() { return m_calibrating; }

		void StartCalibration()
		{
			m_calibrating = true;
			m_calibrateState = 0;
		}

		virtual void ProcessInput(InputEventData *input, int playerIndex)
		{
		}
	private:
		WidgetText m_information;
		WidgetText m_text;
		WidgetProgressBar m_rawValue;
		WidgetProgressBar m_calValue;
		int m_calibrateState;
		int m_lastState;
		bool m_calibrating;

		int m_axis;

		float m_timer;
	};
	namespace FactoryCanvas
	{
		void CanvasCreateBindingOptions(Canvas *canvas, void *canvasData)
		{
			CanvasDataBindingOptions *options = (CanvasDataBindingOptions*)canvasData;

			canvas->Add("fsd", new WidgetCanvasPreview(MenuSystem::GetInstance()->GetTopCanvas()));

			canvas->Add("p4", new WidgetTile(0, 0, RenderManager::GetSheet("ui_elements"), glm::vec4(0, 0, 0, 0.6f)));
			
			WidgetAxisCalibrator *axisCalibrator = new WidgetAxisCalibrator(options->rawButton.axis);

			WidgetTile *frame = new WidgetTile(0, 0, RenderManager::GetSheet("ui_elements"), glm::vec4(1, 1, 1, 0.8f));
			frame->SetScale(glm::vec2(0.3f, 0.3f));
			frame->SetPosition(glm::vec2(0.5f));
			canvas->Add("t", frame);

			WidgetText *title = new WidgetText("Binding Options", RenderManager::GetFontRef("menus"), 1, glm::vec4(0.2f,0.2f,0.2f,1));
			title->SetScale(glm::vec2(0.02f));
			title->SetPosition(glm::vec2(0.5f, 0.3f));
			canvas->Add("tt", title);

			WidgetButton *buttonVar = new WidgetButton("Remove", 0, 1);
			buttonVar->SetExtents(glm::vec2(0.3f, 0.65f), glm::vec2(0.48f, 0.7f));
			InputRawButton rawButton = options->rawButton;
			WidgetBindingRow *bindingRow = options->row;
			buttonVar->SetOnClick([rawButton, bindingRow]() 
				{
					ContainerPlayer *player = PlayerContainer::GetPlayerByIndex(g_canvasBindingSelectedPlayer);
					InputDevice *device = InputGetDevice(player->m_inputHandle);
					if (device)
					{
						/*for (auto it = device->binding.begin(); it != device->binding.end(); it++)
						{
							if (it->first == rawButton)
							{
								device->binding.erase(it);
								bindingRow->ResetPosition();
								MenuSystem::GetInstance()->QueuePopMenu();
								break;
							}
						}*/
						InputBindingsRemoveButton(&device->binding, rawButton, bindingRow->GetGameButton());
						bindingRow->ResetPosition();
					}
					MenuSystem::GetInstance()->QueuePopMenu();
					return BUTTON_FINISHED;
				});
			canvas->Add("r", buttonVar);
			
			int axis = (int)options->rawButton.axis;
			buttonVar = new WidgetButton("Calibrate Axis", 0, 1);
			buttonVar->SetExtents(glm::vec2(0.52f, 0.65f), glm::vec2(0.7f, 0.7f));
			buttonVar->SetDisabled(axis < 0);
			buttonVar->SetOnClick([axisCalibrator]()
				{
					if (!axisCalibrator->IsCalibrating())
						axisCalibrator->StartCalibration();
					return BUTTON_FINISHED;
				});
			canvas->Add("c", buttonVar);

			// Get name of button
			std::string thing = "";

			if (options->rawButton.axis >= 0)
				thing = "Controller Axis " + std::to_string((int)options->rawButton.axis);
			else if (options->rawButton.button >= 0)
				thing = "Controller Button " + std::to_string(options->rawButton.button);
			else if (options->rawButton.keybutton >= 0)
			{
				const char *key = Util::GetKeyName(options->rawButton.keybutton); // Returns the key name
				thing = key == 0 ? "" : key;
			}
			else if (options->rawButton.midi >= 0)
				thing = "MIDI " + std::to_string(options->rawButton.midi);

			title = new WidgetText((thing + " is bound to " + InputGetButtonName(options->button)).c_str(), RenderManager::GetFontRef("menus"), 1, glm::vec4(0.2f, 0.2f, 0.2f, 1));
			title->SetPosition(glm::vec2(0.5f, 0.4f));
			title->SetScale(glm::vec2(0.015f));
			canvas->Add("jfj", title);
			/////////////////
			
			WidgetButton *buttonBack = new WidgetButton("<", [axisCalibrator, axis]() {
				if ((!axisCalibrator->IsCalibrating() && axis >= 0) || axis < 0) // Do not leave the menu while user is calibrating their axis
					MenuSystem::GetInstance()->QueuePopMenu();
				return BUTTON_FINISHED;
				}, 1);

			buttonBack->SetExtents(glm::vec2(0.015f, 0.05f), glm::vec2(0.05f, 0.14f));
			canvas->Add("button_back", buttonBack);


			canvas->Add("b", axisCalibrator); // Just to make sure all it's stuff is on top
		}
	}
}

#endif

