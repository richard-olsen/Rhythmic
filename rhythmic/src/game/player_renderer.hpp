#ifndef PLAYER_RENDERER_H_
#define PLAYER_RENDERER_H_

#include "player.hpp"

#include "catchers/catcher_renderer.hpp"
#include "notes/note_renderer.hpp"

#include "../rendering/vertex_batcher.hpp"
#include "../rendering/sheet.hpp"
#include "../rendering/texture.hpp"
#include "../rendering/shader.hpp"

#include <glm/common.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#define HIGHWAY_FOV 50.0f

namespace Rhythmic
{
	class Stage;
	class PlayerRenderer
	{
	public:
		PlayerRenderer();
		~PlayerRenderer();

		void Init(bool displayStarpower, bool displayScoreModifier);

		void RenderPlayer(Player &player, double time, const glm::vec2 &pos, const glm::vec2 &scale);
	private:
		CatcherRenderer		m_catcherRenderer;
		NoteRenderer		m_noteRenderer;

		bool				m_displayStarpower;
		bool				m_displayScoreMod;

		glm::mat4x4			m_highwayProjection;
		glm::mat4x4			m_highwayModelView;
	};
}

#endif

