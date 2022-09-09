#include "io.hpp"
#include <wge/core/logging.hpp>

#include <iostream>
#include <vector>

#include "../container/player_container.hpp"
#include "../container/binding_container.hpp"

#include "../util/misc.hpp"

#include <stdint.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace Rhythmic
{
	bool		g_requestClose;

	bool		g_windowLostFocus;

	int			g_mouseX;
	int			g_mouseY;
	bool		g_mouseDown[2];
	bool		g_mousePressed[2];
	bool		g_mouseDoubleClicked;

	TextFieldButtons g_textFieldButtons;

	float		g_lastMouseDownTime;

	std::vector<DisplayMode> g_validDisplayModes;

	void IO_cb_glfwError(int code, const char *error)
	{
		LOG_CRITICAL("GLFW Error: ({0}: {1})", code, error);
	}


	std::function<void(unsigned int)> g_fieldListener;

	void IO_Char_Callback(unsigned int c)
	{
		TextData data;
		data.c = c;

		memset(&data.buttons, 0, sizeof(data.buttons));

		EventDispatch(ET_TEXT, &data);
	}

	void IO_Key_Callback(int key, int scancode, int action, int mods)
	{
		bool changed = false;

		if (key == GLFW_KEY_BACKSPACE)
		{
			g_textFieldButtons.pressedBackspace = action == GLFW_PRESS;
			changed = true;
		}
		if (key == GLFW_KEY_ENTER)
		{
			g_textFieldButtons.pressedEnter = action == GLFW_PRESS;
			changed = true;
		}
		if (key == GLFW_KEY_ESCAPE)
		{
			g_textFieldButtons.pressedEscape = action == GLFW_PRESS;
			changed = true;
		}
		if (key == GLFW_KEY_LEFT)
		{
			g_textFieldButtons.pressedLeft = action == GLFW_PRESS;
			changed = true;
		}
		if (key == GLFW_KEY_RIGHT)
		{
			g_textFieldButtons.pressedRight = action == GLFW_PRESS;
			changed = true;
		}
		if (key == GLFW_KEY_DELETE)
		{
			g_textFieldButtons.pressedDelete = action == GLFW_PRESS;
			changed = true;
		}
		if (key == GLFW_KEY_LEFT_CONTROL || key == GLFW_KEY_RIGHT_CONTROL)
		{
			g_textFieldButtons.downModCtrl = action == GLFW_PRESS;
			changed = true;
		}
		if (key == GLFW_KEY_V)
		{
			g_textFieldButtons.paste = action == GLFW_PRESS;
			changed = true;
		}


		if (changed)
		{
			TextData data;
			data.c = 0;
			memcpy(&data.buttons, &g_textFieldButtons, sizeof(data.buttons));

			EventDispatch(ET_TEXT, &data);
		}

	}
	
	bool IO_Systems_Initialize()
	{
		g_requestClose = false;

		glfwInitHint(GLFW_JOYSTICK_HAT_BUTTONS, GLFW_TRUE);

		if (!glfwInit())
			return true;
		
		g_mouseX = 0;
		g_mouseY = 0;

		g_windowLostFocus = false;

		g_lastMouseDownTime = Util::GetTime();
		g_mouseDoubleClicked = false;

		for (int i = 0; i < 2; i++)
		{
			g_mouseDown[i] = false;
			g_mousePressed[i] = false;
		}

		return false;
	}

	void IO_Systems_Destroy()
	{
		glfwTerminate();
	}

	void IO_Systems_Update(bool windowOnly)
	{
		g_mouseDoubleClicked = false;
		g_mousePressed[0] = false;
		g_mousePressed[1] = false;
		g_windowLostFocus = false;

		g_textFieldButtons.pressedBackspace = g_textFieldButtons.pressedDelete =
			g_textFieldButtons.pressedEnter = g_textFieldButtons.pressedEscape =
			g_textFieldButtons.pressedLeft = g_textFieldButtons.pressedRight =
			g_textFieldButtons.paste = false;

		PlayerContainer::UpdateAllControllers();

		glfwPollEvents();
		g_requestClose = glfwWindowShouldClose((GLFWwindow*)IO_Window_Handle());
	}

	bool IO_Systems_RequestedClose()
	{
		return g_requestClose;
	}

	glm::vec2 IO_Systems_GetMouseCoords()
	{
		int hw = IO_Window_GetWidth() / 2;
		int hh = IO_Window_GetHeight() / 2;
		return glm::vec2(g_mouseX - hw, g_mouseY - hh);
	}

	bool IO_Systems_MousePressed(int button)
	{
		return g_mousePressed[button];
	}
	bool IO_Systems_MouseDoubleClicked()
	{
		return g_mouseDoubleClicked;
	}

	const std::vector<DisplayMode> &IO_Systems_GetValidDisplayModes() { return g_validDisplayModes; }
	bool IO_Systems_IsDisplayModeValid(const DisplayMode &mode)
	{
		for (auto it = g_validDisplayModes.begin(); it != g_validDisplayModes.end(); it++)
		{
			if (it->width == mode.width && it->height == mode.height)
				return true;
		}
		return false;
	}

	TextFieldButtons IO_Systems_TextFieldButtons() { return g_textFieldButtons; }

	bool IO_Window_LostFocus()
	{
		return g_windowLostFocus;
	}

	IO_Clipboard::IO_Clipboard()
	{
		m_data = glfwGetClipboardString((GLFWwindow*)IO_Window_Handle());
	}
	IO_Clipboard::~IO_Clipboard()
	{
		// GLFW handles this
	}
	IO_Clipboard::operator const char*()
	{
		return m_data;
	}
}