#include "stage_renderer.hpp"

#include "../player_renderer.hpp"
#include "../../container/player_container.hpp"
#include "../../io/io_window.hpp"

#include "../../util/timing.hpp"
#include "../../util/render_util.hpp"

#include "stage.hpp"

namespace Rhythmic
{
	StageRenderer::StageRenderer()
	{

	}
	StageRenderer *StageRenderer::GetInstance()
	{
		static StageRenderer renderer;
		return &renderer;
	}

	void StageRenderer::Render(double interpolation)
	{
		StageSystem *system = StageSystem::GetInstance();
		
		std::vector<Player*> *players = system->GetEstablishedPlayers();

		Rendering::VertexBatcher *batcher = RenderManager::GetBatcher();
		// Render Video / Image
		RenderManager::ResetScreenPos(); // Make sure to keep an identity matrix as the projection

		const glm::vec2 &imageScale = system->m_bgImageSize;
		static const glm::vec2 imagePos = glm::vec2(0.5f);

		Rendering::Texture *texture = system->GetBackgroundImage();
		texture->Bind();

		batcher->Clear();
		batcher->AddVertex(imagePos.x - imageScale.x, imagePos.y - imageScale.y, 0, 0, 0, 1, 1, 1, 1);
		batcher->AddVertex(imagePos.x + imageScale.x, imagePos.y - imageScale.y, 0, 1, 0, 1, 1, 1, 1);
		batcher->AddVertex(imagePos.x + imageScale.x, imagePos.y + imageScale.y, 0, 1, 1, 1, 1, 1, 1);
		batcher->AddVertex(imagePos.x + imageScale.x, imagePos.y + imageScale.y, 0, 1, 1, 1, 1, 1, 1);
		batcher->AddVertex(imagePos.x - imageScale.x, imagePos.y + imageScale.y, 0, 0, 1, 1, 1, 1, 1);
		batcher->AddVertex(imagePos.x - imageScale.x, imagePos.y - imageScale.y, 0, 0, 0, 1, 1, 1, 1);
		batcher->Update();
		batcher->Draw();

		// Render Players
		int activePlayers = players->size();

		float pos = (1.0f / (float)activePlayers);
		float offset = -2.0f + (1.0f - 1.0f / activePlayers);

		float aspect = (float)IO_Window_GetHeight() / (float)IO_Window_GetWidth();

		float sY = 0.8f;
		float sX = sY * aspect;

		if (sX > pos)
		{
			float invAspect = 1.0f / aspect;

			sX = pos;
			sY = sY - (0.025f * activePlayers);
		}
		float y = -(1.0f - sY);

		PlayerRenderer pRenderer;
		pRenderer.Init(true, true);

		//float interpolatedTime = system->GetSongTime() + (system->GetPlaybackSpeed() * interpolation);
		float interpolatedTime = system->GetSongTime();// RenderUtil::Interpolate(system->m_prevSongTime, system->m_songTime, interpolation);
		//interpolatedTime = system->m_prevSongTime + ((system->m_songTime - system->m_prevSongTime) * interpolation);
		//interpolatedTime = glm::mix(system->m_prevSongTime + offsetTime, system->m_songTime + offsetTime, interpolation); <--- This one is the used one

		for (int i = 0; i < activePlayers; i++)
		{
			ContainerPlayer *player = PlayerContainer::GetPlayerByIndex(i);

			pRenderer.RenderPlayer(*(*players)[i], interpolatedTime, glm::vec2(offset + (pos * 2 * (i + 1)), y), glm::vec2(sX, sY));
		}
		RenderManager::ResetScreenPos();
	}
}

