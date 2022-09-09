#ifndef RHYTHMIC_CANVAS_AUTO_BINDER_HPP_
#define RHYTHMIC_CANVAS_AUTO_BINDER_HPP_

#include "../canvas.hpp"
#include "../widget.hpp"
#include "../widgets/tile.hpp"
#include "../widgets/canvas_preview.hpp"

#include "../canvas_data.hpp"

namespace Rhythmic
{
	class WidgetBinderDrums : public WidgetBase
	{
	public:
		WidgetBinderDrums(InputDeviceHandle handler) :
			WidgetBase(),
			m_text("", RenderManager::GetFontRef("menus"), 1, glm::vec4(0.1f, 0.1f, 0.1f, 1)),
			m_text2("", RenderManager::GetFontRef("menus"), 1, glm::vec4(0.1f, 0.1f, 0.1f, 1)),
			m_state(0),
			m_handle(handler),
			m_buttonRed(-1),
			m_buttonYellow(-1),
			m_buttonBlue(-1),
			m_buttonGreen(-1),
			m_skipButton("Cancel", 0, 1)
		{
			InterceptHandler([this](InputDevice *device, const InputRawButton &button, int rawData)
				{
					if (m_handle != device->handle)
						return;

					if (button.axis >= 0)
						return;
					if (rawData > 0)
						return;
					ProcessInterceptedInput(button);
				}, true);

			m_skipButton.SetDisabled(false);
			m_skipButton.SetExtents(glm::vec2(0.25f, 0.69f), glm::vec2(0.75f, 0.75f));
			m_skipButton.SetOnClick([this]()
				{
					MenuSystem::GetInstance()->QueuePopMenu();
					return BUTTON_FINISHED;
				});

			m_text.SetPosition(glm::vec2(0.5f, 0.4f));
			m_text.SetScale(glm::vec2(0.02f));

			m_text2.SetPosition(glm::vec2(0.5f, 0.5f));
			m_text2.SetScale(glm::vec2(0.015f));

			SetCanvasChangeCallback([this](Canvas *c)
				{
					m_skipButton.SetCanvas(c);
					m_text.SetCanvas(c);
					m_text2.SetCanvas(c);
				});
		}
		virtual ~WidgetBinderDrums()
		{
			InterceptHandler(0);
		}

		void ProcessInterceptedInput(const InputRawButton &button)
		{
			LOG_INFO("Button {0} on state {1}", button.button, m_state);
			m_buttons.push_back(button);
		}

		virtual void Render(float interpolation)
		{
			m_skipButton.Render(interpolation);
			m_text.Render(interpolation);
			m_text2.Render(interpolation);
		}

		virtual void Update(float delta)
		{
			switch (m_state)
			{
			case 0:
				m_text.SetText("Hit the following pads in order:");
				m_text2.SetText("Red, Yellow, Blue, Green");
				m_skipButton.SetDisabled(false);

				if (m_buttons.size() >= 8)
				{
					// Determine each button
					std::map<int, int> countMap;
					InputDevice *device = InputGetDevice(m_handle);

					for (size_t i = 0; i < m_buttons.size(); i++)
					{
						countMap[m_buttons[i].button]++;
					}

					int mostCountButton = -1;

					for (auto it = countMap.begin(); it != countMap.end(); it++)
					{
						if (it->second == 4)
						{
							mostCountButton = it->first;
							break;
						}
					}

					if (mostCountButton < 0)
						m_state = -1;

					for (auto it = m_buttons.begin(); it != m_buttons.end(); it++)
					{
						if (it->button == mostCountButton)
							continue;
						if (m_buttonRed < 0)
						{
							m_buttonRed = it->button;
							InputBindingsAddButton(&device->binding, *it, INPUT_BUTTON_RED);
						}
						else if (m_buttonYellow < 0)
						{
							m_buttonYellow = it->button;
							InputBindingsAddButton(&device->binding, *it, INPUT_BUTTON_YELLOW);
						}
						else if (m_buttonBlue < 0)
						{
							m_buttonBlue = it->button;
							InputBindingsAddButton(&device->binding, *it, INPUT_BUTTON_BLUE);
						}
						else if (m_buttonGreen < 0)
						{
							m_buttonGreen = it->button;
							InputBindingsAddButton(&device->binding, *it, INPUT_BUTTON_GREEN);
						}
					}
					InputBindingsAddButton(&device->binding, InputRawButton(mostCountButton, -1, -1, -1), INPUT_BUTTON_MOD_RB_DRUMS_PAD);

					m_buttons.clear();
					m_state++;
				}
				break;
			case 1:
				m_text.SetText("Now hit any cymbal");
				m_text2.SetText("");
				m_skipButton.GetLabel().SetText("Skip...");
				m_skipButton.SetDisabled(false);

				if (m_buttons.size() >= 2)
				{
					InputDevice *device = InputGetDevice(m_handle);

					int pad = m_buttons[0].button;
					int cymbal = m_buttons[1].button;
					if (pad < 0 || cymbal < 0)
					{
						m_buttons.clear();
						return;
					}

					if (pad == m_buttonRed || pad == m_buttonYellow || pad == m_buttonBlue || pad == m_buttonGreen)
					{
						InputBindingsAddButton(&device->binding, m_buttons[1], INPUT_BUTTON_MOD_RB_DRUMS_CYMBAL);
					}
					m_state = 5;
				}
				break;
			case 2:
				m_skipButton.SetDisabled(true);
				m_state = 5;
				//m_text.SetText("Primary foot pedal!");
				break;
			case 3:
				m_text.SetText("Slam your foot down on your second pedal!");
				m_text2.SetText("Don't break it...");
				break;
			case 4:
				m_text.SetText("Binding Completed");
				m_text2.SetText("");
			case 5:
				MenuSystem::GetInstance()->QueuePopMenu();
			}
		}
	private:
		int m_state;
		WidgetText m_text;
		WidgetText m_text2;
		InputDeviceHandle m_handle;

		WidgetButton m_skipButton;

		std::vector<InputRawButton> m_buttons;

		int m_buttonRed;
		int m_buttonYellow;
		int m_buttonBlue;
		int m_buttonGreen;
	};
	namespace FactoryCanvas
	{
		void CreateCanvasAutoBinder(Canvas *canvas, void *canvasData)
		{
			canvas->Add("fsd", new WidgetCanvasPreview(MenuSystem::GetInstance()->GetTopCanvas()));

			canvas->Add("p4", new WidgetTile(0, 0, RenderManager::GetSheet("ui_elements"), glm::vec4(0, 0, 0, 0.6f)));

			WidgetTile *frame = new WidgetTile(0, 0, RenderManager::GetSheet("ui_elements"), glm::vec4(1, 1, 1, 0.8f));
			frame->SetScale(glm::vec2(0.3f, 0.3f));
			frame->SetPosition(glm::vec2(0.5f));
			canvas->Add("t", frame);

			WidgetText *title = new WidgetText("Auto Binder", RenderManager::GetFontRef("menus"), 1, glm::vec4(0.2f, 0.2f, 0.2f, 1));
			title->SetScale(glm::vec2(0.02f));
			title->SetPosition(glm::vec2(0.5f, 0.3f));
			canvas->Add("tt", title);

			CanvasDataAutoBinder *data = (CanvasDataAutoBinder*) canvasData;

			canvas->Add("sf", new WidgetBinderDrums(data->handle));

			WidgetButton *buttonBack = new WidgetButton("<", []() { MenuSystem::GetInstance()->QueuePopMenu(); return BUTTON_FINISHED; }, 1);
			buttonBack->SetExtents(glm::vec2(0.015f, 0.05f), glm::vec2(0.05f, 0.14f));
			canvas->Add("button_back", buttonBack);
		}
	}
}

#endif
