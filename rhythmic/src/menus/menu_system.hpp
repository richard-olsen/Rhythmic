#ifndef RHYTHMIC_MENU_SYSTEM_HPP_
#define RHYTHMIC_MENU_SYSTEM_HPP_

#include "canvas.hpp"
#include "../util/singleton.hpp"
#include <vector>
#include <string>

#include "widgets/button.hpp"

#include "../input/input.hpp"

namespace Rhythmic
{
	struct _MenuSystemQueueJob
	{
		int type;
		Canvas *menu;
	};
	class MenuSystem : public Singleton<MenuSystem>
	{
		friend class Singleton<MenuSystem>;
	public:
		void QueuePushMenu(Canvas *canvas); // Queues a canvas to be pushed on canvas stack
		void QueuePopMenu();								// Queues a job to pop the canvas off the canvas stack

		void PopAllMenus();

		Canvas *GetTopCanvas();
		Canvas *QueryCanvas(int back);		// Traverses back through the canvas stack (0 being the top, 1 being the next one behind)

		InputDevice *CurrentInputDevice();

		void _OnInput(void *);

		void Update(float delta);
		void DrawTopCanvas(float interpolation);

		void DisplayMessage(const std::string &string);
		void DisplayConfirmationBox(std::function<void()> onClick, const std::string& string);

		void ShouldIgnoreInputQueues(bool should);

		void InitializeListeners();
		void DestroyListeners();

		void _OnMouseMove(void *);
		void _OnMouseClick(void *);
		void _OnScroll(void *);
	private:
		MenuSystem();

		void PushMenu(Canvas *canvas);
		void PopMenu();

		float m_lastMouseX;
		float m_lastMouseY;

		bool m_ignoreInputQueues;

		Canvas *m_queueCanvas;
		std::vector<Canvas*> m_listCanvas;

		EventListener m_listenerInput;
		EventListener m_listenerMouseMove;
		EventListener m_listenerMouseClick;
		EventListener m_listenerScroll;

		InputDevice *m_currentDevice;

		std::queue<_MenuSystemQueueJob> m_queue;

		bool m_popMenuThisUpdate;
		bool m_pushMenuThisUpdate;
	};
}

#endif

