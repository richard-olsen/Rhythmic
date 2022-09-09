#include "shader.hpp"

#include "glad/glad.h"

#include "../util/misc.hpp"

#include <wge/core/logging.hpp>

#include <fstream>
#include <iostream>

namespace Rhythmic
{
	namespace Rendering
	{
		bool HasCausedError(int object, int type, const std::string &file = "");
		std::string LoadShader(const std::string &file);

		Shader::Shader() : m_successful(false),
						   m_vertexShader(0),
						   m_fragmentShader(0),
						   m_program(0)
		{}
		
		Shader::~Shader()
		{}

		bool Shader::CreateShader(const std::string &name)
		{
			if (!m_successful)
			{
				m_vertexShader = glCreateShader(GL_VERTEX_SHADER);
				m_fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
				m_program = glCreateProgram();

				std::string shaderSource = LoadShader(name + ".vs");

				const GLchar *source = shaderSource.c_str();
				GLint length = shaderSource.size();

				glShaderSource(m_vertexShader, 1, &source, &length);
				glCompileShader(m_vertexShader);
				if (HasCausedError(m_vertexShader, GL_COMPILE_STATUS, name + ".vs"))
				{
					DestroyShaderObjects();
					return false;
				}

				shaderSource = LoadShader(name + ".fs");

				source = shaderSource.c_str();
				length = shaderSource.size();

				glShaderSource(m_fragmentShader, 1, &source, &length);
				glCompileShader(m_fragmentShader);
				if (HasCausedError(m_fragmentShader, GL_COMPILE_STATUS, name + ".fs"))
				{
					DestroyShaderObjects();
					return false;
				}

				m_successful = true;

				glAttachShader(m_program, m_vertexShader);
				glAttachShader(m_program, m_fragmentShader);

				glBindAttribLocation(m_program, 0, "position");
				glBindAttribLocation(m_program, 1, "texture");
				glBindAttribLocation(m_program, 2, "color");

				glLinkProgram(m_program);
				if (HasCausedError(m_program, GL_LINK_STATUS))
				{
					DestroyShader();
					return false;
				}
				glValidateProgram(m_program);
				if (HasCausedError(m_program, GL_VALIDATE_STATUS))
				{
					DestroyShader();
					return false;
				}
				
				m_uniProjection = glGetUniformLocation(m_program, "projection");
				m_uniCameraTransform = glGetUniformLocation(m_program, "cameraTransform");
				m_uniModelview = glGetUniformLocation(m_program, "modelview");
				m_uniInvModelview = glGetUniformLocation(m_program, "inv_modelview");
				m_uniTexture = glGetUniformLocation(m_program, "textureSample");
				m_uniPointCenter = glGetUniformLocation(m_program, BILLBOARD_CENTER);
				m_uniScreenPos = glGetUniformLocation(m_program, "screenPos");
				m_uniFogPos = glGetUniformLocation(m_program, "fogPos");

				m_uniDirection = glGetUniformLocation(m_program, "direction");

				return true;
			}
			return false;
		}

		void Shader::DestroyShader()
		{
			if (m_successful)
			{
				glDetachShader(m_program, m_vertexShader);
				glDetachShader(m_program, m_fragmentShader);
				DestroyShaderObjects();
				m_successful = false;
			}
		}

		void Shader::Use()
		{
			if (m_successful)
				glUseProgram(m_program);
		}

		void Shader::SetScreenPos(const glm::mat4x4 &screenPos)
		{
			if (m_successful && m_uniScreenPos != -1)
				glUniformMatrix4fv(m_uniScreenPos, 1, false, &screenPos[0][0]);
		}

		void Shader::SetProjection(const glm::mat4x4 &projection)
		{
			if (m_successful && m_uniProjection != -1)
				glUniformMatrix4fv(m_uniProjection, 1, false, &projection[0][0]);
		}

		void Shader::SetCameraTransform(const glm::mat4x4 &cameraTransform)
		{
			if (m_successful && m_uniCameraTransform != -1)
				glUniformMatrix4fv(m_uniCameraTransform, 1, false, &cameraTransform[0][0]);
		}

		void Shader::SetModelView(const glm::mat4x4 &modelview)
		{
			if (m_successful && m_uniModelview != -1)
				glUniformMatrix4fv(m_uniModelview, 1, false, &modelview[0][0]);
		}

		void Shader::SetInverseModelView(const glm::mat4x4 &inv_modelview)
		{
			if (m_successful && m_uniInvModelview != -1)
				glUniformMatrix4fv(m_uniInvModelview, 1, false, &inv_modelview[0][0]);
		}

		void Shader::SetTexture(const int &sampler)
		{
			if (m_successful && m_uniTexture != 1)
				glUniform1i(m_uniTexture, sampler);
		}

		void Shader::SetPointCenter(const glm::vec3 &value)
		{
			if (m_successful && m_uniPointCenter != -1)
				glUniform3fv(m_uniPointCenter, 1, &value[0]);
		}

		void Shader::SetFogPos(const float &fogPos)
		{
			if (m_successful && m_uniFogPos != -1)
				glUniform1f(m_uniFogPos, fogPos);
		}

		void Shader::SetDirection(const glm::vec2 &direction)
		{
			if (m_successful && m_uniDirection != -1)
				glUniform2f(m_uniDirection, direction.x, direction.y);
		}

		void Shader::DestroyShaderObjects()
		{
			glDeleteShader(m_vertexShader);
			glDeleteShader(m_fragmentShader);
			glDeleteProgram(m_program);
		}

		bool HasCausedError(int object, int type, const std::string &file)
		{
			bool ret = false;
			int param;
			if (glIsProgram(object))
			{
				glGetProgramiv(object, type, &param);
				if (param == GL_FALSE)
				{
					GLchar info[512];
					int len = 0;
					glGetProgramInfoLog(object, 512, &len, info);

					LOG_ERROR(std::string(info, len));
					ret = true;
				}
			}
			else if (glIsShader(object))
			{
				glGetShaderiv(object, type, &param);
				if (param == GL_FALSE)
				{
					GLchar info[512];
					int len = 0;
					glGetShaderInfoLog(object, 512, &len, info);

					LOG_ERROR("Shader Error ({0}): \n{1}", file, std::string(info, len));
					ret = true;
				}
			}

			return ret;
		}

		std::string LoadShader(const std::string &file)
		{
			std::string file_location = Util::GetExecutablePath() + "/game_data/shader/" + file;
			std::ifstream shader(file_location);
			if (!shader.is_open())
			{
				LOG_CRITICAL("Shader Not Found [{0}]", file_location);
				exit(1);
				return "";
			}

			std::string line;
			std::string ret = "";
			while (shader.good())
			{
				std::getline(shader, line);
				ret += (line + '\n');
			}
			return ret;
		}
	}
}