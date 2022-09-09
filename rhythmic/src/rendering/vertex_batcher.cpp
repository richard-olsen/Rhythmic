#include "vertex_batcher.hpp"

#include "glad/glad.h"

#include <iostream>

namespace Rhythmic
{
	namespace Rendering
	{
		VertexBatcher::VertexBatcher() :
			m_vertices(0),
			m_drawCount(0),
			m_maxVertices(0),
			m_vertexArrayObject(0),
			m_vertexBufferObject(0)
		{}

		VertexBatcher::~VertexBatcher()
		{}

		void VertexBatcher::Clear()
		{
			m_drawCount = 0;
		}
		void VertexBatcher::Update()
		{
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * m_drawCount * VERTEX_SIZE, &m_vertices[0]);
		}

		void VertexBatcher::Use()
		{
			glBindVertexArray(m_vertexArrayObject);
			glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferObject);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * VERTEX_SIZE, (void*)0);		// Position
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * VERTEX_SIZE, (void*)12);	// Texture
			glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(float) * VERTEX_SIZE, (void*)20);	// Color
		}

		void VertexBatcher::Create(unsigned int max_size)
		{
			glGenVertexArrays(1, &m_vertexArrayObject);
			glGenBuffers(1, &m_vertexBufferObject);

			glBindVertexArray(m_vertexArrayObject);
			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glEnableVertexAttribArray(2);

			glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferObject);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * VERTEX_BATCH_MAX_VERTICES, 0, GL_DYNAMIC_DRAW);

			m_vertices = new float[max_size * VERTEX_SIZE];
			m_maxVertices = max_size;
		}

		void VertexBatcher::Destroy()
		{
			glDeleteBuffers(1, &m_vertexBufferObject);
			delete[] m_vertices;
			m_vertices = 0;
			m_maxVertices = 0;
		}

		void VertexBatcher::AddVertex(float x, float y, float z, float u, float v, float r, float g, float b, float a)
		{
			if (m_drawCount < m_maxVertices && m_vertices != 0)
			{
				unsigned int index = m_drawCount * VERTEX_SIZE;
				m_vertices[index + 0] = x;
				m_vertices[index + 1] = y;
				m_vertices[index + 2] = z;
				m_vertices[index + 3] = u;
				m_vertices[index + 4] = v;
				m_vertices[index + 5] = r;
				m_vertices[index + 6] = g;
				m_vertices[index + 7] = b;
				m_vertices[index + 8] = a;

				m_drawCount++;
			}
		}

		void VertexBatcher::Draw()
		{
			glDrawArrays(GL_TRIANGLES, 0, m_drawCount);
		}
	}
}
