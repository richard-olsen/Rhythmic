#include "menu_system.hpp"

#include "types/dialog/canvas_message.hpp"
#include "types/dialog/canvas_confirmation_box.hpp"
#include "../input/input.hpp"
#include "../input/input_handlers.hpp"
#include "../container/player_container.hpp"
#include "../io/io.hpp"

namespace Rhythmic
{
	MenuSystem::MenuSystem() :
		m_popMenuThisUpdate(false),
		m_pushMenuThisUpdate(false),
		m_queueCanvas(0),
		m_lastMouseX(0),
		m_lastMouseY(0),
		m_ignoreInputQueues(false),
		m_queue()
	{

	}

	void MenuSystem::PushMenu(Canvas *canvas)
	{
		if (!canvas)
			return;
		size_t arrayLength = m_listCanvas.size();
		if (arrayLength > 0)
			m_listCanvas[arrayLength - 1]->SetActive(false);
		m_listCanvas.push_back(canvas);
		canvas->SetActive(true);
		canvas->OnMouseMove(m_lastMouseX, m_lastMouseY);
	}
	void MenuSystem::PopMenu()
	{
		size_t arrayLength = m_listCanvas.size();
		if (arrayLength > 0)
		{
			delete m_listCanvas[arrayLength - 1];
			m_listCanvas.pop_back();
			arrayLength--;

			if (arrayLength > 0)
			{
				m_listCanvas.back()->SetActive(true);
				m_listCanvas.back()->OnMouseMove(m_lastMouseX, m_lastMouseY);
			}
		}
	}
	void MenuSystem::QueuePushMenu(Canvas *canvas)
	{
		_MenuSystemQueueJob job;
		job.menu = canvas;
		job.type = 1; // Job type 1 is to push the menu
		m_queue.push(job);
	}
	void MenuSystem::QueuePopMenu()
	{
		_MenuSystemQueueJob job;
		job.menu = 0;
		job.type = 0; // Job type 0 is to pop the menu
		m_queue.push(job);
	}

	void MenuSystem::PopAllMenus()
	{
		for (int i = 0; i < m_listCanvas.size(); i++)
			delete m_listCanvas[i];
		m_listCanvas.clear();
	}

	void MenuSystem::DisplayMessage(const std::string &string)
	{
		Canvas *canvas = new Canvas();
		CanvasCreateMessage(canvas, string);
		QueuePushMenu(canvas);
	}

	void MenuSystem::DisplayConfirmationBox(std::function<void()> onClick, const std::string& string)
	{
		Canvas* canvas = new Canvas();
		CanvasCreateConfirmation(canvas, onClick, string);
		QueuePushMenu(canvas);
	}

	void MenuSystem::ShouldIgnoreInputQueues(bool should)
	{
		m_ignoreInputQueues = should;
	}

	void MenuSystem::Update(float delta)
	{
		while (!m_queue.empty())
		{
			_MenuSystemQueueJob &job = m_queue.front();
			if (job.type == 0)
				PopMenu();
			if (job.type == 1)
				PushMenu(job.menu);
			m_queue.pop();
		}


		if (m_listCanvas.empty())
			return;

		m_listCanvas.back()->Update(delta);
	}
	void MenuSystem::DrawTopCanvas(float interpolation)
	{
		size_t arrayLength = m_listCanvas.size();
		if (arrayLength > 0)
		{
			auto widgets = m_listCanvas[arrayLength - 1]->GetWidgets();

			for (auto i = widgets.begin(); i != widgets.end(); i++)
			{
				i->second->Render(interpolation);
			}
		}
	}
	Canvas *MenuSystem::GetTopCanvas()
	{
		return QueryCanvas(0);
	}

	Canvas *MenuSystem::QueryCanvas(int back)
	{
		size_t arrayLength = m_listCanvas.size();

		assert(back < arrayLength);

		if (arrayLength == 0)
			return 0;
		return m_listCanvas[arrayLength - 1 - back];
	}
	InputDevice *MenuSystem::CurrentInputDevice()
	{
		return m_currentDevice;
	}

	void MenuSystem::_OnInput(void *data)
	{
		if (m_listCanvas.empty())
			return;

		InputEventData *input = (InputEventData*)data;
		Canvas *canvas = m_listCanvas.back();
		
		canvas->ProcessInput(input, PlayerContainer::GetPlayerIndexByInputDevice(input->device));
	}

	void MenuSystem::InitializeListeners()
	{
		m_listenerMouseMove = EventAddListener(ET_MOUSE_MOVE, CREATE_MEMBER_CB(MenuSystem::_OnMouseMove));
		m_listenerMouseClick = EventAddListener(ET_MOUSE_DOWN, CREATE_MEMBER_CB(MenuSystem::_OnMouseClick));
		m_listenerScroll = EventAddListener(ET_MOUSE_SCROLL, CREATE_MEMBER_CB(MenuSystem::_OnScroll));
		m_listenerInput = EventAddListener(ET_INPUT, CREATE_MEMBER_CB(MenuSystem::_OnInput));
	}
	void MenuSystem::DestroyListeners()
	{
		EventRemoveListener(m_listenerMouseMove);
		EventRemoveListener(m_listenerMouseClick);
		EventRemoveListener(m_listenerScroll);
		EventRemoveListener(m_listenerInput);
	}

	void MenuSystem::_OnMouseMove(void *raw)
	{
		if (m_listCanvas.empty())
			return;

		MouseMoveData *mData = (MouseMoveData*)raw;

		m_lastMouseX = mData->identX;
		m_lastMouseY = mData->identY;

		m_listCanvas.back()->OnMouseMove(mData->identX, mData->identY);
	}
	void MenuSystem::_OnMouseClick(void *raw)
	{
		if (m_listCanvas.empty())
			return;

		MouseDownData *mData = (MouseDownData*)raw;

		if (mData->state == 1)
			m_listCanvas.back()->OnMouseClick(mData->button, m_lastMouseX, m_lastMouseY);
	}
	void MenuSystem::_OnScroll(void *raw)
	{
		if (m_listCanvas.empty())
			return;

		MouseScrollData *mData = (MouseScrollData*)raw;
		m_listCanvas.back()->OnScroll(mData->scrollX, mData->scrollY);
	}
}