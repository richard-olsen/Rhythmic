#ifndef MESH_H_
#define MESH_H_

#include "references.hpp"

#include <string>

namespace Rhythmic
{
	class Mesh
	{
	public:
		Mesh();
		~Mesh();

		bool Create(const std::string &mesh);
		void Destroy();

		void Draw();
	private:
		bool m_successful;

		unsigned int m_vbo;
		unsigned int m_ibo;
		unsigned int m_vao;

		unsigned int m_indexCount;
	};

	typedef DataReference<Mesh, const std::string, 3> MeshReference;
}

#endif

