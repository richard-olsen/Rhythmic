#ifndef NOTE_RENDERER_H_
#define NOTE_RENDERER_H_

#include "../../rendering/render_manager.hpp"

#include "note.hpp"
#include "beatline.hpp"

#include "../catchers/catcher_manager.hpp"
#include "../color.hpp"

#include <deque>

namespace Rhythmic
{
	struct SustainMissData
	{
		float timeMissed;
		unsigned int pointer;
	};

	struct WhammyEffectData
	{
		float stretch;		// The stretch factor 0 - 1
		float time;			// 0 - 7 (7 seconds being the most a player can see on notespeed of 1)
	};

	class NoteRenderer
	{
	public:
		NoteRenderer();
		~NoteRenderer();

		void Init();

		float GetX(const Note &note, unsigned int catcherCount);
		void RenderNotes(CatcherManager &cManager, const std::vector<Note> &notes, float timeOffset, unsigned int noteCutoff, bool isStarpower, bool flip, float noteSpeed = 1.0f, unsigned int viewDistance = 7);
		void RenderNote(const Catcher &catcher, const Note &note, float x, const glm::vec4 &color, bool drum, float timeOffset, float noteSpeed);
		void RenderNoteOpen(const Note &note, const glm::vec4 &color, float timeOffset, float noteSpeed);

		void RenderSustains(CatcherManager &cManager, const std::vector<Note> &notePool, float timeOffset, unsigned int noteCutoff, const std::vector<unsigned int> &activeSustains, const std::vector<SustainMissData> &missedSustains, const std::deque<WhammyEffectData> &whammyEffectData, bool isStarpower, bool flip, float noteSpeed = 1.0f, unsigned int viewDistance = 7);
		//void RenderSustain(const Catcher &catcher, const Note &note, float x, const glm::vec4 &color, bool drum, float timeOffset, float noteSpeed, bool missed, float missTime, const std::vector<WhammyEffectData> &whammyData, bool active = false);
		//void RenderSustainOpen(const Note &note, const glm::vec4 &color, float timeOffset, float noteSpeed, bool missed, float missTime, const std::vector<WhammyEffectData> &whammyData, bool active = false);
	
		void RenderSustain(const Note &note, float x, float size, const glm::vec4 &color, float timeOffset, float noteSpeed, bool missed, float missTime, const std::deque<WhammyEffectData> &whammyData, bool active, float stretchFactor);

		void RenderBeatlines(std::vector<Beatline> &beatlines, float timeOffset, float noteSpeed = 1.0f, unsigned int viewDistance = 7);
	private:
		SheetReference				 m_highwayElements;
		Rendering::Shader			*m_default;
		Rendering::Shader			*m_billboard;
		Rendering::VertexBatcher	*m_batcher;
	};
}

#endif

