#ifndef _IO_COMMON_H_
#define _IO_COMMON_H_

#include "io_window.hpp"

#include <functional>

#include <map>

namespace Rhythmic
{
	struct TextFieldButtons
	{
		bool pressedEscape;
		bool pressedEnter;

		bool pressedLeft;
		bool pressedRight;
		
		bool pressedBackspace;
		bool pressedDelete;

		bool downModCtrl;

		bool paste;
		bool copy;
		bool cut;

		/*bool operator==(const TextFieldButtons &buttons) const
		{
			return (pressedEscape == buttons.pressedEscape) &&
				   (pressedEnter == buttons.pressedEnter) &&
				   (pressedLeft == buttons.pressedLeft) &&
				   (pressedRight == buttons.pressedRight) &&
				   (pressedBackspace == buttons.pressedBackspace) &&
				   (pressedDelete == buttons.pressedDelete) &&
				   (downModCtrl == buttons.downModCtrl) &&
				   (paste == buttons.paste) &&
				   (copy == buttons.copy) &&
				   (cut == buttons.cut);
		}
		bool operator!=(const TextFieldButtons &buttons) const
		{
			return !(*this == buttons);
		}*/
	};

	struct TextData
	{
		TextFieldButtons buttons;
		unsigned int c;
	};

	void IO_Char_Callback(unsigned int c);
	void IO_Key_Callback(int key, int scancode, int action, int mods);

	/*
	Initializes libraries for IO
	*/
	bool IO_Systems_Initialize();

	/*
	Terminates libraries for IO
	*/
	void IO_Systems_Destroy();

	/*
	Updates everything in the IO System
	*/
	void IO_Systems_Update(bool windowOnly = false);

	/*
	Will return true if the IO systems recieve any close input from the window
	*/
	bool IO_Systems_RequestedClose();

	/*
	Returns the mouse coordinates corrected for the viewing matrix, where (0,0) is the center of the window
	*/
	glm::vec2 IO_Systems_GetMouseCoords();

	/*
	Returns true if the mouse is down for one frame
	button = 0 Left Button
	button = 1 Right Button
	*/
	bool IO_Systems_MousePressed(int button);
	bool IO_Systems_MouseDoubleClicked();

	const std::vector<DisplayMode> &IO_Systems_GetValidDisplayModes();
	bool IO_Systems_IsDisplayModeValid(const DisplayMode &mode);

	TextFieldButtons IO_Systems_TextFieldButtons();

	bool IO_Window_LostFocus();

	/*
	A class that retrieves the Operating Systems clipboard
	RAII
	*/
	class IO_Clipboard
	{
	public:
		IO_Clipboard();
		~IO_Clipboard();
		operator const char*();
	private:
		const char *m_data;
	};
} // namespace Rhythmic

#endif //_IO_COMMON_H_
