#ifndef CATCHER_RENDERER_H_
#define CATCHER_RENDERER_H_

#include "catcher_manager.hpp"

#include "../../rendering/render_manager.hpp"
#include "../../themes/theme.hpp"

namespace Rhythmic
{
	class CatcherRenderer 
	{
	public:
		CatcherRenderer();
		~CatcherRenderer();

		void Init();

		void RenderCatchers(CatcherManager &manager, bool hitsOnly, bool flip);
		void Render6Foot(const Catcher& catcher, int activeCatchers, const glm::vec2& pos, int index, bool flip, bool add_joint);
		void RenderCatcher(const Catcher &catcher, const glm::vec2 &pos, int index, bool flip, bool add_joint);
	private:
		SheetReference				 m_highwayElements;
		Rendering::Shader			*m_billboard;
		Rendering::VertexBatcher	*m_batcher;
	};
} // namespace Rhythmic

#endif
