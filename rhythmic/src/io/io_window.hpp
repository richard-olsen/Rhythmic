#ifndef _IO_WINDOW_H_
#define _IO_WINDOW_H_

#include <glm/mat4x4.hpp>

#include <vector>

namespace Rhythmic
{
	struct MouseMoveData
	{
		float x; // Ranges from 0 to window width
		float y; // Ranges from 0 to window height

		float identX; // Ranges from -1 to 1
		float identY; // Ranges from -1 to 1
	};

	struct MouseDownData
	{
		int button; // Mouse button (0 - left click, 1 - right click)
		int state;  // The state of the button (1 - down, 0 - released)
	};

	struct MouseScrollData
	{
		float scrollX;
		float scrollY;
	};

	struct DisplayMode
	{
		int width;
		int height;
		std::vector<int> refresh_rates;
	};
	void *IO_Window_Handle();

	/*
	Creates the main game window. Returns true if window creation failed!
	*/
	bool IO_Window_Initialize();

	/*
	Destroys the main game window.
	*/
	void IO_Window_Destroy();
	/*
	Swaps the back buffer and front buffer.
	*/
	void IO_Window_SwapBuffers();

	/*
	Refreshes window variables to fit with settings
	*/
	void IO_Window_RefreshVars();

	/*
	Resets the window's size, and changes to fullscreen if requested
	0 = windowed
	1 = true fullscreen
	2 = borderless window fullscreen
	*/
	void IO_Window_SetDisplayMode(const DisplayMode &mode, int fullscreen, int refresh_rate_index);

	/*
	Updates the projection
	*/
	void IO_Window_UpdateProjection();

	/*
	Returns width
	*/
	int IO_Window_GetWidth();
	/*
	Returns height
	*/
	int IO_Window_GetHeight();

	void IO_Window_SetCallbacks();

	/*
	Returns a copy of the projection used to fit the window
	*/
	const glm::mat4 &IO_Window_GetProjection();

	const float &IO_Window_GetProjLeft();
	const float &IO_Window_GetProjRight();
	const float &IO_Window_GetProjUp();
	const float &IO_Window_GetProjDown();

	void IO_Window_SetDearImGui();
	void IO_Window_DearImGui_NewFrame();

	/*
	A temporary hack, remove when viable

	Handles the key input that comes from the Input System
	*/
	void IO_Window_HandleKey(int key, int scancode, int action, int mods);
} // namespace Rhythmic

#endif //_IO_WINDOW_H_