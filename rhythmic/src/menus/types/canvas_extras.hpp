#ifndef RHYTHMIC_CANVAS_FACTORY_EXTRAS_HPP_
#define RHYTHMIC_CANVAS_FACTORY_EXTRAS_HPP_

#include <random>

#include "../canvas.hpp"

#include "../widgets/image.hpp"
#include "../widgets/text.hpp"
#include "../widgets/button.hpp"
#include "../widgets/button_field.hpp"
#include "../widgets/controller_back_menu.hpp"
#include "../widgets/player_display.hpp"

#include "../menu_system.hpp"

#include "../../util/misc.hpp"

#include "../../rendering/render_manager.hpp"

#include "../../events/event_system.hpp"
#include "../../events/events_common.hpp"

#include "../../util/dear_imgui/imgui.h"
#include "../../util/version.hpp"

#include "../../game/engine_vals.hpp"

#include "../../container/engine/engine_container.hpp"
#include "../../container/engine/engine.hpp"

#include "../../engine/include/wge/core/logging.hpp"

namespace Rhythmic
{
	namespace FactoryCanvas
	{
		class WidgetExtrasIMGUIManager : public WidgetBase
		{
		public:
			WidgetExtrasIMGUIManager() :
				m_showAbout(false),
				m_showNetwork(false),
				m_showEngine(false)
			{
				m_listener = EventAddListener(ET_IMGUI_TICK, std::bind(&WidgetExtrasIMGUIManager::ImGUITick, this, std::placeholders::_1));
			}
			~WidgetExtrasIMGUIManager()
			{
				EventRemoveListener(m_listener);
			}

			void ImGUITick(void *ed)
			{
				if (m_showAbout)
				{
					ImGui::Begin("About Rhythmic", &m_showAbout, ImGuiWindowFlags_NoResize);

					ImGui::Text(GetRhythmicVersionString().c_str());
					ImGui::Separator();
					ImGui::Text("\tRhythmic is a rhythm game similar to Guitar Hero or Rock Band, allowing the\n\tplayer to use either a guitar controller or drums to play along to their favorite songs.");
					ImGui::Separator();
					ImGui::Text("Programmed by:\n"
						"\tRichard (aka Warmful)\n"
						"\tJacob (aka RileyTheFox/EVILCAT6)");
					ImGui::Separator();
					ImGui::Text("Special Thanks To:\n"
						"\tBlair McDonough (rhythmic logo)\n"
						"\tPixelGH\n"
						"\tVenerabela\n"
						"\tGoulart\n"
						"\tVekunin\n"
						"\tPaige\n"
						"\tAedando\n"
						"\tAnd to others who've provided feedback and assets :D"
					);
					
					ImGui::End();
				}

				if (m_showNetwork)
				{
					ImGui::Begin("Network", &m_showNetwork);

					ImGui::Text("Networking Menu");

					static char ipbuffer[33] = { 0 };
					ImGui::InputText("ip", ipbuffer, 32);
					if (ImGui::Button("Connect"))
					{
					}

					if (ImGui::Button("STOP"))
					{
					}

					ImGui::End();
				}
				if (m_showEngine) 
				{
					ImGui::Begin("Engine Creator", &m_showEngine);

					ImGui::Text("Change the values and click \"Export Engine\" when done.");

					ImGui::BeginGroup();
					ImGui::Text("Guitar/Bass variables:");
					ImGui::InputInt("Strum Range MS", &Rhythmic::GamePlayVariables::g_engineFret5.m_strumRange, 1, 5);
					ImGui::InputInt("HOPO/Tap Range MS", &Rhythmic::GamePlayVariables::g_engineFret5.m_hopoTapRange, 1, 5);
					ImGui::InputInt("HOPO/Tap Front End MS", &Rhythmic::GamePlayVariables::g_engineFret5.m_hopoFrontEnd, 1, 5);
					ImGui::Text("Enabling Infinite HOPO Front End will make the MS value irrelevant.");
					ImGui::Checkbox("Infinite HOPO/Tap Front End", &Rhythmic::GamePlayVariables::g_engineFret5.m_infiniteHOPOFrontEnd);
					ImGui::InputInt("Strum Leniency MS", &Rhythmic::GamePlayVariables::g_engineFret5.m_strumLeniency, 1, 5);
					ImGui::InputInt("Hopo/Tap Leniency MS", &Rhythmic::GamePlayVariables::g_engineFret5.m_hopoLeniency, 1, 5);
					ImGui::InputInt("Tap/Hopo Strums Combo Protection MS", &Rhythmic::GamePlayVariables::g_engineFret5.m_timeTapIgnoreOverstrum, 1, 5);
					ImGui::Checkbox("Anti Ghosting", &Rhythmic::GamePlayVariables::g_engineFret5.m_antiGhost);
					ImGui::EndGroup();

					ImGui::Separator();

					ImGui::BeginGroup();
					ImGui::Text("Drum variables:");
					ImGui::InputInt("Range MS", &Rhythmic::GamePlayVariables::g_engineDrums.m_rangeCatcher, 1, 5);
					ImGui::Text("This value controls how long you have to hit all the notes in a chord, before the combo breaks");
					ImGui::InputInt("Chord Timer", &Rhythmic::GamePlayVariables::g_engineDrums.m_chordTime, 1, 5);
					ImGui::InputInt("Starpower Activation Timer", &Rhythmic::GamePlayVariables::g_engineDrums.m_spActivationTime, 1, 5);
					ImGui::Text("Visual Variables Only:");
					ImGui::InputInt("Pad Activation", &Rhythmic::GamePlayVariables::g_engineDrums.m_padActive, 1, 5);
					ImGui::EndGroup();

					ImGui::Separator();

					ImGui::BeginGroup();
					static char buffer[33] = { 0 };
					ImGui::Text("Engine Name:");
					ImGui::InputText("", buffer, 32);
					ImGui::EndGroup();

					if (ImGui::Button("Export Engine")) {
						if (buffer[0] != 0) {
							Rhythmic::Engine gameEngine;

							gameEngine.m_fret5.m_hopoLeniency = Rhythmic::GamePlayVariables::g_engineFret5.m_hopoLeniency;
							gameEngine.m_fret5.m_strumRange = Rhythmic::GamePlayVariables::g_engineFret5.m_strumRange;
							gameEngine.m_fret5.m_hopoTapRange = Rhythmic::GamePlayVariables::g_engineFret5.m_hopoTapRange;
							gameEngine.m_fret5.m_hopoFrontEnd = Rhythmic::GamePlayVariables::g_engineFret5.m_hopoFrontEnd;
							gameEngine.m_fret5.m_infiniteHOPOFrontEnd = Rhythmic::GamePlayVariables::g_engineFret5.m_infiniteHOPOFrontEnd;
							gameEngine.m_fret5.m_strumLeniency = Rhythmic::GamePlayVariables::g_engineFret5.m_strumLeniency;
							gameEngine.m_fret5.m_timeTapIgnoreOverstrum = Rhythmic::GamePlayVariables::g_engineFret5.m_timeTapIgnoreOverstrum;
							gameEngine.m_fret5.m_antiGhost = Rhythmic::GamePlayVariables::g_engineFret5.m_antiGhost;

							gameEngine.m_drums.m_chordTime = Rhythmic::GamePlayVariables::g_engineDrums.m_chordTime;
							gameEngine.m_drums.m_padActive = Rhythmic::GamePlayVariables::g_engineDrums.m_padActive;
							gameEngine.m_drums.m_rangeCatcher = Rhythmic::GamePlayVariables::g_engineDrums.m_rangeCatcher;
							gameEngine.m_drums.m_spActivationTime = Rhythmic::GamePlayVariables::g_engineDrums.m_spActivationTime;

							gameEngine.m_author = std::string("Placeholder Author");
							gameEngine.m_name = std::string(buffer);
							gameEngine.m_isCustom = true;

							WGE::Util::GUID guid = WGE::Util::GUID(true);
							gameEngine.m_guid = guid;

							LOG_INFO(guid.ToString());
							LOG_INFO(guid.ToString().size());

							Rhythmic::EngineContainer::SaveEngineToFile(&gameEngine);
							Rhythmic::EngineContainer::GetEngines()->insert(std::make_pair(guid, gameEngine));
						}
					}

					ImGui::End();
				}
			}

			void ShowNetwork(bool show) { m_showNetwork = show; }
			void ShowAbout(bool show) { m_showAbout = show; }
			void ShowEngine(bool show) { m_showEngine = show; }

			void HideAll()
			{
				m_showNetwork = false;
				m_showAbout = false;
				m_showEngine = false;
			}

			virtual void Update(float update)
			{ }
			virtual void Render(float render)
			{ }
		private:
			EventListener m_listener;

			bool m_showNetwork;
			bool m_showAbout;
			bool m_showEngine;
		};
		void CanvasCreateExtras(Canvas *canvas, void *canvasData)
		{
			IO_Image image;
			IO_LoadImage(Util::GetExecutablePath() + "/game_data/images/backgrounds/space_01.png", &image);
			WidgetImage *bg = new WidgetImage(image, Rendering::TEXTURE_FLAG_CLAMP | Rendering::TEXTURE_FLAG_FILTER_LINEAR, true);
			IO_FreeImageData(&image);
			canvas->Add("p0", bg);

			WidgetExtrasIMGUIManager *imguiMan = new WidgetExtrasIMGUIManager();

			WidgetButtonField *field = new WidgetButtonField(glm::vec4(0, 0, 0, 0.6f), 0);
			field->SetPosition(glm::vec2(0.5f, 0.3f));
			field->SetScale(glm::vec2(0.3f, 0.4));
			field->AddButton(new WidgetButton("Engine Editor", [imguiMan]()
				{
					MenuSystem::GetInstance()->QueuePushMenu(CanvasFactory::CreateCanvas(CANVAS_FACTORY_ENGINE_CREATOR));
					imguiMan->HideAll();

					return BUTTON_FINISHED; 
				}));
			//field->AddButton(new WidgetButton("Engine Editor", [imguiMan]() { imguiMan->ShowEngine(true); return BUTTON_FINISHED; }));
			field->AddButton(new WidgetButton("About", [imguiMan]() { imguiMan->ShowAbout(true); return BUTTON_FINISHED; }));
//#ifdef WGE_DEBUG
			// Secret :P
			std::random_device device;
			std::mt19937 engine(device());
			std::uniform_int_distribution<int> dist(1, 2);

			int rand = dist(engine);

			if (rand == 1) // Show button
			{
				field->AddButton(new WidgetButton("???", []()
					{
						MenuSystem::GetInstance()->QueuePushMenu(CanvasFactory::CreateCanvas(CANVAS_FACTORY_SECRET_UNLOCK));
						return BUTTON_FINISHED;
					}));
			}
//#endif

			//field->AddButton(new WidgetButton("*Network", [imguiMan]() { imguiMan->ShowNetwork(true); return BUTTON_FINISHED; }));
			//field->AddButton(new WidgetButton("*Themify", []() { return BUTTON_FINISHED; }));
			//field->AddButton(new WidgetButton("*Rhythm Editor", []() { return BUTTON_FINISHED; }));
			canvas->Add("p2", field);

			WidgetButton *buttonBack = new WidgetButton("<", []() { MenuSystem::GetInstance()->QueuePopMenu(); return BUTTON_FINISHED; }, 1);
			buttonBack->SetExtents(glm::vec2(0.015f, 0.05f), glm::vec2(0.05f, 0.14f));
			canvas->Add("button_back", buttonBack);
			canvas->Add("p10", new WidgetControllerMenuBack(0));

			WidgetText *disclaimer = new WidgetText("Anything marked with '*' is either experimental or not implemented!", RenderManager::GetFontRef("menus"), 1);
			disclaimer->SetPosition(glm::vec2(0.5f, 0.8f));
			disclaimer->SetScale(glm::vec2(0.02f));
			canvas->Add("p6", disclaimer);

			canvas->Add("p8", new WidgetPlayerDisplay());

			canvas->Add("p4", imguiMan);
		}
	}
}

#endif

