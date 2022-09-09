#include "mesh.hpp"

//#include <assimp/Importer.hpp>
//#include <assimp/scene.h>
//#include <assimp/postprocess.h>

#include "glad/glad.h"

#include "render_manager.hpp"

#include "vertex_batcher.hpp"

namespace Rhythmic
{
	Mesh::Mesh() :
		m_vbo(0),
		m_ibo(0),
		m_vao(0),
		m_successful(false)
	{

	}
	Mesh::~Mesh()
	{
		Destroy();
	}

	bool Mesh::Create(const std::string &mesh)
	{
		//Assimp::Importer im;
		//const aiScene *scene = im.ReadFile(mesh, aiProcess_Triangulate | aiProcess_OptimizeMeshes | aiProcess_FlipUVs);

		//if (!scene)
		//	return false;

		//if (scene->HasMeshes())
		//{
		//	aiMesh *mesh = scene->mMeshes[0];

		//	Rendering::Vertex *vertices = new Rendering::Vertex[mesh->mNumVertices];
		//	m_indexCount = mesh->mNumFaces * 3;
		//	unsigned int *indices = new unsigned int[m_indexCount];
		//	for (int i = 0; i < mesh->mNumVertices; i++)
		//	{
		//		aiVector3D pos = mesh->mVertices[i];
		//		aiVector3D tex = mesh->HasTextureCoords(0) ? mesh->mTextureCoords[0][i] : aiVector3D();

		//		vertices[i].x = pos.x;
		//		vertices[i].y = pos.y;
		//		vertices[i].z = pos.z;
		//		vertices[i].u = tex.x;
		//		vertices[i].v = tex.y;
		//		vertices[i].r = 1;
		//		vertices[i].g = 1;
		//		vertices[i].b = 1;
		//		vertices[i].a = 1;
		//	}
		//	for (int i = 0; i < mesh->mNumFaces; i++)
		//	{
		//		aiFace face = mesh->mFaces[i];

		//		indices[i * 3 + 0] = face.mIndices[0];
		//		indices[i * 3 + 1] = face.mIndices[1];
		//		indices[i * 3 + 2] = face.mIndices[2];
		//	}

		//	//glGenVertexArrays(1, &m_vao);

		//	//glBindVertexArray(m_vao);

		//	glGenBuffers(1, &m_vbo);
		//	glGenBuffers(1, &m_ibo);

		//	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		//	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * VERTEX_SIZE * mesh->mNumVertices, vertices, GL_STATIC_DRAW);
		//	//glBufferData(GL_ARRAY_BUFFER, 32, 0, GL_DYNAMIC_DRAW);

		//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
		//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * m_indexCount, indices, GL_STATIC_DRAW);

		//	//glEnableVertexAttribArray(0); // Position
		//	//glEnableVertexAttribArray(1); // Texture
		//	//glEnableVertexAttribArray(2); // Color
		//	glBindBuffer(GL_ARRAY_BUFFER, 0);
		//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		//	//glBindVertexArray(0);

		//	delete[] vertices;
		//	delete[] indices;
		//	m_successful = true;
		//	return true;
		//}
		return false;
	}
	void Mesh::Destroy()
	{
		if (m_successful)
		{
			glDeleteBuffers(1, &m_vbo);
			glDeleteBuffers(1, &m_ibo);
			//glDeleteVertexArrays(1, &m_vao);
		}
	}

	void Mesh::Draw()
	{
		if (m_successful)
		{
			//glBindVertexArray(m_vao);
			glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * VERTEX_SIZE, (void*)0);		// Position
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * VERTEX_SIZE, (void*)12);	// Texture
			glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(float) * VERTEX_SIZE, (void*)20);	// Color

			glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}
	}
}