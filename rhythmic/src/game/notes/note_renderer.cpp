#include "note_renderer.hpp"

#include <iostream>

#include "../../rendering/render_manager.hpp"

#include "../../themes/theme.hpp"

namespace Rhythmic
{
	NoteRenderer::NoteRenderer() :
		m_highwayElements()
	{ }
	NoteRenderer::~NoteRenderer()
	{ }

	void NoteRenderer::Init()
	{
		m_billboard = RenderManager::GetShader(SHADER_TYPE_BILLBOARD);
		m_default = RenderManager::GetShader(SHADER_TYPE_DEFAULT);
		m_highwayElements = RenderManager::GetSheet("highway_elements");
		m_batcher = RenderManager::GetBatcher();
	}

	float NoteRenderer::GetX(const Note &note, unsigned int catcherCount)
	{
		if (note.note == 5)
			return 0;
		if (catcherCount == 4)
		{
			if (note.note == NOTE_1)
				return CATCHER_POS_4_1;
			else if (note.note == NOTE_2)
				return CATCHER_POS_4_2;
			else if (note.note == NOTE_3)
				return CATCHER_POS_4_3;
			else if (note.note == NOTE_4)
				return CATCHER_POS_4_4;
		}
		else if (catcherCount == 5)
		{
			if (note.note == NOTE_1)
				return CATCHER_POS_5_1;
			else if (note.note == NOTE_2)
				return CATCHER_POS_5_2;
			else if (note.note == NOTE_3)
				return CATCHER_POS_5_3;
			else if (note.note == NOTE_4)
				return CATCHER_POS_5_4;
			else if (note.note == NOTE_5)
				return CATCHER_POS_5_5;
		}
		return 0;
	}
	void NoteRenderer::RenderNotes(CatcherManager &cManager, const std::vector<Note> &notePool, float timeOffset, unsigned int noteCutoff, bool isStarpower, bool flip, float noteSpeed, unsigned int viewDistance)
	{
		if (noteCutoff >= notePool.size())
			return;
		//unsigned int size = notePool.Size();
		bool drums = cManager.IsDrums();

		// This rendering should be more efficient. Instead of iterating from the end to the cutoff, it starts at the cutoff and goes until the note is out of range
		unsigned int start = noteCutoff;
		unsigned int end = notePool.size() - 1;
		for (unsigned int i = start; i < notePool.size(); i++)
		{
			const Note &note = notePool[i];
			if ((note.time - timeOffset) * noteSpeed > viewDistance)
			{
				end = i;
				break;
			}
		}
		// Now it renders in reverse order (there is no depth test, since shit WILL GO DOWN if it was enabled. Which is why the game renders from back to near
		for (unsigned int i = end; i >= start; i--)
		{
			if (i >= notePool.size()) // There is a case where i will just jump to the maximum value of an unsigned int. It's what happens when (0 - x). There are no negatives
				break;
			const Note &note = notePool[i];

			if (IS_NOTE(note, NOTE_FLAG_HIT))
				continue;

			if (note.note != NOTE_OPEN && note.note < 5)
			{
				Catcher *catcher = cManager[note.note];
				RenderNote(*catcher, note, GetX(note, cManager.Size()) * (flip ? -1 : 1), isStarpower ? glm::vec4(0, 1, 1, 1) : g_colors[catcher->type], drums, timeOffset, noteSpeed);
			}
			else
			{
				RenderNoteOpen(note, isStarpower ? glm::vec4(0,1,1,1) : g_colors[5], timeOffset, noteSpeed);
			}
		}
	}
	void NoteRenderer::RenderNote(const Catcher &catcher, const Note &note, float x, const glm::vec4 &color, bool drum, float timeOffset, float noteSpeed)
	{
		m_billboard->SetPointCenter(glm::vec3(x, ((note.time - timeOffset) * noteSpeed) + g_themeVars.highway.element_correction_note_time_offset, 0));
		m_batcher->Clear();

		glm::vec2 pos = glm::vec2(-g_themeVars.highway.note_size.x * 0.5f, g_themeVars.highway.note_size.y - (g_themeVars.highway.element_y_pixel_size*g_themeVars.highway.element_correction_note));
		glm::vec2 size = glm::vec2(g_themeVars.highway.note_size.x, -g_themeVars.highway.note_size.y);

		bool useCymbal = IS_NOTE(note, NOTE_FLAG_CYMBAL);
		if (IS_NOTE(note, NOTE_FLAG_TAP))
		{
			m_highwayElements.data->AddSprite((useCymbal || !drum) ? 2 : 0, 12, 2, 1, pos, size, color);
			m_highwayElements.data->AddSprite((useCymbal || !drum) ? 2 : 0, 9, 2, 1, pos, size, g_white);
		}
		else
		{
			m_highwayElements.data->AddSprite((useCymbal || !drum) ? 2 : 0, 10, 2, 1, pos, size, color);
			m_highwayElements.data->AddSprite((useCymbal || !drum) ? 2 : 0, 9, 2, 1, pos, size, g_white);
			if (IS_NOTE(note, NOTE_FLAG_HOPO))
				m_highwayElements.data->AddSprite((useCymbal || !drum) ? 2 : 0, 13, 2, 1, pos, size, g_white);
		}
		if (IS_NOTE(note, NOTE_FLAG_IS_STARPOWER))
		{
			m_highwayElements.data->AddSprite((useCymbal || !drum) ? 2 : 0, 11, 2, 1, pos, size, glm::vec4(0, 1, 1, 1));
		}

		m_batcher->Update();
		m_batcher->Draw();
	}

	void NoteRenderer::RenderNoteOpen(const Note &note, const glm::vec4 &color, float timeOffset, float noteSpeed)
	{
		// 25 26 27(HOPO overlay)

		// Render color, then base

		m_billboard->SetPointCenter(glm::vec3(0, (((note.time - timeOffset) * noteSpeed) + g_themeVars.highway.element_correction_note_open_time_offset) + 0.0001f, 0));
		m_batcher->Clear();

		glm::vec3 pos = glm::vec3(-g_themeVars.highway.open_note_size.x * 0.5f, g_themeVars.highway.open_note_size.y - (g_themeVars.highway.element_y_pixel_size*g_themeVars.highway.element_correction_note_open), 0);
		glm::vec3 size = glm::vec3(g_themeVars.highway.open_note_size.x, -g_themeVars.highway.open_note_size.y, 0);
		
		if (IS_NOTE(note, NOTE_FLAG_TAP))
		{
			m_highwayElements.data->AddSprite(0, 28, 12, 1, pos, size, color);
			m_highwayElements.data->AddSprite(0, 25, 12, 1, pos, size, g_white);
		}
		else
		{
			m_highwayElements.data->AddSprite(0, 26, 12, 1, pos, size, color);
			m_highwayElements.data->AddSprite(0, 27, 12, 1, pos, size, g_white);
			m_highwayElements.data->AddSprite(0, 25, 12, 1, pos, size, g_white);
			if (IS_NOTE(note, NOTE_FLAG_HOPO))
				m_highwayElements.data->AddSprite(0, 29, 12, 1, pos, size, g_white);
		}
		if (IS_NOTE(note, NOTE_FLAG_IS_STARPOWER))
		{
			m_highwayElements.data->AddSprite(0, 27, 12, 1, pos, size, glm::vec4(0, 1, 1, 1));
		}

		m_batcher->Update();
		m_batcher->Draw();
	}

	void NoteRenderer::RenderSustains(CatcherManager &cManager, const std::vector<Note> &notePool, float timeOffset, unsigned int noteCutoff, const std::vector<unsigned int> &activeSustains, const std::vector<SustainMissData> &missedSustains, const std::deque<WhammyEffectData> &whammyEffectData, bool isStarpower, bool flip, float noteSpeed, unsigned int viewDistance)
	{
		// Need a way to render active sustains
		bool drums = cManager.IsDrums();

		// Render active sustains first
		for (unsigned int i = 0; i < activeSustains.size(); i++)
		{
			if (activeSustains[i] > notePool.size())
				break;
			const Note &note = notePool[activeSustains[i]];
			if (note.time - timeOffset > viewDistance || note.sustain - timeOffset < -1)
				continue;
			if (note.sustain > note.time)
			{
				if (note.note != NOTE_OPEN)
				{
					Catcher *catcher = cManager[note.note];
					RenderSustain(note, GetX(note, cManager.Size()) * (flip ? -1 : 1), 0.25f, (isStarpower || IS_NOTE(note, NOTE_FLAG_ADDING_STARPOWER)) ? g_colorsSP[catcher->type] : g_colors[catcher->type], timeOffset, noteSpeed, false, note.lastScoreCheck, whammyEffectData, true, 1.0f);
				}
				else
				{
					RenderSustain(note, 0, 2, (isStarpower || IS_NOTE(note, NOTE_FLAG_ADDING_STARPOWER)) ? g_colorsSP[5] : g_colors[5], timeOffset, noteSpeed, false, note.lastScoreCheck, whammyEffectData, true, 0.4f);
				}
			}
		}

		// Missed sustains
		for (unsigned int i = 0; i < missedSustains.size(); i++)
		{
			if (missedSustains[i].pointer > notePool.size())
				break;
			const Note &note = notePool[missedSustains[i].pointer];
			if (note.time - timeOffset > viewDistance || note.sustain - timeOffset < -1)
				continue;
			if (note.sustain > note.time)
			{
				if (note.note != NOTE_OPEN)
				{
					Catcher *catcher = cManager[note.note];
					RenderSustain(note, GetX(note, cManager.Size()) * (flip ? -1 : 1), 0.25f, g_white, timeOffset, noteSpeed, true, missedSustains[i].timeMissed, whammyEffectData, false, 1.0f);
				}
				else
				{
					RenderSustain(note, 0, 2, g_white, timeOffset, noteSpeed, true, missedSustains[i].timeMissed, whammyEffectData, false, 1.0f);
				}
			}
		}

		if (noteCutoff == notePool.size())
			return;

		// This rendering should be more efficient. Instead of iterating from the end to the cutoff, it starts at the cutoff and goes until the note is out of range
		unsigned int start = noteCutoff;
		unsigned int end = notePool.size() - 1;
		for (unsigned int i = start; i < notePool.size(); i++)
		{
			const Note &note = notePool[i];
			if ((note.time - timeOffset) * noteSpeed > viewDistance)
			{
				end = i;
				break;
			}
		}
		// Now it renders in reverse order (there is no depth test, since shit WILL GO DOWN if it was enabled. Which is why the game renders from back to near
		for (unsigned int i = end; i >= start; i--)
		{
			if (i >= notePool.size()) // There is a case where i will just jump to the maximum value of an unsigned int. It's what happens when (0 - x). There are no negatives
				break;
			const Note &note = notePool[i];

			if (IS_NOTE(note, NOTE_FLAG_HIT))
				continue;

			if (note.sustain > note.time)
			{
				if (note.note != NOTE_OPEN && note.note <= 4)
				{
					Catcher *catcher = cManager[note.note];
					RenderSustain(note, GetX(note, cManager.Size()) * (flip ? -1 : 1), 0.25f, (isStarpower || IS_NOTE(note, NOTE_FLAG_ADDING_STARPOWER)) ? g_colorsSP[catcher->type] : g_colors[catcher->type], timeOffset, noteSpeed, false, 0, whammyEffectData, false, 1.0f);
				}
				else
				{
					RenderSustain(note, 0, 2, (isStarpower || IS_NOTE(note, NOTE_FLAG_ADDING_STARPOWER)) ? g_colorsSP[5] : g_colors[5], timeOffset, noteSpeed, false, 0, whammyEffectData, false, 0.4f);
				}
			}
		}
	}

	void NoteRenderer::RenderSustain(const Note &note, float x, float size, const glm::vec4 &color, float timeOffset, float noteSpeed, bool missed, float missTime, const std::deque<WhammyEffectData> &whammyData, bool active, float stretchFactor)
	{
		m_batcher->Clear();

		const bool useWhammyVisuals = true;

		//glm::vec2 pos = glm::vec2(-0.2f, 0.1f);
		//glm::vec2 size = glm::vec2(0.4f, -0.2f);

		// Render body
		float begin;
		if (!active && !missed)
			begin = (note.time - timeOffset) * noteSpeed + 0.15f;
		else
			begin = (missTime - timeOffset) * noteSpeed + 0.15f;
		float end = (note.sustain - timeOffset) * noteSpeed;

		Rendering::VertexBatcher *batcher = RenderManager::GetBatcher();

		// Render body
		if (active && useWhammyVisuals)
		{
			glm::vec4 activeColor = color * glm::vec4(2.0f, 2.0f, 2.0f, 1.0f);

			glm::vec4 texCoords = m_highwayElements->GetTextureCoordForQuadOnSheet(36, 1, 2, 0);
			glm::vec4 activeTexCoords = m_highwayElements->GetTextureCoordForQuadOnSheet(32, 1, 2, 0);

			float begTemp = 0;

			glm::vec2 pos = glm::vec2(x, 0);
			glm::vec2 scale;
			
			for (auto it = whammyData.begin(); it != whammyData.end(); it++)
			{
				float mEnd = end - begin; // Modified end to work in whammy space

				auto it2 = it + 1;

				float time = mEnd;
				if (it2 != whammyData.end())
					time = it2->time * noteSpeed;

				if (time > mEnd)
					time = mEnd;

				float t = (time - begTemp) * 0.5f;
				pos.y = begin + begTemp + t;
				scale.x = size * (1.0f + (it->stretch * stretchFactor));
				scale.y = t;

				m_highwayElements->AddSpriteScaled(36, 1, 2, 0, pos, scale, color, false);
				m_highwayElements->AddSpriteScaled(32, 1, 2, 0, pos, scale, activeColor, false);

				begTemp = time;

				if (time >= mEnd)
				{
					begTemp = end;
					break;
				}
			}

			begTemp += begin;

			float tail = end + 0.25f;

			if (begTemp < end)
			{
				float t = (end - begTemp) * 0.5f;
				pos.y = begTemp + t;
				scale.x = size;
				scale.y = t;

				m_highwayElements->AddSpriteScaled(36, 1, 2, 0, pos, scale, color, false);
				m_highwayElements->AddSpriteScaled(32, 1, 2, 0, pos, scale, activeColor, false);

				t = (tail - end) * 0.5f;
				pos = glm::vec2(x, end + t);
				scale.y = -t;

				m_highwayElements->AddSpriteScaled(36, 0, 2, 0.5f, pos, scale, color, false);
				m_highwayElements->AddSpriteScaled(32, 0, 2, 0.5f, pos, scale, activeColor, false);
			}
			else
			{
				float t = (tail - end) * 0.5f;
				pos.y = end + t;
				scale.y = -t;

				m_highwayElements->AddSpriteScaled(36, 0, 2, 0.5f, pos, scale, color, false);
				m_highwayElements->AddSpriteScaled(32, 0, 2, 0.5f, pos, scale, activeColor, false);
			}
		}
		else
		{
			float t = (end - begin) * 0.5f;
			glm::vec2 pos = glm::vec2(x, begin + t);
			glm::vec2 scale = glm::vec2(size, t);

			if (missed)
				m_highwayElements->AddSpriteScaled(34, 1, 2, 0, pos, scale, g_white);
			else
				m_highwayElements->AddSpriteScaled(36, 1, 2, 0, pos, scale, color);
			if (active)
				m_highwayElements->AddSpriteScaled(32, 1, 2, 0, pos, scale, color * glm::vec4(2.0f, 2.0f, 2.0f, 1.0f));
		}
		// Render tail
		if (!(active && useWhammyVisuals))
		{
			begin = (note.sustain - timeOffset) * noteSpeed;
			end = (note.sustain - timeOffset) * noteSpeed + 0.25f;
			float t = (end - begin) * 0.5f;
			glm::vec2 pos = glm::vec2(x, begin + t);
			glm::vec2 scale = glm::vec2(size, -t);

			if (missed)
				m_highwayElements->AddSpriteScaled(34, 0, 2, 0.5f, pos, scale, g_white);
			else
				m_highwayElements->AddSpriteScaled(36, 0, 2, 0.5f, pos, scale, color);
			if (active)
				m_highwayElements->AddSpriteScaled(32, 0, 2, 0.5f, pos, scale, color * glm::vec4(2.0f, 2.0f, 2.0f, 1.0f));
		}

		m_batcher->Update();
		m_batcher->Draw();
	}

	void NoteRenderer::RenderBeatlines(std::vector<Beatline> &beatlines, float timeOffset, float noteSpeed, unsigned int viewDistance)
	{
		// This rendering should be more efficient. Instead of iterating from the end to the cutoff, it starts at the cutoff and goes until the note is out of range
		unsigned int start = 0;
		unsigned int end = beatlines.size() - 1;
		for (unsigned int i = start; i < beatlines.size(); i++)
		{
			Beatline &beatline = beatlines[i];

			if ((beatline.time - timeOffset) < -2)
				start++;

			if ((beatline.time - timeOffset) * noteSpeed > viewDistance)
			{
				end = i;
				break;
			}
		}
		// Now it renders in reverse order (there is no depth test, since shit WILL GO DOWN if it was enabled. Which is why the game renders from back to near
		for (unsigned int i = end; i >= start; i--)
		{
			if (i >= beatlines.size())
				break;
			Beatline &beatline = beatlines[i];

			m_billboard->SetPointCenter(glm::vec3(0, ((beatline.time - timeOffset) * noteSpeed) + 0.0002f, 0));

			m_batcher->Clear();

			switch (beatline.type)
			{
			case BEATLINE_TYPE_FULL:
				m_highwayElements.data->AddSprite(8, 4, 1, 1, glm::vec2(-1.0f, 0.25f - (g_themeVars.highway.element_y_pixel_size*g_themeVars.highway.element_correction_beatline)), glm::vec2(2.0f, -0.25f), g_white);
				break;
			case BEATLINE_TYPE_HALF:
				m_highwayElements.data->AddSprite(8, 5, 1, 1, glm::vec2(-1.0f, 0.25f - (g_themeVars.highway.element_y_pixel_size*g_themeVars.highway.element_correction_beatline)), glm::vec2(2.0f, -0.25f), g_white);
				break;
			}

			m_batcher->Update();
			m_batcher->Draw();
		}
	}
}