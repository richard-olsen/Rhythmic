#ifndef CANVAS_FACTORY_SECTION_SELECTION_HPP_
#define CANVAS_FACTORY_SECTION_SELECTION_HPP_

#include "../canvas.hpp"

#include "../widget.hpp"

#include "../widgets/text.hpp"
#include "../widgets/tile.hpp"
#include "../widgets/button_field.hpp"
#include "../../chart/chart.hpp"

#include "../../rendering/render_manager.hpp"
#include "../../game/stage/stage.hpp"

#include "../../input/input_game.hpp"

namespace Rhythmic
{
	class WidgetPracticeSectionSetter : public WidgetBase
	{
	public:
		WidgetPracticeSectionSetter(WidgetButtonField *buttonField) :
			WidgetBase(),
			m_state(0),
			m_buttonField(buttonField)
		{
			
		}


		virtual void Update(float delta)
		{
			if (m_state < 0)
				MenuSystem::GetInstance()->QueuePopMenu();

			if (m_state > 0)
			{
				int button = StageSystem::GetInstance()->GetModulePractice()->GetTargetButton();
				if (m_buttonField->CurrentlySelected() < button)
					m_buttonField->Select(button);
			}
		}
		virtual void Render(float interpolation)
		{ }
		virtual void ProcessInput(InputEventData *input, int playerIndex)
		{
			if (playerIndex == 0 && input->m_buttonsPressed & ButtonToFlag(INPUT_BUTTON_RED))
			{
				m_state--;
			}
		}
		void SetTargetButtonIfAble(int button)
		{
			if (m_state == 0)
				StageSystem::GetInstance()->GetModulePractice()->SetTargetButton(button);
		}
		void PushSection(const ChartSection &section)
		{
			if (m_state == 0)
				m_timeStart = section.position;
			if (m_state == 1)
			{
				if (section.endPosition <= m_timeStart) 
					return;

				m_timeEnd = section.endPosition;
			}

			m_state++;

			if (m_state >= 2) // Go into game
			{
				StageSystem::GetInstance()->GetModulePractice()->SetTimeAOffset(0);
				StageSystem::GetInstance()->GetModulePractice()->SetTimeBOffset(0);

				StageSystem *stageSystem = StageSystem::GetInstance();
				PracticeModule *practiceModule = stageSystem->GetModulePractice();

				practiceModule->SetTime(m_timeStart, m_timeEnd);
				practiceModule->PrepareStage();

				stageSystem->Reset();

				stageSystem->SetPause(false);

				MenuSystem::GetInstance()->QueuePopMenu();

				// if (PracticeManager::GetInstance()->GetTimeState() >= 2)
						// {
						// 	StageSystem *stageSystem = StageSystem::GetInstance();
						// 	stageSystem->SetSongTime(practiceManager->GetTimeStart() - (1.5f * stageSystem->GetPlaybackSpeed()));
						// 	//stageSystem->SetSongTime(0);
						// 	MenuSystem::GetInstance()->QueuePopMenu();
						// 	MenuSystem::GetInstance()->QueuePushMenu(CanvasFactory::CreateCanvas(CANVAS_FACTORY_IN_GAME_HUD));
						// 	stageSystem->SetPause(false);
						// }
			}
		}
	private:
		int m_timeStart;
		int m_timeEnd;

		int m_state;

		WidgetButtonField *m_buttonField;
	};

	namespace FactoryCanvas
	{
		void CanvasCreateSectionSelection(Canvas *canvas, void *canvasData)
		{
			Chart *chart = StageSystem::GetInstance()->GetChart(); // Need to get the chart

			WidgetTile *tile = new WidgetTile(0, 0, RenderManager::GetSheet("ui_elements"), glm::vec4(0, 0, 0, 0.6f));
			canvas->Add("p4", tile);

			WidgetText *text = new WidgetText("Select a section", RenderManager::GetFontRef("menus"), 1, glm::vec4(1, 1, 1, 1));
			text->SetPosition(glm::vec2(0.5f, 0.1f));
			text->SetScale(glm::vec2(0.03f, 0.03f));
			canvas->Add("p6", text);

			WidgetButtonField *buttonField = new WidgetButtonField();
			buttonField->SetPosition(glm::vec2(0.5f, 0.3f));
			buttonField->SetScale(glm::vec2(0.3f, 0.5f));

			WidgetPracticeSectionSetter *sectionSetter = new WidgetPracticeSectionSetter(buttonField);
			canvas->Add("pss", sectionSetter);

			// Dont see the point in this just use quickplay??
			/*buttonField->AddButton(new WidgetButton("Full Song", []() {
					StageSystem *system = StageSystem::GetInstance();
					system->GetModulePractice()->SetTimeToWholeSong();
					system->GetModulePractice()->PrepareStage();
					MenuSystem::GetInstance()->QueuePopMenu();
					system->SetPause(false);
					return BUTTON_FINISHED;
				}));*/

			for (int i = 0; i < chart->listSection.size(); i++)
			{
				ChartSection section = chart->listSection[i];
				buttonField->AddButton(new WidgetButton(section.name, [section, sectionSetter, i]() {
						sectionSetter->SetTargetButtonIfAble(i);
						sectionSetter->PushSection(section);
						return BUTTON_FINISHED;
					}));
			}

			buttonField->Select(StageSystem::GetInstance()->GetModulePractice()->GetTargetButton());

			canvas->Add("p2", buttonField);
		}
	}
}

#endif

