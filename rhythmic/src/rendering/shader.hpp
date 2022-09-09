#ifndef RENDERING_SHADER_H_
#define RENDERING_SHADER_H_

#include <string>
#include <glm/glm.hpp>

#define BILLBOARD_CENTER "point_center"

namespace Rhythmic
{
	namespace Rendering
	{
		class Shader
		{
		public:
			Shader();
			~Shader();

			bool CreateShader(const std::string &shader);
			void DestroyShader();

			void Use();

			void SetScreenPos(const glm::mat4x4 &screenPos);
			void SetProjection(const glm::mat4x4 &projection);
			void SetCameraTransform(const glm::mat4x4 &cameraTransform);
			void SetModelView(const glm::mat4x4 &modelview);
			void SetInverseModelView(const glm::mat4x4 &inv_modelview);
			void SetTexture(const int &sampler);
			void SetFogPos(const float &fogPos);

			// For Gaussian Filter
			void SetDirection(const glm::vec2 &direction);

			// Only certain shaders (*cough* billboarding) supports this function
			void SetPointCenter(const glm::vec3 &value);
			//void SetVec3(const std::string &name, const glm::vec3 &value);

		private:
			bool m_successful;
			int m_vertexShader;
			int m_fragmentShader;

			int m_program;

			int m_uniScreenPos;
			int m_uniProjection;
			int m_uniCameraTransform;
			int m_uniModelview;
			int m_uniInvModelview;
			int m_uniTexture;
			int m_uniPointCenter;
			int m_uniFogPos;

			int m_uniDirection;

			void DestroyShaderObjects();
		};
	} // namespace Rendering
} // namespace Rhythmic

#endif
