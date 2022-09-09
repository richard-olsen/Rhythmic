#ifndef RENDERING_VERTEX_BATCHER_H_
#define RENDERING_VERTEX_BATCHER_H_

#include <vector>

#define VERTEX_BATCH_MAX_VERTICES 100000*6
#define VERTEX_SIZE 9

namespace Rhythmic
{
	namespace Rendering 
	{
		struct Vertex
		{
			float x;
			float y;
			float z;

			float u;
			float v;

			float r;
			float g;
			float b;
			float a;
		};

		class VertexBatcher 
		{
		public:
			VertexBatcher();
			~VertexBatcher();

			void Clear();
			void Update();
			void Use();

			void Create(unsigned int max_value = VERTEX_BATCH_MAX_VERTICES);
			void Destroy();

			void AddVertex(float x, float y, float z, float u, float v, float r, float g, float b, float a);

			void Draw();
		private:
			float *m_vertices;
			unsigned int m_drawCount;
			unsigned int m_maxVertices;

			unsigned int m_vertexArrayObject;
			unsigned int m_vertexBufferObject;
		};
	} // namespace Rendering
}

#endif
