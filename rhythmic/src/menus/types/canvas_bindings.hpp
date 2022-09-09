#ifndef CANVAS_FACTORY_BINDINGS_HPP_
#define CANVAS_FACTORY_BINDINGS_HPP_

#include "../canvas.hpp"

#include "../widgets/button_field.hpp"
#include "../widgets/image.hpp"
#include "../widgets/tile.hpp"
#include "../widgets/controller_back_menu.hpp"
#include "../widgets/player_display.hpp"

#include "../../util/misc.hpp"

#include "../../rendering/render_manager.hpp"

#include "../menu_system.hpp"

#include "../../util/settings.hpp"
#include "../../util/thread_pool.hpp"

#include "../../container/binding_container.hpp"

#include "../../input/input.hpp"
#include "../../input/input_handlers.hpp"

#include "../../events/event_system.hpp"

#include "../../rendering/render_manager.hpp"
#include "../../rendering/glad/glad.h"

#include "../canvas_data.hpp"

#include "../../util/binding_parser.hpp"

#include <wge/io/filesystem.hpp>
#include <fstream>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <vector>

namespace Rhythmic
{
	static int g_canvasBindingSelectedPlayer = 0;
	static std::vector<std::pair<InputRawButton, int>> g_entryAxis;	// This is to keep track of axis. This way, the binder doesn't automatically choose an axis instead of a button

	class WidgetBindingRow : public WidgetBase
	{
	public:
		WidgetBindingRow(WidgetButton *assignPlayer, InputGameButton button_bind, const WidgetButtonColors &color) :
			WidgetBase(),
			m_title(InputGetButtonName(button_bind), RenderManager::GetFontRef("menus")),
			m_assignPlayer(assignPlayer),
			m_addButton("+", 0, 1),
			m_bindingChanged(false),
			m_bindButton(button_bind),
			m_buttonColors(color)
		{
			SetCanvasChangeCallback([this](Canvas *canvas)
				{
					m_title.SetCanvas(canvas);
					m_addButton.SetCanvas(canvas);
				});

			m_addButton.SetOnClick([this,button_bind]()
				{
					g_entryAxis.clear();
					InterceptHandler([this,button_bind](InputDevice *device, const InputRawButton &raw, int rawData)
						{
							ContainerPlayer *player = PlayerContainer::GetPlayerByIndex(g_canvasBindingSelectedPlayer);
							
							if (device != InputGetDevice(INPUT_HANDLE_KEYBOARD) &&
								device != InputGetDevice(player->m_inputHandle))
								return; // The input device doesn't belong to this player!

							if (player)
							{
								// Get the player's actual input device
								// As the device object itself is different per device
								InputDevice *playersDevice = InputGetDevice(player->m_inputHandle);

								if (raw.axis >= 0)
								{
									auto it = std::find_if(g_entryAxis.begin(), g_entryAxis.end(), [raw](std::pair<InputRawButton, int> value)
										{
											return value.first.axis == raw.axis;
										});
									if (it != g_entryAxis.end())
									{
										int difference = rawData - it->second;
										//LOG_INFO("Axis {0} at a difference of {1}", (int)raw.axis, rawData - it->second);
										if (difference >= 7500 || difference <= -7500)
										{
											InputBindingsAddButton(&playersDevice->binding, raw, button_bind);

											m_bindingChanged = true;
											InterceptHandler(0);
											return;
										}
									}
									else // The axis doesn't exist in the map. Just add it
									{
										g_entryAxis.push_back(std::pair<InputRawButton, int>(raw, rawData));
									}
								}
								else
								{
									InputBindingsAddButton(&playersDevice->binding, raw, button_bind);
									m_bindingChanged = true;
									InterceptHandler(0);
									return;
								}
							}
						}, true);
					return BUTTON_FINISHED;
				});
			m_addButton.SetColor(m_buttonColors);

			SetScale(glm::vec2(0, m_addButton.GetScale().y));
		}
		~WidgetBindingRow()
		{
			for (int i = 0; i < m_buttons.size(); i++)
				delete m_buttons[i];
		}

		const WidgetButton &GetAddButton() { return m_addButton; }

		virtual void Update(float delta)
		{
			int activePlayers = PlayerContainer::GetActivePlayers();

			if (m_lastActivePlayers != activePlayers)
			{
				m_lastActivePlayers = activePlayers;

				ResetPosition();
			}

			if (m_title.GetPosition() != m_position || m_bindingChanged)
			{
				ResetPosition();
			}

			m_addButton.SetDisabled(m_assignPlayer->GetForceOn() || m_isDisabled);
			// Get all assigned raw buttons for the given game button, re-adjust m_buttons accordingly. Each button will pull up a pop up to either calibrate axis or remove
			
		}

		virtual void Render(float interpolation)
		{
			m_title.Render(interpolation);

			auto it = m_buttons.begin();
			while (it != m_buttons.end())
			{
				(*it)->Render(interpolation);
				it++;
			}
			auto keyButton = m_keyButtons.begin();
			while (keyButton != m_keyButtons.end())
			{
				(*keyButton)->Render(interpolation);
				keyButton++;
			}

			m_addButton.Render(interpolation);
		}

		void PushButton(const InputRawButton &button)
		{
			std::string thing = "";

			bool addAsKey = false;

			// Generate text for the specific binding
			if (button.axis >= 0)
				thing = "CA_" + std::to_string((int)button.axis);
			else if (button.button >= 0)
				thing = "CB_" + std::to_string(button.button);
			else if (button.keybutton >= 0)
			{
				addAsKey = true;
				const char *key = Util::GetKeyName(button.keybutton); // Returns the key name
				thing = key == 0 ? "" : key;
			}
			else if (button.midi >= 0)
				thing = "M_" + std::to_string(button.midi);
			else
				return; // Do not push the button if there isn't a valid key

			WidgetButton *buttonObj = new WidgetButton(thing, 0, 1);
			buttonObj->SetCanvas(m_canvas);
			buttonObj->SetColor(m_buttonColors);
			buttonObj->SetOnClick([button, this]()
				{
					CanvasDataBindingOptions options;
					options.button = m_bindButton;
					options.rawButton = button;
					options.row = this;

					MenuSystem::GetInstance()->QueuePushMenu(CanvasFactory::CreateCanvas(CANVAS_FACTORY_BINDING_OPTIONS, &options));
					return BUTTON_FINISHED;
				});

			if (addAsKey)
				m_keyButtons.push_back(buttonObj);
			else
				m_buttons.push_back(buttonObj);
		}

		virtual void OnMouseMove(float x, float y)
		{
			if (m_addButton.IsIntersectingPoint(x, y))
				m_addButton.Select();
			else
				m_addButton.Deselect();

			for (int i = 0; i < m_buttons.size(); i++)
			{
				if (m_buttons[i]->IsIntersectingPoint(x, y))
					m_buttons[i]->Select();
				else
					m_buttons[i]->Deselect();
			}

			for (int i = 0; i < m_keyButtons.size(); i++)
			{
				if (m_keyButtons[i]->IsIntersectingPoint(x, y))
					m_keyButtons[i]->Select();
				else
					m_keyButtons[i]->Deselect();
			}
		}

		virtual void OnMouseClick(int button, float x, float y)
		{
			if (m_addButton.IsSelected())
				m_addButton.OnClick();

			for (int i = 0; i < m_buttons.size(); i++)
			{
				WidgetButton *button = m_buttons[i];

				if (button->IsSelected())
					button->OnClick();
			}
			for (int i = 0; i < m_keyButtons.size(); i++)
			{
				WidgetButton *button = m_keyButtons[i];

				if (button->IsSelected())
					button->OnClick();
			}
		}

		void ResetPosition()
		{
			m_title.SetScale(glm::vec2(0.02f));
			m_title.SetPosition(m_position);

			glm::vec2 buttonScale = glm::vec2(0.03f, 0.02f);

			for (int i = 0; i < m_buttons.size(); i++)
				delete m_buttons[i];
			for (int i = 0; i < m_keyButtons.size(); i++)
				delete m_keyButtons[i];
			m_buttons.clear();
			m_keyButtons.clear();

			// Go through binding and readd them all
			ContainerPlayer *player = PlayerContainer::GetPlayerByIndex(g_canvasBindingSelectedPlayer);
			InputDevice *device = InputGetDevice(player->m_inputHandle);
			if (device)
			{
				for (auto it = device->binding.begin(); it != device->binding.end(); it++)
				{
					for (auto button = it->second.begin(); button != it->second.end(); button++)
					{
						if ((*button) == m_bindButton)
							PushButton(it->first);
					}
				}
			}

			m_addButton.SetPosition(m_position + glm::vec2(0.3f, 0));
			m_addButton.SetScale(buttonScale);
			
			// Go through keyboard bindings...

			for (int i = 0; i < m_buttons.size(); i++)
			{
				m_buttons[i]->SetPosition(m_position + glm::vec2(0.4f + (i * buttonScale.x * 2.1f), 0));
				m_buttons[i]->SetScale(buttonScale);
			}
			for (int i = 0; i < m_keyButtons.size(); i++)
			{
				m_keyButtons[i]->SetPosition(m_position + glm::vec2(0.6f + (i * buttonScale.x * 2.1f), 0));
				m_keyButtons[i]->SetScale(buttonScale);
			}

			m_bindingChanged = false;
		}

		InputGameButton GetGameButton() { return m_bindButton; }

	private:
		WidgetText m_title;

		WidgetButton *m_assignPlayer;

		InputGameButton m_bindButton;

		bool m_bindingChanged;
		std::vector<WidgetButton*> m_buttons;		// List of GamePad buttons and axes
		WidgetButton m_addButton;								// Add button for GamePad buttons and axes
		std::vector<WidgetButton*> m_keyButtons;// List of key buttons
		//WidgetButton m_addKeyButton;						// Add button for Keyboard

		const WidgetButtonColors &m_buttonColors;

		int m_lastActivePlayers;
	};
	
	//------------------------------------
	// Binding Buttons
	//------------------------------------

	class WidgetBindingButtons : public WidgetBase
	{
	public:
		WidgetBindingButtons(WidgetButton *assignPlayer, const WidgetButtonColors &colors) :
			WidgetBase(),
			m_colors(colors),
			m_assignPlayer(assignPlayer),
			m_buttonInstrumentDrumsRB("RB 2/3 Drums Auto Bind", 0, 1),
			m_buttonInstrumentGuitar5("5 Fret Guitar", 0, 1),
			m_buttonInstrumentController("Simple Controller", 0, 1),
			m_scrollOffset(0),
			m_maxContentYSize(0)
		{
			m_buttonInstrumentDrumsRB.SetExtents(glm::vec2(0.056f, 0.164), glm::vec2(0.3f, 0.214f));
			m_buttonInstrumentDrumsRB.SetColor(m_colors);
			m_buttonInstrumentDrumsRB.SetOnClick([]()
				{
					CanvasDataAutoBinder binder;
					binder.binderType = CanvasDataAutoBinder::AUTO_BINDER_DRUMS;
					binder.handle = PlayerContainer::GetPlayerByIndex(g_canvasBindingSelectedPlayer)->m_inputHandle;
					MenuSystem::GetInstance()->QueuePushMenu(CanvasFactory::CreateCanvas(CANVAS_FACTORY_AUTO_BINDER, &binder));
					return BUTTON_FINISHED;
				});

			const glm::vec2 &scale = m_buttonInstrumentDrumsRB.GetScale();

			m_buttonInstrumentGuitar5.SetPosition(m_buttonInstrumentDrumsRB.GetPosition() + (glm::vec2(2.1f, 0) * scale));
			m_buttonInstrumentGuitar5.SetColor(m_colors);
			m_buttonInstrumentGuitar5.SetScale(scale);

			m_buttonInstrumentController.SetPosition(m_buttonInstrumentDrumsRB.GetPosition() + (glm::vec2(4.2f, 0) * scale));
			m_buttonInstrumentController.SetScale(scale);
			m_buttonInstrumentController.SetColor(m_colors);

			int offset = 0;
			glm::vec2 textScale = glm::vec2(0.02f);
			for (int i = 0; i < INPUT_BUTTON_size; i++)
			{
				std::string text;
				switch (i)
				{
				case 0:
					text = "~~~~ Basics ~~~~";
					break;
				case 11:
					text = "~~~~ Guitar / Bass / Keys ~~~~";
					break;
				case 13:
					text = "~~~~ Drums ~~~~";
					break;
				case 17:
					text = "~~~~ Cymbals for non RB drums ~~~~";
					break;
				}
				if (!text.empty())
				{
					if (offset > 0)
					{
						m_bindings.push_back(new WidgetText("", RenderManager::GetFontRef("menus")));
						offset++;
					}
					
					WidgetText *textElement = new WidgetText(text, RenderManager::GetFontRef("menus"));
					textElement->SetScale(textScale);
					textElement->SetPosition(glm::vec2(0.1f, 0.25f + (offset * 0.045f)));
					m_bindings.push_back(textElement);
					offset++;
				}
				WidgetBindingRow *row = new WidgetBindingRow(m_assignPlayer, (InputGameButton)i, colors);
				m_bindings.push_back(row);
				row->SetPosition(glm::vec2(0.1f, 0.25f + (offset * 0.045f)));

				offset++;
			}

			m_maxContentYSize = m_bindings.back()->GetPosition().y + m_bindings.back()->GetScale().y * 0.5f;

			SetCanvasChangeCallback([this](Canvas* canvas) 
				{
					m_buttonInstrumentGuitar5.SetCanvas(canvas);
					m_buttonInstrumentDrumsRB.SetCanvas(canvas);
					m_buttonInstrumentController.SetCanvas(canvas);
					for (int i = 0; i < m_bindings.size(); i++)
					{
						m_bindings[i]->SetCanvas(canvas);
					}
				});
		}
		~WidgetBindingButtons()
		{
			for (int i = 0; i < m_bindings.size(); i++)
				delete m_bindings[i];
		}
		
		void SetScrollOffset(const glm::vec2 &offset)
		{
			float scrollAmnt = offset.y;
			if (scrollAmnt > 0)
				scrollAmnt = 0;

			//bool isLastYInRange = InsideRange(m_lastY);

			if (scrollAmnt != m_scrollOffset)
			{
				for (int i = 0; i < m_bindings.size(); i++)
				{
					WidgetBase *row = m_bindings[i];

					row->SetPosition(glm::vec2(0.1f, scrollAmnt + 0.25f + (i * 0.045f)));
					row->Update(0);
				}
				m_scrollOffset = scrollAmnt;
			}
		}
		virtual void Update(float delta)
		{
			bool playerHasNoInput = InputGetDevice(PlayerContainer::GetPlayerByIndex(g_canvasBindingSelectedPlayer)->m_inputHandle) == 0;
			for (int i = 0; i < m_bindings.size(); i++)
			{
				m_bindings[i]->SetDisabled(playerHasNoInput);
				m_bindings[i]->Update(delta);
			}
			m_buttonInstrumentDrumsRB.Update(delta);
		}

		virtual void Render(float interpolation)
		{
			m_buttonInstrumentDrumsRB.Render(interpolation);
			//m_buttonInstrumentGuitar5.Render(interpolation);
			//m_buttonInstrumentController.Render(interpolation);
			
			glEnable(GL_SCISSOR_TEST);
			glm::vec2 pos = glm::vec2(IO_Window_GetWidth(), IO_Window_GetHeight());
			glm::vec2 sca = pos * m_scale;
			pos *= m_position;

			float height = (float)(0.03 * IO_Window_GetHeight());

			glScissor(
				0, (int)(IO_Window_GetHeight() * 0.15f),
				IO_Window_GetWidth(), (int)(IO_Window_GetHeight() * 0.63f)
			);

			for (int i = 0; i < m_bindings.size(); i++)
			{
				m_bindings[i]->Render(interpolation);
			}

			glDisable(GL_SCISSOR_TEST);
		}

		void Reset()
		{ }

		bool InsideRange(float y)
		{
			return (y > 0.22f && y < 0.85f);
		}

		virtual void OnScroll(float dx, float dy) 
		{
			SetScrollOffset(
				glm::vec2(0, glm::clamp(m_scrollOffset + dy * 0.05f, -(glm::max(0.0f, m_maxContentYSize - m_scale.y * 1.25f)), 0.0f)));
		}
		virtual void OnMouseMove(float x, float y)
		{
			m_buttonInstrumentDrumsRB.OnMouseMove(x, y);

			if (!InsideRange(y))
				return;

			for (int i = 0; i < m_bindings.size(); i++)
			{
				m_bindings[i]->OnMouseMove(x, y);
			}
		}
		virtual void OnMouseClick(int button, float x, float y)
		{
			m_buttonInstrumentDrumsRB.OnMouseClick(button, x, y);

			if (!(y > 0.22f && y < 0.85f)) // Copied from InsideRange of the widget below
				return;

			for (int i = 0; i < m_bindings.size(); i++)
				m_bindings[i]->OnMouseClick(button, x, y);
		}

	private:
		WidgetButton m_buttonInstrumentGuitar5;
		WidgetButton m_buttonInstrumentDrumsRB;
		WidgetButton m_buttonInstrumentController;

		WidgetButton *m_assignPlayer;

		float m_scrollOffset;
		float m_maxContentYSize;

		const WidgetButtonColors &m_colors;

		std::vector<WidgetBase*> m_bindings;
	};

	//--------------------------------------------
	// Player buttons disabler and enabler
	//--------------------------------------------

	class WidgetPlayerButtonsModifier : public WidgetBase
	{
	public:
		WidgetPlayerButtonsModifier(WidgetButton *player1, WidgetButton *player2, WidgetButton *player3, WidgetButton *player4) :
			WidgetBase(),
			m_player1(player1),
			m_player2(player2),
			m_player3(player3),
			m_player4(player4)
		{ }

		~WidgetPlayerButtonsModifier() { }

		virtual void Update(float delta)
		{
			int activePlayers = PlayerContainer::GetActivePlayers();

			m_player2->SetDisabled(activePlayers < 1);
			m_player3->SetDisabled(activePlayers < 2);
			m_player4->SetDisabled(activePlayers < 3);

			if (g_canvasBindingSelectedPlayer > activePlayers && activePlayers != 0)
				g_canvasBindingSelectedPlayer = activePlayers;
			else if (activePlayers == 0 && g_canvasBindingSelectedPlayer != 0)
				g_canvasBindingSelectedPlayer = 0;

			m_player1->SetForceOn(g_canvasBindingSelectedPlayer == 0);
			m_player2->SetForceOn(g_canvasBindingSelectedPlayer == 1);
			m_player3->SetForceOn(g_canvasBindingSelectedPlayer == 2);
			m_player4->SetForceOn(g_canvasBindingSelectedPlayer == 3);
		}

		virtual void Render(float interpolation) { }
		virtual void ProcessInput(InputEventData *input, int playerIndex) { }
	private:
		WidgetButton *m_player1;
		WidgetButton *m_player2;
		WidgetButton *m_player3;
		WidgetButton *m_player4;
	};

	//--------------------------------------------
	// Canvas
	//--------------------------------------------

	namespace FactoryCanvas
	{
		void CanvasCreateBindings(Canvas *canvas, void *canvasData)
		{
			static bool pressAnyKey = false;
			pressAnyKey = false;

			g_canvasBindingSelectedPlayer = 0;

			canvas->Add("bg", new WidgetCanvasPreview(MenuSystem::GetInstance()->GetTopCanvas()));

			WidgetTile *tile = new WidgetTile(0, 0, RenderManager::GetSheet("ui_elements"), glm::vec4(0, 0, 0, 0.6f));
			canvas->Add("p4", tile);


			canvas->Add("player_display", new WidgetPlayerDisplay());

			WidgetButton *buttonBack = new WidgetButton("<", []() { 
					SettingsSave();
					MenuSystem::GetInstance()->QueuePopMenu();
					if (pressAnyKey)
					{
						InterceptHandler(0);
					}
					return BUTTON_FINISHED;
				}, 1);
			buttonBack->SetExtents(glm::vec2(0.015f, 0.05f), glm::vec2(0.05f, 0.14f));
			canvas->Add("button_back", buttonBack);

			const static WidgetButtonColors colors = {
				glm::vec4(0.6f, 0.6f, 0.6f, 1.0f),		// Normal
				glm::vec4(0.1f, 0.1f, 0.1f, 1.0f),		// Disabled
				glm::vec4(0.6f, 0.6f, 0.9f, 1.0f),		// Selected
				glm::vec4(0.3f, 0.3f, 0.6f, 1.0f),		// Disabled Selected
				glm::vec4(0.7f, 0.8f, 1.0f, 1.0f),		// Forced On

				glm::vec4(0.3f, 0.3f, 0.3f, 1.0f),		// Normal
				glm::vec4(0.7f, 0.7f, 0.7f, 1.0f),		// Disabled
				glm::vec4(0.3f, 0.3f, 0.3f, 1.0f),		// Selected
				glm::vec4(0.7f, 0.7f, 0.7f, 1.0f),		// Disabled Selected
				glm::vec4(0.3f, 0.3f, 0.3f, 1.0f)		// Forced On
			};

			WidgetButton *buttonPlayer1 = new WidgetButton("Player 1", []() { g_canvasBindingSelectedPlayer = 0; return BUTTON_FINISHED; }, 1);
			buttonPlayer1->SetExtents(glm::vec2(0.056f, 0.05f), glm::vec2(0.15f, 0.1f));
			buttonPlayer1->SetColor(colors);
			canvas->Add("player1", buttonPlayer1);

			glm::vec2 scale = buttonPlayer1->GetScale();

			WidgetButton *buttonPlayer2 = new WidgetButton("Player 2", []() { g_canvasBindingSelectedPlayer = 1; return BUTTON_FINISHED; }, 1);
			buttonPlayer2->SetScale(scale);
			buttonPlayer2->SetPosition(buttonPlayer1->GetPosition() + glm::vec2(scale.x * 2 + 0.006f, 0));
			buttonPlayer2->SetColor(colors);
			buttonPlayer2->SetDisabled(true);
			canvas->Add("player2", buttonPlayer2);

			WidgetButton *buttonPlayer3 = new WidgetButton("Player 3", []() { g_canvasBindingSelectedPlayer = 2; return BUTTON_FINISHED; }, 1);
			buttonPlayer3->SetScale(scale);
			buttonPlayer3->SetPosition(buttonPlayer2->GetPosition() + glm::vec2(scale.x * 2 + 0.006f, 0));
			buttonPlayer3->SetColor(colors);
			buttonPlayer3->SetDisabled(true);
			canvas->Add("player3", buttonPlayer3);

			WidgetButton *buttonPlayer4 = new WidgetButton("Player 4", []() { g_canvasBindingSelectedPlayer = 3; return BUTTON_FINISHED; }, 1);
			buttonPlayer4->SetScale(scale);
			buttonPlayer4->SetPosition(buttonPlayer3->GetPosition() + glm::vec2(scale.x * 2 + 0.006f, 0));
			buttonPlayer4->SetColor(colors);
			buttonPlayer4->SetDisabled(true);
			canvas->Add("player4", buttonPlayer4);

			WidgetButton *buttonAssignPlayer = new WidgetButton("Assign Player", 0, 1);
			buttonAssignPlayer->SetOnClick([buttonAssignPlayer]() {
					pressAnyKey = !pressAnyKey;
					if (pressAnyKey)
					{
						InterceptHandler([buttonAssignPlayer](InputDevice *device, const InputRawButton &input, int) {
							if (input.button < 0 &&
								input.keybutton < 0)
								return;

							pressAnyKey = false;

							if (PlayerContainer::GetActivePlayers() >= 4)
								return;

							PlayerContainer::AssignNewPlayer(device);

							buttonAssignPlayer->SetForceOn(false);
							InterceptHandler(0);
							});
					}
					else
					{
						InterceptHandler(0);
					}
					buttonAssignPlayer->SetForceOn(pressAnyKey);
					return BUTTON_FINISHED;
				});
			buttonAssignPlayer->SetExtents(glm::vec2(0.056f, 0.05f), glm::vec2(0.2f, 0.1f));
			buttonAssignPlayer->SetPosition(buttonPlayer4->GetPosition() + glm::vec2(scale.x + buttonAssignPlayer->GetScale().x + 0.006f, 0));
			buttonAssignPlayer->SetColor(colors);
			canvas->Add("assign", buttonAssignPlayer);


			WidgetBindingButtons *buttons = new WidgetBindingButtons(buttonAssignPlayer, colors);
			canvas->Add("binding", buttons);

			WidgetButton *buttonSaveBinding = new WidgetButton("Save Controller Binding", 0, 1);
			buttonSaveBinding->SetExtents(glm::vec2(0.056f, 0.106f), glm::vec2(0.29f, 0.156f));
			buttonSaveBinding->SetColor(colors);
			buttonSaveBinding->SetOnClick([]()
				{
					ContainerPlayer *player = PlayerContainer::GetPlayerByIndex(g_canvasBindingSelectedPlayer);

					if (player->m_playerID == 5)
						return BUTTON_FINISHED;

					InputDevice *device = InputGetDevice(player->m_inputHandle);

					ContainerBinding bindingContainer;
					bindingContainer.binding = device->binding;
					bindingContainer.calibrations = device->axisCalibrations;

					BindingContainer::InsertBinding(device->guid, bindingContainer);

					return BUTTON_FINISHED;
				});
			canvas->Add("save", buttonSaveBinding);

			WidgetButton *buttonSaveBindingPlayer = new WidgetButton("Save Player's Binding", 0, 1);
			buttonSaveBindingPlayer->SetExtents(glm::vec2(0.296f, 0.106f), glm::vec2(0.54f, 0.156f));
			buttonSaveBindingPlayer->SetColor(colors);
			buttonSaveBindingPlayer->SetOnClick([]()
				{
					// Save a copy of the binding but use only for the player
					// Will eventually be scaled to save to the profile instead of player index
					MenuSystem::GetInstance()->DisplayMessage("Not yet implemented");
					return BUTTON_FINISHED;
				});
			canvas->Add("save_player", buttonSaveBindingPlayer);


			WidgetButton *buttonLoadBinding = new WidgetButton("Load Binding", 0, 1);
			buttonLoadBinding->SetExtents(glm::vec2(0.546f, 0.106f), glm::vec2(0.706f, 0.156f));
			buttonLoadBinding->SetColor(colors);
			buttonLoadBinding->SetDisabled(true);
			buttonLoadBinding->SetOnClick([buttons]() {
				ContainerPlayer *player = PlayerContainer::GetPlayerByIndex(g_canvasBindingSelectedPlayer);

				if (player->m_playerID == 5)
					return BUTTON_FINISHED;

				InputDevice *device = InputGetDevice(player->m_inputHandle);
				

				/*std::ifstream file;
				file.open(Util::GetExecutablePath() + "/configs/buttons.wbc");

				BindingFileHeader header;
				file.read((char *)&header.magic, 4);
				file.read(&header.version, 1);
				file.read((char *)&header.binding_count, 2);
				file.read(&header.controller_type, 1);
				file.read(header.reserved, 8);

				for (short i = 0; i < header.binding_count; i++)
				{
					char type = -1;
					short gameButton = -1;
					int raw = -1;

					file.read(&type, 1);
					file.read((char *)&gameButton, 2);
					file.read((char *)&raw, 4);

					InputRawButton rawButton;
					rawButton.axis = type == 1 ? raw : -1;
					rawButton.keybutton = type == 2 ? raw : -1;
					rawButton.midi = type == 3 ? raw : -1;
					rawButton.button = type == 0 ? raw : -1;

					InputBindingsAddButton(&device->binding, rawButton, (InputGameButton) gameButton);
				}

				buttons->OnScroll(0, -1);
				buttons->OnScroll(0, 1);*/

				return BUTTON_FINISHED;
				});
			canvas->Add("load", buttonLoadBinding);

			/*WidgetBindingRow *row = new WidgetBindingRow(INPUT_BUTTON_START, colors);
			row->SetPosition(glm::vec2(0.05f, 0.2f));
			canvas->Add("test_row", row);
			WidgetBindingRow *row2 = new WidgetBindingRow(INPUT_BUTTON_ACCEPT, colors);
			row2->SetPosition(glm::vec2(0.05f, 0.25f));
			canvas->Add("test_row2", row2);*/


			canvas->Add("updater", new WidgetPlayerButtonsModifier(buttonPlayer1, buttonPlayer2, buttonPlayer3, buttonPlayer4));
		}
	}
}

#endif

