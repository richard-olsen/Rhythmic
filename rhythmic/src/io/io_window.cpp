#include "io_window.hpp"

#include "io.hpp"

#include <wge/core/logging.hpp>

#include "../util/settings.hpp"

#include "../util/dear_imgui/imgui.h"

#include "../events/event_system.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#ifdef WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#endif

#include <glm/gtc/matrix_transform.hpp>

#include "../rendering/render_manager.hpp"

extern unsigned int g_iconWidth;
extern unsigned int g_iconHeight;
extern unsigned char g_iconData[64 * 64 * 4];

namespace Rhythmic
{
	GLFWwindow		*g_window;
	glm::mat4		g_projection;
	int				g_width;
	int				g_height;
	int				g_refreshRate;
	float			g_projLeft;
	float			g_projRight;
	float			g_projUp;
	float			g_projDown;

	void ImGuiKey(int key, int scancode, int action, int mods);
	void ImGuiMouse(int button, int action, int mods);
	void ImGuiScroll(double xoffset, double yoffset);
	void ImGuiChar(unsigned int c);

	void IO_Window_HandleKey(int key, int scancode, int action, int mods)
	{
		// Send data to input class
		IO_Key_Callback(key, scancode, action, mods);
		ImGuiKey(key, scancode, action, mods);
	}
	void MouseCallback(GLFWwindow *window, int button, int action, int mods)
	{
		ImGuiMouse(button, action, mods);

		ImGuiIO &io = ImGui::GetIO();

		if (io.WantCaptureMouse)
			return;

		MouseDownData data;
		data.button = button;
		data.state = action;
		EventDispatch(ET_MOUSE_DOWN, &data);

	}
	void ScrollCallback(GLFWwindow *window, double xoffset, double yoffset)
	{
		ImGuiScroll(xoffset, yoffset);

		ImGuiIO &io = ImGui::GetIO();

		if (io.WantCaptureMouse)
			return;

		MouseScrollData data;
		data.scrollX = xoffset;
		data.scrollY = yoffset;
		EventDispatch(ET_MOUSE_SCROLL, &data);
	}
	void CharCallback(GLFWwindow *window, unsigned int c)
	{
		ImGuiChar(c);
		IO_Char_Callback(c);
	}
	void MouseMoveCallback(GLFWwindow* window, double xpos, double ypos)
	{
		ImGuiIO &io = ImGui::GetIO();

		if (io.WantCaptureMouse)
			return;

		MouseMoveData data;
		data.x = xpos;
		data.y = ypos;

		data.identX = (xpos / (float)g_width);
		data.identY = (ypos / (float)g_height);
		
		EventDispatch(ET_MOUSE_MOVE, &data);
	}


	void SetProjection(float width, float height)
	{
		float hw = width  * 0.5f;
		float hh = height * 0.5f;

		g_projLeft = -hw;
		g_projRight = hw;
		g_projUp = -hh;
		g_projDown = hh;

		// glm::ortho doesn't want to cooperate! So here is a simplified orthographic projection :D
		//g_projection = {
		//	2.0f / width,0,0,0,
		//	0,2.0f / -height,0,0,
		//	0,0,1,0,
		//	0,0,0,1
		//};

		g_projection = glm::ortho(g_projLeft, g_projRight, g_projDown, g_projUp);

		//orthoProjection = glm::ortho(-640, 640, 360, -360);
		// {
		// 	1,0,0,0,
		// 	0,1,0,0,
		// 	0,0,1,0,
		// 	0,0,0,1
		// };

	}

	void *IO_Window_Handle()
	{
		return g_window;
	}

	void CenterPos()
	{
		const GLFWvidmode *desktop = glfwGetVideoMode(glfwGetPrimaryMonitor());
		glfwSetWindowPos(g_window, (desktop->width - g_width) * 0.5f, (desktop->height - g_height) * 0.5f);
	}

	bool IO_Window_Initialize()
	{
		bool fullscreen = false;


		SettingValue value;

		#ifdef WGE_DEBUG

		g_width = 1280;
		g_height = g_width * (9.0f / 16.0f);
		//g_height = g_width * (3.0f / 4.0f);

		#else

		value.type = SETTING_WINDOW_FULLSCREEN;
		SettingsGet(&value);
		fullscreen = value.m_iValue;

		value.type = SETTING_WINDOW_WIDTH;
		SettingsGet(&value);

		g_width = value.m_iValue;

		value.type = SETTING_WINDOW_HEIGHT;
		SettingsGet(&value);
		g_height = value.m_iValue;

		#endif

		value.type = SETTING_WINDOW_REFRESH_RATE;
		SettingsGet(&value);
		g_refreshRate = value.m_iValue;

		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE); // Hide the window before creating it, to set the position to the center before showing it
		// Without this, the window will appear to flicker and it just screams "THIS IS A BAD GAME"

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);


		//if (fullscreen == 1) g_window = glfwCreateWindow(mode->width, mode->height, "Rhythmic", glfwGetPrimaryMonitor(), NULL);
		g_window = glfwCreateWindow(g_width, g_height, "Rhythmic", 0, NULL);

		// Sets up custom refresh rate if any
		glfwSetWindowMonitor(g_window, 0, 0, 0, g_width, g_height, g_refreshRate);

		if (fullscreen)
		{
			GLFWmonitor *monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode *mode = glfwGetVideoMode(monitor);

			glfwSetWindowMonitor(g_window, monitor, 0, 0, mode->width, mode->height, g_refreshRate);
		}

		GLFWimage icon;
		icon.width = g_iconWidth;
		icon.height = g_iconHeight;
		icon.pixels = g_iconData;

		glfwSetWindowIcon(g_window, 1, &icon);

		CenterPos();

		if (!g_window)
			return true;

		IO_Window_UpdateProjection();

		glfwMakeContextCurrent(g_window);
		glfwSwapInterval(0);

		glfwShowWindow(g_window);

		return false;
	}
	void IO_Window_SetCallbacks()
	{
		glfwSetCharCallback(g_window, CharCallback);
		glfwSetMouseButtonCallback(g_window, MouseCallback);
		glfwSetScrollCallback(g_window, ScrollCallback);
		glfwSetCursorPosCallback(g_window, MouseMoveCallback);
	}
	void IO_Window_Destroy()
	{
		glfwDestroyWindow(g_window);
	}

	void IO_Window_SwapBuffers()
	{
		glfwSwapBuffers(g_window);
	}

	void IO_Window_RefreshVars()
	{
		bool fullscreen = false;
		SettingValue value;
#ifdef WGE_DEBUG

		g_width = 1280;
		g_height = g_width * (9.0f / 16.0f);
		//g_height = g_width * (3.0f / 4.0f);

#else

		value.type = SETTING_WINDOW_FULLSCREEN;
		SettingsGet(&value);
		fullscreen = value.m_iValue != 0;

		value.type = SETTING_WINDOW_WIDTH;
		SettingsGet(&value);

		g_width = value.m_iValue;

		value.type = SETTING_WINDOW_HEIGHT;
		SettingsGet(&value);
		g_height = value.m_iValue;

#endif

		value.type = SETTING_WINDOW_REFRESH_RATE;
		SettingsGet(&value);
		g_refreshRate = value.m_iValue;

		if (fullscreen)
		{
			GLFWmonitor *monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode *mode = glfwGetVideoMode(monitor);

			glfwSetWindowMonitor(g_window, monitor, 0, 0, mode->width, mode->height, g_refreshRate);
		}
		else
		{
			glfwSetWindowMonitor(g_window, 0, 0, 0, g_width, g_height, g_refreshRate);
			CenterPos();
		}

		IO_Window_UpdateProjection();

		RenderManager::ResetViewport();
	}

	void IO_Window_UpdateProjection()
	{
		glfwGetWindowSize(g_window, &g_width, &g_height);

		SetProjection(g_width, g_height);
	}

	int IO_Window_GetWidth()
	{
		return g_width;
	}
	int IO_Window_GetHeight()
	{
		return g_height;
	}


	void IO_Window_SetDisplayMode(const DisplayMode &mode, int fullscreen, int refresh_rate_index)
	{
		
	}

	const glm::mat4 &IO_Window_GetProjection()
	{
		return g_projection;
	}

	const float &IO_Window_GetProjLeft()
	{
		return g_projLeft;
	}
	const float &IO_Window_GetProjRight()
	{
		return g_projRight;
	}
	const float &IO_Window_GetProjUp()
	{
		return g_projUp;
	}
	const float &IO_Window_GetProjDown()
	{
		return g_projDown;
	}
	
	
	// Dear ImGui stuff

	double g_time;
	bool g_mouseJustPressed[5] = { false, false, false, false, false };
	
	void ImGuiSetClipboard(void *, const char *string)
	{
		glfwSetClipboardString(g_window, string);
	}
	const char *ImGuiGetClipboard(void *)
	{
		return glfwGetClipboardString(g_window);
	}
	void ImGuiMouse(int button, int action, int mods)
	{
		if (ImGui::GetCurrentContext() == NULL)
			return;

		if (action == GLFW_PRESS && button >= 0 && button < IM_ARRAYSIZE(g_mouseJustPressed))
			g_mouseJustPressed[button] = true;
	}
	void ImGuiKey(int key, int scancode, int action, int mods)
	{
		if (ImGui::GetCurrentContext() == NULL)
			return;

		ImGuiIO& io = ImGui::GetIO();
		if (action == GLFW_PRESS)
			io.KeysDown[key] = true;
		if (action == GLFW_RELEASE)
			io.KeysDown[key] = false;

		// Modifiers are not reliable across systems
		io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
		io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
		io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
		io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];
	}
	void ImGuiChar(unsigned int c)
	{
		if (ImGui::GetCurrentContext() == NULL)
			return;

		ImGuiIO& io = ImGui::GetIO();
		if (c > 0 && c < 0x10000)
			io.AddInputCharacter((unsigned short)c);
	}
	void ImGuiScroll(double xoffset, double yoffset)
	{
		if (ImGui::GetCurrentContext() == NULL)
			return;

		ImGuiIO& io = ImGui::GetIO();
		io.MouseWheelH += (float)xoffset;
		io.MouseWheel += (float)yoffset;
	}

	void IO_Window_SetDearImGui()
	{
		ImGuiIO &io = ImGui::GetIO();

		//io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

		io.BackendPlatformName = "Rhythmic";
		io.BackendRendererName = "Rhythmic Renderer";
		io.IniFilename = 0;

		io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;
		io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
		io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
		io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
		io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
		io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
		io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
		io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
		io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
		io.KeyMap[ImGuiKey_Insert] = GLFW_KEY_INSERT;
		io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
		io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
		io.KeyMap[ImGuiKey_Space] = GLFW_KEY_SPACE;
		io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
		io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
		io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
		io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
		io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
		io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
		io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
		io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;

		io.SetClipboardTextFn = ImGuiSetClipboard;
		io.GetClipboardTextFn = ImGuiGetClipboard;
#ifdef WIN32
		io.ImeWindowHandle = glfwGetWin32Window(g_window);
#endif
		g_time = glfwGetTime();
	}


	static void IO_Window_DearImGui_UpdateCursorPos()
	{
		// Update buttons
		ImGuiIO& io = ImGui::GetIO();
		for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++)
		{
			// If a mouse press event came, always pass it as "mouse held this frame", so we don't miss click-release events that are shorter than 1 frame.
			io.MouseDown[i] = g_mouseJustPressed[i] || glfwGetMouseButton(g_window, i) != 0;
			g_mouseJustPressed[i] = false;
		}

		// Update mouse position
		const ImVec2 mouse_pos_backup = io.MousePos;
		io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);
#ifdef __EMSCRIPTEN__
		const bool focused = true; // Emscripten
#else
		const bool focused = glfwGetWindowAttrib(g_window, GLFW_FOCUSED) != 0;
#endif
		if (focused)
		{
			if (io.WantSetMousePos)
			{
				glfwSetCursorPos(g_window, (double)mouse_pos_backup.x, (double)mouse_pos_backup.y);
			}
			else
			{
				double mouse_x, mouse_y;
				glfwGetCursorPos(g_window, &mouse_x, &mouse_y);
				io.MousePos = ImVec2((float)mouse_x, (float)mouse_y);
			}
		}
	}

	void IO_Window_DearImGui_NewFrame()
	{
		if (ImGui::GetCurrentContext() == NULL)
			return;

		ImGuiIO& io = ImGui::GetIO();
		IM_ASSERT(io.Fonts->IsBuilt() && "Font atlas not built! It is generally built by the renderer back-end. Missing call to renderer _NewFrame() function? e.g. ImGui_ImplOpenGL3_NewFrame().");

		// Setup display size (every frame to accommodate for window resizing)
		int w, h;
		int display_w, display_h;
		glfwGetWindowSize(g_window, &w, &h);
		glfwGetFramebufferSize(g_window, &display_w, &display_h);
		io.DisplaySize = ImVec2((float)w, (float)h);
		if (w > 0 && h > 0)
			io.DisplayFramebufferScale = ImVec2((float)display_w / w, (float)display_h / h);

		// Setup time step
		double current_time = glfwGetTime();
		io.DeltaTime = g_time > 0.0 ? (float)(current_time - g_time) : (float)(1.0f / 60.0f);
		g_time = current_time;

		IO_Window_DearImGui_UpdateCursorPos();
	}

} // namespace Rhythmic