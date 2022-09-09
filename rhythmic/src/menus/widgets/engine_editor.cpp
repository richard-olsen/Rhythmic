#include "engine_editor.hpp"

#include <filesystem>

#include "../../rendering/render_manager.hpp"

#include "../../container/player_container.hpp"
#include "../../container/engine/engine_container.hpp"
#include "../../container/secret/secret_container.hpp"

#include "../canvas_factory.hpp"
#include "../menu_system.hpp"

#include "../../util/misc.hpp"
#include <wge/core/logging.hpp>

#define MAIN_PAGE 0

namespace Rhythmic
{
	WidgetEngineEditor::WidgetEngineEditor(EngineEditorData* data, int playerIndex) :
		WidgetBase(),
		m_frame(0, 0, RenderManager::GetSheet("ui_elements"), glm::vec4(1, 1, 1, 0.7f)),
		m_playerIndex(playerIndex),
		m_page(-10),
		//m_authorBox("Author", 0, false),
		m_editorEngine(data->engine),
		m_engineButtons(data->buttonField),
		m_buttonIndex(data->buttonIndex),
		m_isSaved(data->isSaved),
		m_isChangesSaved(true),
		m_nameBox("")
	{
		if (m_isSaved)
		{
			m_editorEngine = *EngineContainer::GetEngine(data->guid);
		}
		else m_editorEngine.m_isCustom = true;

		if (EngineContainer::GetEngine(data->guid) != 0)
		{
			m_nameBox.GetLabel().SetText(EngineContainer::GetEngine(data->guid)->m_name);
		}

		m_nameBox.SetScale(glm::vec2(0.2f, 0.0275f));
		m_nameBox.SetPosition(glm::vec2(0.75f, 0.8f));
		m_nameBox.SetMaxLength(16);

		//m_authorBox.SetScale(glm::vec2(0.2f, 0.0275f));
		//m_authorBox.SetPosition(glm::vec2(0.75f, 0.8f));
		//m_authorBox.SetMaxLength(16);

		m_frame.SetPosition(glm::vec2(0.75f, 0.5f));
		m_frame.SetScale(glm::vec2(0.2f, 0.35f));

		m_buttons.SetPosition(m_frame.GetPosition() - glm::vec2(0, m_frame.GetScale().y - 0.04f));
		m_buttons.SetScale((m_frame.GetScale() * glm::vec2(1, 1.88f)) - glm::vec2(0, 0.04f));
		m_buttons.SetFocusPlayer(0);
		m_buttons.SetAllowScroll(false);

		SetCanvasChangeCallback([this](Canvas *canvas) { m_buttons.SetCanvas(canvas); m_frame.SetCanvas(canvas); });
	}
	WidgetEngineEditor::~WidgetEngineEditor()
	{

	}

	void WidgetEngineEditor::OnCanvasActivated()
	{
		m_buttons.OnCanvasActivated();
	}

	void WidgetEngineEditor::OnMouseClick(int button, float x, float y)
	{
		m_buttons.OnMouseClick(button, x, y);
	}

	void WidgetEngineEditor::OnMouseMove(float x, float y)
	{
		m_buttons.OnMouseMove(x, y);
	}

	void WidgetEngineEditor::OnScroll(float dx, float dy)
	{
		m_buttons.OnScroll(dx, dy);
	}

	void WidgetEngineEditor::Update(float delta)
	{
		if (m_page == -10)
			ChangePage(0);
		if (m_page == -1)
			ChangePage(1);
		if (m_page == -2)
			ChangePage(2);
		if (m_page == -3)
			ChangePage(3);

		if (m_page == 0)
		{
			if (m_buttons.GetButtons()->size() > 3)
			{
				m_buttons.Get(2)->SetDisabled((!m_isChangesSaved || EngineContainer::GetCurrentEngine()->m_guid == m_editorEngine.m_guid));
			}
			if (!m_editorEngine.m_isCustom)
			{
				m_buttons.Get(m_buttons.GetButtons()->size() - 1)->SetDisabled(true);
			}
		}
		if (m_page == 1) // Guitar Page
		{
			if (!m_editorEngine.m_isCustom)
			{
				for (auto it = m_buttons.GetButtons()->begin(); it != m_buttons.GetButtons()->end(); it++)
				{
					WidgetButton* button = *it;
					if (!button->IsDisabled()) button->SetDisabled(true);
				}
			}
			if (m_buttons.Get(0) != 0) // Button exists
			{
				if (m_editorEngine.m_fret5.m_infiniteHOPOFrontEnd && !m_buttons.Get(0)->IsDisabled() || !m_editorEngine.m_isCustom)
				{
					m_buttons.Get(0)->SetDisabled(true);
				}
				else if (m_buttons.Get(0)->IsDisabled() && !m_editorEngine.m_fret5.m_infiniteHOPOFrontEnd && m_editorEngine.m_isCustom) m_buttons.Get(0)->SetDisabled(false);
			}
		}
		if (m_page == 2) // Drum page
		{
			if (!m_editorEngine.m_isCustom)
			{
				for (auto it = m_buttons.GetButtons()->begin(); it != m_buttons.GetButtons()->end(); it++)
				{
					WidgetButton* button = *it;
					if (!button->IsDisabled()) button->SetDisabled(true);
				}
			}
		}
		if (m_page == 3) // Keys page
		{
			if (!m_editorEngine.m_isCustom)
			{
				for (auto it = m_buttons.GetButtons()->begin(); it != m_buttons.GetButtons()->end(); it++)
				{
					WidgetButton* button = *it;
					if (!button->IsDisabled()) button->SetDisabled(true);
				}
			}
		}

		m_buttons.Update(delta);
	}
	void WidgetEngineEditor::Render(float interpolation)
	{
		if (m_nameBox.IsSelected())
		{
			if (!m_editorEngine.m_isCustom) m_nameBox.Deselect();
			else m_nameBox.Render(interpolation);
		}
		m_frame.Render(interpolation);
		m_buttons.Render(interpolation);
	}
	void WidgetEngineEditor::ProcessInput(InputEventData *input, int playerIndex)
	{
		if (input->m_buttonsPressed & (1 << INPUT_BUTTON_RED))
		{
			if (m_page != MAIN_PAGE)
			{
				ChangePage(0);
			}
			else
			{
				if (!m_isChangesSaved)
				{
					MenuSystem::GetInstance()->DisplayConfirmationBox([]()
						{
							MenuSystem::GetInstance()->QueuePopMenu();
						}, "You have unsaved changes. Are you sure you want to quit?");
				}
			}
			return;
		}
		m_buttons.ProcessInput(input, 0);
	}

	int WidgetEngineEditor::GetPage() { return m_page; }

	bool WidgetEngineEditor::IsChangesSaved() { return m_isChangesSaved; }

	void WidgetEngineEditor::ChangePage(int page)
	{
		if (m_page == page)
			return;
		m_buttons.ClearButtons();
		m_nameBox.Deselect();

		if (page == 0)
		{
			SetPageInstrumentSelector();
			m_nameBox.Select();
		}
		if (page == 1)
			SetPageGuitar();
		if (page == 2)
			SetPageDrums();
		if (page == 3)
			SetPageKeys();

		m_page = page;
	}

	void WidgetEngineEditor::SetPageInstrumentSelector()
	{
		m_buttons.AddButton(new WidgetButton("Guitar", [this]() { m_page = -1; return BUTTON_FINISHED; }, 0));
		m_buttons.AddButton(new WidgetButton("Drums", [this]() { m_page = -2; return BUTTON_FINISHED; }, 0));
		//m_buttons.AddButton(new WidgetButton("Keys", [this]() { m_page = -3; return BUTTON_FINISHED; }, 0));
		if (m_isSaved)
		{
			m_buttons.AddButton(new WidgetButton("Set Engine to Active Engine", [this]()
				{
					if (m_isChangesSaved)
					{
						EngineContainer::SetCurrentEngine(m_editorEngine);
					}
					return BUTTON_FINISHED;
				}, 0));
		}
		m_buttons.AddButton(new WidgetButton("Save Changes", [this]() {
			if (m_isSaved)
			{
				EngineContainer::DeleteEngineFile(m_editorEngine);
			}

			if(m_nameBox.GetLabel().GetText() != "" || m_nameBox.GetLabel().GetText() != m_editorEngine.m_name) m_editorEngine.m_name = m_nameBox.GetLabel().GetText();
			
			bool nameTaken = false;

			std::map<WGE::Util::GUID, Engine> *engines = EngineContainer::GetEngines();

			if (!m_isSaved)
			{
				WGE::Util::GUID guid = WGE::Util::GUID(true);
				m_editorEngine.m_guid = guid;

				Engine lambdEngine = m_editorEngine;
				WidgetButtonField* field = m_engineButtons;
				int index = m_buttonIndex + 1;

				m_engineButtons->AddButton(new WidgetButton(m_editorEngine.m_name, [lambdEngine, field, index]() {
					EngineEditorData data = { lambdEngine.m_guid, field, index, true, lambdEngine };

					MenuSystem::GetInstance()->QueuePushMenu(CanvasFactory::CreateCanvas(CANVAS_FACTORY_ENGINE_CREATOR_EDITOR, &data));
					return BUTTON_FINISHED;
				}, 0));

				EngineContainer::GetEngines()->insert(std::make_pair(guid, m_editorEngine));

				m_isSaved = true;
			}
			else {
				std::map<WGE::Util::GUID, Engine>* engines = EngineContainer::GetEngines();

				for (auto it = engines->begin(); it != engines->end(); it++)
				{
					if (it->first == m_editorEngine.m_guid) it->second = m_editorEngine;
				}
				m_engineButtons->Get(m_buttonIndex)->GetLabel().SetText(m_editorEngine.m_name);

				EngineContainer::DeleteEngineFile(m_editorEngine);
			}
			EngineContainer::SaveEngineToFile(&m_editorEngine);

			if (!m_isChangesSaved) m_isChangesSaved = true;
			return BUTTON_FINISHED;
		}, 0));
		if (m_isSaved && m_editorEngine.m_isCustom)
		{
			m_buttons.AddButton(new WidgetButton("Delete Engine", [this]()
				{
					EngineContainer::DeleteEngineFile(m_editorEngine);
					EngineContainer::GetEngines()->erase(m_editorEngine.m_guid);

					//ReinitializeButtons(m_engineButtons);

					MenuSystem::GetInstance()->QueuePopMenu();
					MenuSystem::GetInstance()->QueuePopMenu();
					MenuSystem::GetInstance()->QueuePushMenu(CanvasFactory::CreateCanvas(CANVAS_FACTORY_ENGINE_CREATOR));

					return BUTTON_FINISHED;
				}, 0));
		}
		/*m_buttons.AddButton(new WidgetButton("Delete Engine", [this]() {
			std::map<WGE::Util::GUID, Engine>* engines = EngineContainer::GetEngines();
			if (m_isSaved)
			{
				for (auto it = engines->begin(); it != engines->end(); it++)
				{
					if (it->first == m_editorEngine.m_guid)
					{
						std::string filename = boost::algorithm::to_lower_copy(std::string(m_editorEngine.m_name));

						for (std::string::iterator it = filename.begin(); it != filename.end(); ++it)
						{
							if (*it == ' ')
							{
								*it = '_';
							}
						}

						boost::filesystem::remove(Util::GetExecutablePath() + "/custom/engines/" + it->second.m_filename + ".engine");
						engines->erase(it->first);
						break;
					}
				}
			}
			ReinitialiseButtons();
			MenuSystem::GetInstance()->QueuePopMenu();
			return BUTTON_FINISHED;
		}, 0));*/
	}

	void WidgetEngineEditor::SetPageGuitar()
	{
		m_buttons.AddButton(new WidgetButton("HOPO Front End: ", [this]() { 
			if (!m_editorEngine.m_fret5.m_infiniteHOPOFrontEnd)
				return BUTTON_TOGGLE_SCROLL;
			return BUTTON_FINISHED;
		}, 0, [this](int i) {
			if (!m_editorEngine.m_fret5.m_infiniteHOPOFrontEnd)
			{
				m_editorEngine.m_fret5.m_hopoFrontEnd += i;
				m_isChangesSaved = false;

				if (m_editorEngine.m_fret5.m_hopoFrontEnd > 500 && !SecretContainer::IsSecretUnlocked(INFINITE_ENGINE)) m_editorEngine.m_fret5.m_hopoFrontEnd -= i;
				if (m_editorEngine.m_fret5.m_hopoFrontEnd < 0 && !SecretContainer::IsSecretUnlocked(NEGATIVE_ENGINE)) m_editorEngine.m_fret5.m_hopoFrontEnd -= i;
			}
			return BUTTON_FINISHED;
		}));
		m_buttons.AddButton(new WidgetButton("Inf. HOPO Front End: ", [this]() {
			m_editorEngine.m_fret5.m_infiniteHOPOFrontEnd = !m_editorEngine.m_fret5.m_infiniteHOPOFrontEnd;
			m_isChangesSaved = false;

			return BUTTON_FINISHED;
		}, 0));
		m_buttons.AddButton(new WidgetButton("HOPO Tap Range: ", [this]() { return BUTTON_TOGGLE_SCROLL; }, 0, [this](int i) {
			m_editorEngine.m_fret5.m_hopoTapRange += i;
			m_isChangesSaved = false;

			if (m_editorEngine.m_fret5.m_hopoTapRange > 500 && !SecretContainer::IsSecretUnlocked(INFINITE_ENGINE)) m_editorEngine.m_fret5.m_hopoTapRange -= i;
			if (m_editorEngine.m_fret5.m_hopoTapRange < 0 && !SecretContainer::IsSecretUnlocked(NEGATIVE_ENGINE)) m_editorEngine.m_fret5.m_hopoTapRange -= i;
			return BUTTON_FINISHED;
		}));
		m_buttons.AddButton(new WidgetButton("Strum Range: ", [this]() { return BUTTON_TOGGLE_SCROLL; }, 0, [this](int i) {
			m_editorEngine.m_fret5.m_strumRange += i;
			m_isChangesSaved = false;

			if (m_editorEngine.m_fret5.m_strumRange > 500 && !SecretContainer::IsSecretUnlocked(INFINITE_ENGINE)) m_editorEngine.m_fret5.m_strumRange -= i;
			if (m_editorEngine.m_fret5.m_strumRange < 0 && !SecretContainer::IsSecretUnlocked(NEGATIVE_ENGINE)) m_editorEngine.m_fret5.m_strumRange -= i;
			return BUTTON_FINISHED;
		}));
		m_buttons.AddButton(new WidgetButton("Strum Leniency: ", [this]() { return BUTTON_TOGGLE_SCROLL; }, 0, [this](int i) {
			m_editorEngine.m_fret5.m_strumLeniency += i;
			m_isChangesSaved = false;

			if (m_editorEngine.m_fret5.m_strumLeniency > 500 && !SecretContainer::IsSecretUnlocked(INFINITE_ENGINE)) m_editorEngine.m_fret5.m_strumLeniency -= i;
			if (m_editorEngine.m_fret5.m_strumLeniency < 0 && !SecretContainer::IsSecretUnlocked(NEGATIVE_ENGINE)) m_editorEngine.m_fret5.m_strumLeniency -= i;
			return BUTTON_FINISHED;
		}));
		m_buttons.AddButton(new WidgetButton("HOPO Leniency: ", [this]() { return BUTTON_TOGGLE_SCROLL; }, 0, [this](int i) {
			m_editorEngine.m_fret5.m_hopoLeniency += i;
			m_isChangesSaved = false;

			if (m_editorEngine.m_fret5.m_hopoLeniency > 500 && !SecretContainer::IsSecretUnlocked(INFINITE_ENGINE)) m_editorEngine.m_fret5.m_hopoLeniency -= i;
			if (m_editorEngine.m_fret5.m_hopoLeniency < 0 && !SecretContainer::IsSecretUnlocked(NEGATIVE_ENGINE)) m_editorEngine.m_fret5.m_hopoLeniency -= i;
			return BUTTON_FINISHED;
		}));
		m_buttons.AddButton(new WidgetButton("Tap Overstrum Leniency: ", [this]() { return BUTTON_TOGGLE_SCROLL; }, 0, [this](int i) {
			m_editorEngine.m_fret5.m_timeTapIgnoreOverstrum += i;
			m_isChangesSaved = false;

			if (m_editorEngine.m_fret5.m_timeTapIgnoreOverstrum > 500 && !SecretContainer::IsSecretUnlocked(INFINITE_ENGINE)) m_editorEngine.m_fret5.m_timeTapIgnoreOverstrum -= i;
			if (m_editorEngine.m_fret5.m_timeTapIgnoreOverstrum < 0 && !SecretContainer::IsSecretUnlocked(NEGATIVE_ENGINE)) m_editorEngine.m_fret5.m_timeTapIgnoreOverstrum -= i;
			return BUTTON_FINISHED;
		}));
		m_buttons.AddButton(new WidgetButton("Anti Ghosting: ", [this]() {
			m_editorEngine.m_fret5.m_antiGhost = !m_editorEngine.m_fret5.m_antiGhost;
			m_isChangesSaved = false;

			return BUTTON_FINISHED;
		}, 0));

		m_buttons.Get(0)->GetLabel().TagIntAtEnd(&m_editorEngine.m_fret5.m_hopoFrontEnd);
		m_buttons.Get(1)->GetLabel().TagBoolAtEnd(&m_editorEngine.m_fret5.m_infiniteHOPOFrontEnd);
		m_buttons.Get(2)->GetLabel().TagIntAtEnd(&m_editorEngine.m_fret5.m_hopoTapRange);
		m_buttons.Get(3)->GetLabel().TagIntAtEnd(&m_editorEngine.m_fret5.m_strumRange);
		m_buttons.Get(4)->GetLabel().TagIntAtEnd(&m_editorEngine.m_fret5.m_strumLeniency);
		m_buttons.Get(5)->GetLabel().TagIntAtEnd(&m_editorEngine.m_fret5.m_hopoLeniency);
		m_buttons.Get(6)->GetLabel().TagIntAtEnd(&m_editorEngine.m_fret5.m_timeTapIgnoreOverstrum);
		m_buttons.Get(7)->GetLabel().TagBoolAtEnd(&m_editorEngine.m_fret5.m_antiGhost);
		
	}

	void WidgetEngineEditor::SetPageDrums()
	{
		m_buttons.AddButton(new WidgetButton("Window Range: ", [this]() { return BUTTON_TOGGLE_SCROLL; }, 0, [this](int i) {
			m_editorEngine.m_drums.m_rangeCatcher += i;
			m_isChangesSaved = false;

			if (m_editorEngine.m_drums.m_rangeCatcher > 500 && !SecretContainer::IsSecretUnlocked(INFINITE_ENGINE)) m_editorEngine.m_drums.m_rangeCatcher -= i;
			if (m_editorEngine.m_drums.m_rangeCatcher < 0 && !SecretContainer::IsSecretUnlocked(NEGATIVE_ENGINE)) m_editorEngine.m_drums.m_rangeCatcher -= i;
			return BUTTON_FINISHED;
		}));
		m_buttons.AddButton(new WidgetButton("Chord Hit Leniency: ", [this]() { return BUTTON_TOGGLE_SCROLL; }, 0, [this](int i) {
			m_editorEngine.m_drums.m_chordTime += i;
			m_isChangesSaved = false;

			if (m_editorEngine.m_drums.m_chordTime > 500 && !SecretContainer::IsSecretUnlocked(INFINITE_ENGINE)) m_editorEngine.m_drums.m_chordTime -= i;
			if (m_editorEngine.m_drums.m_chordTime < 0 && !SecretContainer::IsSecretUnlocked(NEGATIVE_ENGINE)) m_editorEngine.m_drums.m_chordTime -= i;
			return BUTTON_FINISHED;
		}));
		m_buttons.AddButton(new WidgetButton("SP Activation Timer: ", [this]() { return BUTTON_TOGGLE_SCROLL; }, 0, [this](int i) {
			m_editorEngine.m_drums.m_spActivationTime += i;
			m_isChangesSaved = false;

			if (m_editorEngine.m_drums.m_spActivationTime > 500 && !SecretContainer::IsSecretUnlocked(INFINITE_ENGINE)) m_editorEngine.m_drums.m_spActivationTime -= i;
			if (m_editorEngine.m_drums.m_spActivationTime < 0 && !SecretContainer::IsSecretUnlocked(NEGATIVE_ENGINE)) m_editorEngine.m_drums.m_spActivationTime -= i;
			return BUTTON_FINISHED;
		}));
		m_buttons.AddButton(new WidgetButton("Pad Activation Time (Visual): ", [this]() { return BUTTON_TOGGLE_SCROLL; }, 0, [this](int i) {
			m_editorEngine.m_drums.m_padActive += i;
			m_isChangesSaved = false;

			if (m_editorEngine.m_drums.m_padActive > 500 && !SecretContainer::IsSecretUnlocked(INFINITE_ENGINE)) m_editorEngine.m_drums.m_padActive -= i;
			if (m_editorEngine.m_drums.m_padActive < 0 && !SecretContainer::IsSecretUnlocked(NEGATIVE_ENGINE)) m_editorEngine.m_drums.m_padActive -= i;
			return BUTTON_FINISHED;
		}));

		m_buttons.Get(0)->GetLabel().TagIntAtEnd(&m_editorEngine.m_drums.m_rangeCatcher);
		m_buttons.Get(1)->GetLabel().TagIntAtEnd(&m_editorEngine.m_drums.m_chordTime);
		m_buttons.Get(2)->GetLabel().TagIntAtEnd(&m_editorEngine.m_drums.m_spActivationTime);
		m_buttons.Get(3)->GetLabel().TagIntAtEnd(&m_editorEngine.m_drums.m_padActive);
	}

	void WidgetEngineEditor::SetPageKeys() 
	{

	}

	void WidgetEngineEditor::ReinitializeButtons(WidgetButtonField *buttons)
	{
		m_engineButtons->ClearButtons();

		m_engineButtons->AddButton(new WidgetButton("Create New Engine", [buttons]
			{
				EngineEditorData data;

				Engine engine;
				data.engine = engine;
				data.guid = WGE::Util::GUID();
				data.buttonField = buttons;
				data.buttonIndex = buttons->GetButtons()->size() - 1;
				data.isSaved = false;

				// Queue the push with the data
				MenuSystem::GetInstance()->QueuePushMenu(CanvasFactory::CreateCanvas(CANVAS_FACTORY_ENGINE_CREATOR_EDITOR, &data));
				return BUTTON_FINISHED;
			}, 0));

		std::map<WGE::Util::GUID, Engine>* engines = EngineContainer::GetEngines();
		std::vector<Engine> engineSort;

		for (auto swag = engines->begin(); swag != engines->end(); swag++) // Create built-in Engines buttons so they are viewable but not editable.
		{
			WGE::Util::GUID guid = swag->first;
			Engine engine = swag->second;

			if (engine.m_isCustom) continue;

			int size = m_engineButtons->GetButtons()->size();

			m_engineButtons->AddButton(new WidgetButton(engine.m_name, [this, guid, size, engine]
				{
					EngineEditorData data = { guid, m_engineButtons, size, true, engine };
					MenuSystem::GetInstance()->QueuePushMenu(CanvasFactory::CreateCanvas(CANVAS_FACTORY_ENGINE_CREATOR_EDITOR, &data));
					return BUTTON_FINISHED;
				}, 0));
		}

		// Begin engine sorting by name

		for (auto it = engines->begin(); it != engines->end(); it++)
		{
			engineSort.push_back(it->second);
		}

		std::sort(engineSort.begin(), engineSort.end(), [](Engine first, Engine second)
			{
				return first.m_name.compare(second.m_name);
			});

		engines->clear();

		for (int i = 0; i < engineSort.size(); i++)
		{
			engines->insert(std::make_pair(engineSort[i].m_guid, engineSort[i]));
		}

		// End of engine sorting

		// Add custom engine buttons
		for (auto swag = engines->begin(); swag != engines->end(); swag++)
		{
			WGE::Util::GUID guid = swag->first;
			Engine engine = swag->second;

			if (!engine.m_isCustom) continue;

			int size = m_engineButtons->GetButtons()->size();

			m_engineButtons->AddButton(new WidgetButton(engine.m_name, [this, guid, size, engine]
				{
					EngineEditorData data = { guid, m_engineButtons, size, true, engine };
					MenuSystem::GetInstance()->QueuePushMenu(CanvasFactory::CreateCanvas(CANVAS_FACTORY_ENGINE_CREATOR_EDITOR, &data));
					return BUTTON_FINISHED;
				}, 0));
		}
	}
}