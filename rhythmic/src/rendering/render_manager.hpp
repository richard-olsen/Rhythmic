#ifndef RENDER_MANAGER_H_
#define RENDER_MANAGER_H_

#include "references.hpp"
#include "vertex_batcher.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "sheet.hpp"
#include "font.hpp"
#include "mesh.hpp"
#include "framebuffer.hpp"

#define SHADER_TYPE_DEFAULT 0
#define SHADER_TYPE_BILLBOARD 1
#define SHADER_TYPE_SCREEN 2
#define SHADER_TYPE_FX_GAUSSIAN_9 3
#define SHADER_TYPES 4

namespace Rhythmic
{
	namespace RenderManager
	{
		template<typename T>
		struct dref
		{
			T data;
			unsigned int reference;
		};

		bool Initialize();
		void Dispose();

		void SetScreenPos(const glm::vec2 &pos, const glm::vec2 &scale);
		void ResetScreenPos();

		Rendering::Shader *GetShader(unsigned int shaderType);
		Rendering::VertexBatcher *GetBatcher();

		void UseShader(unsigned int shaderType);
		
		TextureReference CreateTexture(const std::string &texture, int textureFlags = Rendering::TEXTURE_FLAG_FILTER_LINEAR | Rendering::TEXTURE_FLAG_CLAMP);
		TextureReference CreateTexture(const std::string &texture, const std::string &reference, int textureFlags = Rendering::TEXTURE_FLAG_FILTER_LINEAR | Rendering::TEXTURE_FLAG_CLAMP);
		TextureReference CreateTexture(IO_Image *texture, const std::string &reference, int textureFlags = Rendering::TEXTURE_FLAG_FILTER_LINEAR | Rendering::TEXTURE_FLAG_CLAMP);
		TextureReference GetTexture(const std::string &reference);
		void RemoveTexture(const std::string &reference);

		SheetReference CreateSheet(const std::string &sheet, const std::string &reference, const int &tiles_x, const int &tiles_y, int textureFlags = Rendering::TEXTURE_FLAG_FILTER_LINEAR | Rendering::TEXTURE_FLAG_CLAMP);
		SheetReference GetSheet(const std::string &sheet);
		void RemoveSheet(const std::string &reference);

		FontReference CreateFontRef(const std::string &font);
		FontReference CreateFontRef(const std::string &font, const std::string &reference);
		FontReference GetFontRef(const std::string &reference);
		void RemoveFontRef(const std::string &reference);

		MeshReference CreateMesh(const std::string &mesh);
		MeshReference CreateMesh(const std::string &mesh, const std::string &reference);
		MeshReference GetMesh(const std::string &reference);
		void RemoveMesh(const std::string &reference);

		bool CanApplyFilter();
		/*
		Applies a blur filter to the framebuffer

		This expects the frame buffer's size to be equal to the windows!
		*/
		void ApplyBlur(FrameBuffer &affectedBuffer, float blur);

		void DebugControls();

		void InitDearImGui();
		void DearImGui_NewFrame();

		void ResetViewport();
	};
}

#endif

