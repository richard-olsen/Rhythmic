#include "render_manager.hpp"

#include <map>

#include "../util/misc.hpp"

#include "../io/io_window.hpp"
#include <wge/core/logging.hpp>

#include "glad/glad.h"

#include <glm/gtc/matrix_transform.hpp>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace Rhythmic
{
	namespace RenderManager
	{
		typedef dref<Rendering::Texture*> TexRef;
		typedef dref<Sheet*> SheetRef;
		typedef dref<Font*> FontRef;
		typedef dref<Mesh*> MeshRef;

		std::map<std::string, TexRef> g_textures;
		std::map<std::string, SheetRef> g_sheets;
		std::map<std::string, FontRef> g_fonts;
		std::map<std::string, MeshRef> g_meshes;

		Rendering::VertexBatcher g_batcher;
		FrameBuffer g_filterBuffer; // The frame buffer used to apply filters

		Rendering::Shader	g_shaders[SHADER_TYPES];

		int g_currentlyUsedShader;

		void OnWindowResize(GLFWwindow *window, int width, int height)
		{
			g_filterBuffer.Create(width, height);
		}

		bool Initialize()
		{
			if (!gladLoadGL())
			{
				LOG_CRITICAL("Failed to initialize GLAD!");
				return true;
			}

			g_shaders[SHADER_TYPE_DEFAULT].CreateShader("batcher");
			g_shaders[SHADER_TYPE_BILLBOARD].CreateShader("billboard");
			g_shaders[SHADER_TYPE_SCREEN].CreateShader("batcher");
			g_shaders[SHADER_TYPE_FX_GAUSSIAN_9].CreateShader("filter-gaussian");

			for (unsigned int i = 0; i < SHADER_TYPES; i++)
			{
				g_shaders[i].Use();
				g_shaders[i].SetProjection(IO_Window_GetProjection());
				g_shaders[i].SetModelView(glm::mat4x4(1.0f));
				g_shaders[i].SetScreenPos(glm::mat4x4(1.0f));
				g_shaders[i].SetCameraTransform(glm::mat4x4(1.0f));
			}
			g_currentlyUsedShader = SHADER_TYPE_SCREEN;
			g_shaders[SHADER_TYPE_SCREEN].Use();

			ResetScreenPos();

			g_batcher.Create();

			glfwSetWindowSizeCallback((GLFWwindow*)IO_Window_Handle(), OnWindowResize);

			g_filterBuffer.Create(IO_Window_GetWidth(), IO_Window_GetHeight());

			return false;
		}
		void Dispose()
		{
			// Remove textures, fonts, and sheets
			// Start off with fonts
			for (auto it = g_fonts.begin(); it != g_fonts.end(); it++)
			{
				delete it->second.data;
				it->second.reference = 0;
			}
			for (auto it = g_sheets.begin(); it != g_sheets.end(); it++)
			{
				delete it->second.data;
				it->second.reference = 0;
			}
			for (auto it = g_textures.begin(); it != g_textures.end(); it++)
			{
				delete it->second.data;
				it->second.reference = 0;
			}
			g_fonts.clear();
			g_sheets.clear();
			g_textures.clear();

			g_batcher.Destroy();

			g_shaders[0].DestroyShader();
			g_shaders[1].DestroyShader();
			g_shaders[2].DestroyShader();
		}

		void SetScreenPos(const glm::vec2 &pos, const glm::vec2 &scale)
		{
			//glm::mat4x4 screen = IO_Window_GetProjection();
			glm::mat4x4 screen = glm::mat4x4(1.0f);
			screen = glm::translate(screen, glm::vec3(pos, 0));
			screen = glm::scale(screen, glm::vec3(scale, 1));
			
			for (int i = 0; i < SHADER_TYPES; i++)
			{
				if (i == g_currentlyUsedShader)
					continue;
				g_shaders[i].Use();
				g_shaders[i].SetScreenPos(screen);
			}

			g_shaders[g_currentlyUsedShader].Use();
			g_shaders[g_currentlyUsedShader].SetScreenPos(screen);
		}

		void ResetScreenPos()
		{
			glm::mat4x4 screen = glm::mat4x4(1.0f);

			for (int i = 0; i < SHADER_TYPES; i++)
			{
				if (i == g_currentlyUsedShader)
					continue;
				g_shaders[i].Use();
				g_shaders[i].SetScreenPos(screen);
			}

			g_shaders[g_currentlyUsedShader].Use();
			g_shaders[g_currentlyUsedShader].SetScreenPos(screen);
		}

		Rendering::Shader *GetShader(unsigned int shaderType)
		{
			return &g_shaders[shaderType];
		}
		Rendering::VertexBatcher *GetBatcher()
		{
			return &g_batcher;
		}

		void UseShader(unsigned int shaderType)
		{
			g_currentlyUsedShader = shaderType;
			g_shaders[shaderType].Use();
		}

		TextureReference CreateTexture(const std::string &texture, int textureFlags)
		{
			return CreateTexture(texture, texture, textureFlags);
		}
		TextureReference CreateTexture(const std::string &texture, const std::string &reference, int textureFlags)
		{
			if (!texture.empty())
			{
				IO_Image img;
				IO_LoadImage(texture, &img);

				TextureReference ref;

				if (img.data == 0) // Incase there was a failure in loading the texture, just create a blank one
					ref.valid = false;
				else
					ref = CreateTexture(&img, reference, textureFlags);

				IO_FreeImageData(&img);
				return ref;
			}
			else
			{
				return CreateTexture(0, reference, textureFlags);
			}
		}
		TextureReference CreateTexture(IO_Image *texture, const std::string &reference, int textureFlags)
		{
			auto i = g_textures.find(reference);

			if (i != g_textures.end())
			{
				// Texture exists. Increase the reference and return the data
				i->second.reference++;

				return TextureReference(i->second.data, &i->first, &i->second.reference);
			}
			else
			{
				// Texture does not exist. Create a reference and return the data

				TexRef d = TexRef();

				d.data = new Rendering::Texture();

				d.data->Create(texture, textureFlags);

				d.reference = 0;

				g_textures.insert(std::pair<std::string, TexRef>(reference, d));

				auto j = g_textures.find(reference);

				return TextureReference(j->second.data, &j->first, &j->second.reference);
			}
		}
		TextureReference GetTexture(const std::string &reference)
		{
			auto i = g_textures.find(reference);

			if (i != g_textures.end())
			{
				// Texture exists. Increase the reference and return the data
				return TextureReference(i->second.data, &i->first, &i->second.reference);
			}
			return TextureReference();
		}
		void RemoveTexture(const std::string &texture)
		{
			auto i = g_textures.find(texture);

			if (i != g_textures.end())
			{
				if (i->second.data) delete i->second.data;

				g_textures.erase(texture);
			}
		}

		SheetReference CreateSheet(const std::string &texture, const std::string &reference, const int &tiles_x, const int &tiles_y, int textureFlags)
		{
			auto i = g_sheets.find(reference);

			if (i != g_sheets.end())
			{
				// Sheet exists. Increase the reference and return the data
				return SheetReference(i->second.data, &i->first, &i->second.reference);
			}
			else
			{
				// Sheet does not exist. Create a reference and return the data

				SheetRef d = SheetRef();

				IO_Image img;
				IO_LoadImage(texture, &img);

				d.data = new Sheet();
				d.data->Create(&img, tiles_x, tiles_y, textureFlags);

				IO_FreeImageData(&img);

				d.reference = 0;

				g_sheets.insert(std::pair<std::string, SheetRef>(reference, d));

				auto j = g_sheets.find(reference);

				return SheetReference(j->second.data, &j->first, &j->second.reference);
			}
		}

		SheetReference GetSheet(const std::string &sheet)
		{
			auto i = g_sheets.find(sheet);

			if (i != g_sheets.end())
			{
				// Sheet exists. Increase the reference and return the data
				return SheetReference(i->second.data, &i->first, &i->second.reference);
			}
			return SheetReference();
		}

		void RemoveSheet(const std::string &reference)
		{
			auto i = g_sheets.find(reference);

			if (i != g_sheets.end())
			{
				if (i->second.data) delete i->second.data;

				g_sheets.erase(reference);
			}
		}

		FontReference CreateFontRef(const std::string &font)
		{
			return CreateFontRef(font, font);
		}
		FontReference CreateFontRef(const std::string &font, const std::string &reference)
		{
			auto i = g_fonts.find(reference);

			if (i != g_fonts.end())
			{
				// Sheet exists. Increase the reference and return the data
				return FontReference(i->second.data, &i->first, &i->second.reference);
			}
			else
			{
				// Sheet does not exist. Create a reference and return the data

				FontRef d = FontRef();

				d.data = new Font();
				d.data->CreateFontFromFile(font);

				d.reference = 0;

				g_fonts.insert(std::pair<std::string, FontRef>(reference, d));

				auto j = g_fonts.find(reference);

				return FontReference(j->second.data, &j->first, &j->second.reference);
			}
		}
		FontReference GetFontRef(const std::string &reference)
		{
			auto i = g_fonts.find(reference);

			if (i != g_fonts.end())
			{
				// Sheet exists. Increase the reference and return the data
				return FontReference(i->second.data, &i->first, &i->second.reference);
			}
			return FontReference();
		}
		void RemoveFontRef(const std::string &reference)
		{
			auto i = g_fonts.find(reference);

			if (i != g_fonts.end())
			{
				if (i->second.data) delete i->second.data;

				g_fonts.erase(reference);
			}
		}

		MeshReference CreateMesh(const std::string &mesh)
		{
			return CreateMesh(mesh, mesh);
		}
		MeshReference CreateMesh(const std::string &mesh, const std::string &reference)
		{
			auto i = g_meshes.find(reference);

			if (i != g_meshes.end())
			{
				// Sheet exists. Increase the reference and return the data
				return MeshReference(i->second.data, &i->first, &i->second.reference);
			}
			else
			{
				// Sheet does not exist. Create a reference and return the data

				MeshRef d = MeshRef();

				d.data = new Mesh();
				d.data->Create(mesh);

				d.reference = 0;

				g_meshes.insert(std::pair<std::string, MeshRef>(reference, d));

				auto j = g_meshes.find(reference);

				return MeshReference(j->second.data, &j->first, &j->second.reference);
			}
		}
		MeshReference GetMesh(const std::string &reference)
		{
			auto i = g_meshes.find(reference);

			if (i != g_meshes.end())
			{
				// Mesh exists. Increase the reference and return the data
				return MeshReference(i->second.data, &i->first, &i->second.reference);
			}
			return MeshReference();
		}
		void RemoveMesh(const std::string &reference)
		{
			auto i = g_meshes.find(reference);

			if (i != g_meshes.end())
			{
				if (i->second.data) delete i->second.data;

				g_meshes.erase(reference);
			}
		}

		bool CanApplyFilter()
		{
			return g_filterBuffer.IsValid();
		}
		void ApplyBlur(FrameBuffer &affectedBuffer, float blur)
		{
			int lastShader = g_currentlyUsedShader;

			Rendering::VertexBatcher *batcher = RenderManager::GetBatcher();

			auto *gaussianShader = GetShader(SHADER_TYPE_FX_GAUSSIAN_9);
			gaussianShader->Use();

			/*
			Render in twice to get the desired blurring effect :)
			*/
			gaussianShader->SetDirection(glm::vec2(blur / (float)affectedBuffer.GetWidth(), 0));

			g_filterBuffer.BindTarget();
			affectedBuffer.Bind();
			gaussianShader->SetTexture(0);

			batcher->AddVertex(-1, -1, 0, 0, 0, 1, 1, 1, 1);
			batcher->AddVertex(1, -1, 0, 1, 0, 1, 1, 1, 1);
			batcher->AddVertex(1, 1, 0, 1, 1, 1, 1, 1, 1);

			batcher->AddVertex(1, 1, 0, 1, 1, 1, 1, 1, 1);
			batcher->AddVertex(-1, 1, 0, 0, 1, 1, 1, 1, 1);
			batcher->AddVertex(-1, -1, 0, 0, 0, 1, 1, 1, 1);

			batcher->Update();
			batcher->Draw();
			// Render second time
			affectedBuffer.BindTarget();
			g_filterBuffer.Bind();
			gaussianShader->SetDirection(glm::vec2(0, blur / (float)affectedBuffer.GetHeight()));
			batcher->AddVertex(-1, -1, 0, 0, 0, 1, 1, 1, 1);
			batcher->AddVertex(1, -1, 0, 1, 0, 1, 1, 1, 1);
			batcher->AddVertex(1, 1, 0, 1, 1, 1, 1, 1, 1);

			batcher->AddVertex(1, 1, 0, 1, 1, 1, 1, 1, 1);
			batcher->AddVertex(-1, 1, 0, 0, 1, 1, 1, 1, 1);
			batcher->AddVertex(-1, -1, 0, 0, 0, 1, 1, 1, 1);

			batcher->Update();
			batcher->Draw();

			FrameBuffer::BindGlobalTarget();

			UseShader(lastShader);
		}

		// Textures are assigned a string (for the name of the texture file)
		void RemoveByReference(const std::string &reference, int type) 
		{
			if (type == 0)
				RemoveTexture(reference);
			else if (type == 1)
				RemoveSheet(reference);
			else if (type == 2)
				RemoveFontRef(reference);
			else if (type == 3)
				RemoveMesh(reference);
		}

		static bool resetTextures = false;

		void DebugControls()
		{
#if 0
			SHORT local_resetTextures = GetAsyncKeyState(VK_F1);
			if (local_resetTextures && !resetTextures)
			{
				resetTextures = true;

				for (auto it = g_textures.begin(); it != g_textures.end(); it++)
				{
					IO_Image image;
					const std::string &file = it->second.data->GetFileName();
					
					if (file.empty())
						continue;

					IO_LoadImage(file, &image);
					it->second.data->UpdateTexture(&image);
					IO_FreeImageData(&image);
				}

				for (auto it = g_sheets.begin(); it != g_sheets.end(); it++)
				{
					IO_Image image;
					const std::string &file = it->second.data->GetTexture()->GetFileName();

					if (file.empty())
						continue;

					IO_LoadImage(file, &image);
					it->second.data->GetTexture()->UpdateTexture(&image);
					IO_FreeImageData(&image);
				}
			}
			else if (!local_resetTextures)
				resetTextures = false;
#endif
		}

		void InitDearImGui()
		{

		}
		void ResetViewport()
		{
			glViewport(0, 0, IO_Window_GetWidth(), IO_Window_GetHeight());
		}
	}
}