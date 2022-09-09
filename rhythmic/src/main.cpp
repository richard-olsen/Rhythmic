#include <iostream>
#include <random>
#include <wge/core/logging.hpp>

#include "io/io.hpp"

#include <wge/core/main.hpp>

#include "events/event_system.hpp"

#include "rendering/glad/glad.h"
#include "rendering/render_manager.hpp"
#include "rendering/font.hpp"
#include "rendering/ogl3/imgui_impl_opengl3.h"

#include "game/catchers/catcher_manager.hpp"
#include "game/catchers/catcher_renderer.hpp"
#include "game/notes/note_renderer.hpp"
#include "game/player.hpp"
#include "game/player_renderer.hpp"
#include "game/stage/stage.hpp"
#include "game/stage/stage_renderer.hpp"
#include "game/engine_vals.hpp"
#include "game/version_control.hpp"
#include "game/menu_music_player.hpp"

#include "container/player_container.hpp"
#include "container/song_container.hpp"
#include "container/binding_container.hpp"
#include "container/engine/engine_container.hpp"
#include "container/fretboard/fretboard_container.hpp"
#include "container/bg_image_container.hpp"
#include "container/secret/secret_container.hpp"

#include "container/engine/engine.hpp"

//#include "menus/menu_manager.hpp"
#include "menus/menu_system.hpp"
#include "menus/widgets/image.hpp"
#include "menus/widgets/progress_bar.hpp"
#include "menus/widgets/text.hpp"
#include "menus/canvas_factory.hpp"
#include "menus/widget_util.hpp"

#include "chart/chart_parser.hpp"

#include "themes/theme.hpp"

#include "audio/audio_system.hpp"

#include "net/network.hpp"

#include "profile/profiles.hpp"

#include "util/misc.hpp"
#include "util/settings.hpp"
#include "util/version.hpp"
#include "util/crash_helper.hpp"
#include "util/dear_imgui/imgui.h"
#include "util/timer.hpp"
#include "util/timing.hpp"

#include "input/input.hpp"

#include "util/thread_pool.hpp"

#include <thread>
#include <fstream>

#include <glm/common.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include <wge/core/time.hpp>
#include <wge/io/filesystem.hpp>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

void APIENTRY glDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam)
{
	(void) source;
	(void) type;
	(void) length;
	(void) userParam;
	if (severity == GL_DEBUG_SEVERITY_MEDIUM)
	{
		LOG_WARNING("GL Debug: (ID:{0}, Type:{1}, Source:{2}), {3}", id, type, source, message);
	}
	else if (severity == GL_DEBUG_SEVERITY_HIGH)
	{
		LOG_ERROR("GL Debug: (ID:{0}, Type:{1}, Source:{2}), {3}", id, type, source, message);
	}
	
	//else
	//{
	//	LOG_INFO("GL Debug: id({0}), {1}", id, message);
	//}
}

int gameEntry(int argc, char *argv[])
{
	std::string newPath = "";
	if (argc > 1) // Arguments have been passed into Rhythmic
	{
		for (int i = 1; i < argc; i++)
		{
			std::string t = argv[i];

			if (t.size() > 3)
			{
				if (t.substr(0, 4) == "-dd=") //dd is Debug Directory. Sets game's executable path to a custom one for debugging
				{
					std::vector<std::string> tokens = std::vector<std::string>();
					Rhythmic::Util::Split(t, "=", tokens);
					if (tokens.size() != 2)
						return 1;
					newPath = tokens[1];
				}
			}
		}
	}

#ifdef _WIN32
	if (SetThreadExecutionState(ES_DISPLAY_REQUIRED | ES_SYSTEM_REQUIRED | ES_CONTINUOUS) == NULL)
	{
		LOG_ERROR("Failed to set thread execution state: {0}", GetLastError());
	}
#endif

	Rhythmic::Util::Initialize();
	if (!newPath.empty())
	{
		newPath = Rhythmic::Util::RemoveQuotes(newPath);
		
		char back = newPath.back();
		if (back == '/' || back == '\\') // Remove the trailing /
			newPath = newPath.substr(0, newPath.size() - 1);
		
		Rhythmic::Util::SetExecutablePath(newPath);
		WGE::FileSystem::SetExecutablePath(newPath);
	}

	Rhythmic::SetUpCrashHandler();

	if (Rhythmic::IO_Systems_Initialize())
		return 1;

	Rhythmic::SettingsSystemInit();

	if (Rhythmic::IO_Window_Initialize())
		return 1;

	if (Rhythmic::RenderManager::Initialize())
		return 1;

	bool isRunning = true;

	Rhythmic::Rendering::Shader *screenShader = Rhythmic::RenderManager::GetShader(SHADER_TYPE_SCREEN);
	screenShader->Use();

	Rhythmic::RenderManager::GetBatcher()->Use();

	// Get the active theme from settings
	Rhythmic::Theme *theme = Rhythmic::GetActiveTheme();

	// Create font references so other elements of Rhythmic can access them
	Rhythmic::FontReference menusFont = Rhythmic::RenderManager::CreateFontRef(Rhythmic::Util::GetExecutablePath() + "/themes/default/fonts/FallingSky.ttf", "menus");
	Rhythmic::FontReference scoreFont = Rhythmic::RenderManager::CreateFontRef(Rhythmic::Util::GetExecutablePath() + "/themes/default/fonts/Aurulent-Sans-Mono-Regular.ttf", "score");
	// Same with texture references
	Rhythmic::SheetReference r = Rhythmic::RenderManager::CreateSheet(Rhythmic::Util::GetExecutablePath() + "/themes/default/sprites/highway_elements.png", "highway_elements", 40, 40);
	Rhythmic::SheetReference u = Rhythmic::RenderManager::CreateSheet(Rhythmic::Util::GetExecutablePath() + "/themes/default/sprites/ui_elements.png", "ui_elements", 40, 40, Rhythmic::Rendering::TEXTURE_FLAG_CLAMP);


	// Have an object of the menu system
	Rhythmic::MenuSystem *menuSystem = Rhythmic::MenuSystem::GetInstance();

	// Creating the loading "menu" for when the game is initializing
	Rhythmic::IO_Image image;
	Rhythmic::IO_LoadImage(Rhythmic::Util::GetExecutablePath() + "/game_data/images/note.png", &image);
	Rhythmic::WidgetImage *imgLogo = new Rhythmic::WidgetImage(image, Rhythmic::Rendering::TEXTURE_FLAG_CLAMP | Rhythmic::Rendering::TEXTURE_FLAG_FILTER_LINEAR);
	Rhythmic::IO_FreeImageData(&image);

	imgLogo->SetScale(glm::vec2(0.3f));
	imgLogo->SetPosition(glm::vec2(0.5f, 0.5f));

	// Create a canvas
	Rhythmic::Canvas *canvasLoading = new Rhythmic::Canvas();
	canvasLoading->Add("image_logo", imgLogo);

	Rhythmic::WidgetProgressBar *initProgressBar = new Rhythmic::WidgetProgressBar(0);
	initProgressBar->SetExtents(glm::vec2(0.3f, 0.8f), glm::vec2(0.7f, 0.85f));

	canvasLoading->Add("progress", initProgressBar);

	Rhythmic::WidgetText *initText = new Rhythmic::WidgetText("Initializing...", menusFont, 1);
	initText->SetPosition(glm::vec2(0.5f, 0.9));
	initText->SetScale(glm::vec2(0.02f));
	canvasLoading->Add("text", initText);

	// None of the widgets nor the canvas is deleted explicitly, as the menu system will clean all of that up when the menu is no longer in use

	int jobsFinished = 0;
	Rhythmic::ThreadPool::Initialize();

	std::string loadingProgress = "Initializing...";
	std::mutex loadingProgressMutex;

	menuSystem->InitializeListeners();

	// Offload some initialization to another thread, and let the main thread poll windows (let's Windows know that Rhythmic is a responsive program)
	Rhythmic::ThreadPool::QueueJob([&jobsFinished, theme, initProgressBar, &loadingProgress, &loadingProgressMutex]() {
		if (!Rhythmic::AudioSystem::GetInstance()->Initialize())
			exit(1);
		Rhythmic::FretboardContainer::Initialize();
		Rhythmic::BGContainer::Initialize();
		initProgressBar->SetProgress(0.1f);

		Rhythmic::ProfilesSystemInit();
		initProgressBar->SetProgress(0.2f);

		Rhythmic::EngineContainer::Initialize();
		initProgressBar->SetProgress(0.3f);

		Rhythmic::SecretContainer::InitializeSecrets();
		initProgressBar->SetProgress(0.4f);

		Rhythmic::InputInit();
		initProgressBar->SetProgress(0.5f);

		Rhythmic::InitializeBaseNet();
		initProgressBar->SetProgress(0.6f);

		{
			std::lock_guard<std::mutex> lock(loadingProgressMutex);
			loadingProgress = "Reading Cache";
		}
		bool cacheReadSuccessful = Rhythmic::SongContainer::ReadCache(); // Read cache
		if (!cacheReadSuccessful)
		{
			{
				std::lock_guard<std::mutex> lock(loadingProgressMutex);
				loadingProgress = "Scanning Songs";
			}
			Rhythmic::SongContainer::CacheSongs(); // Cache songs if the cache failed to read
		}
		initProgressBar->SetProgress(0.75f);

		{
			std::lock_guard<std::mutex> lock(loadingProgressMutex);
			loadingProgress = "Finishing Initialization...";
		}



		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGui::StyleColorsDark();

		ImGuiIO &imguiIO = ImGui::GetIO();

		imguiIO.Fonts->AddFontFromFileTTF((Rhythmic::Util::GetExecutablePath() + "/themes/default/fonts/FallingSky.ttf").c_str(), 16);
		initProgressBar->SetProgress(1.0f);

		Rhythmic::VersionControl *control = Rhythmic::VersionControl::GetInstance();
		control->ReadVersion();

		LOG_INFO(control->GetCorrectVersion().overview);
		if(control->IsLatestVersion())
			LOG_INFO("Latest build")
		else
			LOG_INFO("Out of date!")

		jobsFinished++;
	});

	Rhythmic::TextureReference white = Rhythmic::RenderManager::CreateTexture("", "white");

	menuSystem->QueuePushMenu(canvasLoading);

	screenShader->Use();
	screenShader->SetModelView(glm::mat4x4(1));
	screenShader->SetCameraTransform(glm::mat4x4(1));
	screenShader->SetProjection({
			 2,  0,  0,  0,
			 0, -2,  0,  0,
			 0,  0,  1,  0,
			-1,  1,  0,  1
		});

	// First loop to let windows know the program is a responsive program, this way users don't think Rhythmic crashed when it's really just loading stuff
	while (jobsFinished < 1)
	{
		Rhythmic::IO_Systems_Update(false);
		
		if (Rhythmic::IO_Systems_RequestedClose() || Rhythmic::Util::HasGameRequestedToClose())
			isRunning = false;

		{
			if (loadingProgressMutex.try_lock())
			{
				initText->SetText(loadingProgress);
				loadingProgressMutex.unlock();
				menuSystem->Update(0);
			}
		}

		// Display something before initializing everything else, this way it's not a white screen on start up
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		initProgressBar->Update(0); // Only the progress bar needs to update, so it can fix the positions everytime the progress updates
		menuSystem->DrawTopCanvas(0);


		//defFont->DrawString("Loading...", 0, imgLogo.m_element.m_scale.y * imgLogo.m_element.m_size.y * 0.7f, 42 / 1280.0f * Rhythmic::IO_Window_GetWidth(), glm::vec4(1, 1, 1, 1), 1);

		Rhythmic::IO_Window_SwapBuffers();
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	menuSystem->QueuePopMenu();

	// Push main menu onto the menu system
	menuSystem->QueuePushMenu(Rhythmic::CanvasFactory::CreateCanvas(Rhythmic::CANVAS_FACTORY_MAIN));

	//////////////////////////////////////////

	Rhythmic::PlayerContainer::Initialize();
	Rhythmic::BindingContainer::Initialize();


	Rhythmic::IO_Window_SetDearImGui();
	ImGui_ImplOpenGL3_Init("#version 150");

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(glDebugCallback, 0);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, 0, true);

	Rhythmic::IO_Window_SetCallbacks();

	//Rhythmic::Network::GetInstance()->ShowNetwork();
	Rhythmic::StageSystem *systemStage = Rhythmic::StageSystem::GetInstance();
	Rhythmic::StageRenderer *stageRenderer = Rhythmic::StageRenderer::GetInstance();
	
	systemStage->Init();

	double ticks = WGE::Core::GetTimeInSeconds();

	double lastTime = ticks;
	double timeSync = lastTime;

	double unprocessedRender = 0;

	Rhythmic::Timing *timings = Rhythmic::Timing::GetInstance();

	int loops = 0;

	const bool uncapped = true;

	while (isRunning)
	{
		using WGE::Core::TimeStep;

		double currentTime = WGE::Core::GetTimeInSeconds();
		double delta = currentTime - lastTime;
		TimeStep.m_timeStep = glm::min((float)(delta), (1.0f / 30.0f));
		TimeStep.m_fixedTimeStep = TimeStep.m_timeStep;
		lastTime = currentTime;

		unprocessedRender += delta;

		Rhythmic::TickAllTimers();

		double recentTime = 0;
		{
			bool noPlayers = Rhythmic::PlayerContainer::GetActivePlayers() == 0;

			Rhythmic::IO_Systems_Update();
			Rhythmic::InputPoll();

			if (Rhythmic::IO_Systems_RequestedClose() || Rhythmic::Util::HasGameRequestedToClose())
				isRunning = false;

			Rhythmic::AudioSystem::GetInstance()->Update();

			menuSystem->Update(TimeStep.m_fixedTimeStep);
			
			if (systemStage->IsActive())
			{
				systemStage->Update();
			}

			Rhythmic::MusicPlayerMenu::GetInstance()->Update(); // Update the music player for the menu

			Rhythmic::EventUpdateQueue();
		}

		//if (render)
		{
			//Rhythmic::RenderManager::DebugControls();

			ImGui_ImplOpenGL3_NewFrame();
			Rhythmic::IO_Window_DearImGui_NewFrame();
			ImGui::NewFrame();

			Rhythmic::EventDispatch(ET_IMGUI_TICK);
			//ImGui::ShowDemoWindow();

			ImGui::Render();

			glClear(GL_COLOR_BUFFER_BIT);
			glViewport(0, 0, Rhythmic::IO_Window_GetWidth(), Rhythmic::IO_Window_GetHeight());
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			Rhythmic::RenderManager::GetBatcher()->Use();
			//Rhythmic::RenderManager::UseShader(SHADER_TYPE_DEFAULT);
			screenShader->Use();

			double interpolation = (WGE::Core::GetTimeInSeconds() + timings->GetTickRate() - ticks) / timings->GetTickRate();

			if (systemStage->IsActive())
				stageRenderer->Render(interpolation);

			menuSystem->DrawTopCanvas(interpolation);

			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			Rhythmic::IO_Window_SwapBuffers();
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	Rhythmic::CleanTimers();

	menuSystem->PopAllMenus();

	menuSystem->DestroyListeners();

	Rhythmic::ThreadPool::Destroy();

	Rhythmic::DestroyBaseNet();
	
	Rhythmic::InputTerminate();

	Rhythmic::FretboardContainer::Dispose();

	Rhythmic::SongContainer::Dispose();

	Rhythmic::AudioSystem::GetInstance()->Destroy();
	Rhythmic::RenderManager::Dispose();

	Rhythmic::PlayerContainer::Dispose();

	Rhythmic::IO_Window_Destroy();
	Rhythmic::IO_Systems_Destroy();
	return 0;
}


