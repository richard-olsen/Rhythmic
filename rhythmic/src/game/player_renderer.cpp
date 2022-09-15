#include "player_renderer.hpp"

#include "../rendering/glad/glad.h"

#include "stage/stage.hpp"

#include "../rendering/render_manager.hpp"

#include "../util/misc.hpp"

#include "../io/io_window.hpp"

#include "engine_vals.hpp"

namespace Rhythmic
{
	int g_fcAnimIndex = 0;
	float g_fcAnimLastTime = 0;
	const float g_fcAnimFPS = (1.0f / 5.0f);

	PlayerRenderer::PlayerRenderer() :
		m_catcherRenderer(),
		m_noteRenderer(),
		m_highwayProjection(),
		m_highwayModelView()
	{
		glm::mat4x4 position = glm::mat4x4(1);
		//position = glm::translate(position, glm::vec3(0, 1.818f, -1.808f));
		position = glm::translate(position, glm::vec3(0, g_themeVars.highway_camera.offset, -g_themeVars.highway_camera.height));
		glm::quat rotation = glm::angleAxis(glm::radians(g_themeVars.highway_camera.rotation), glm::vec3(1, 0, 0));

		m_highwayProjection = glm::perspective(glm::radians(HIGHWAY_FOV), 1.0f, 0.01f, 13.0f);
		m_highwayModelView = glm::mat4_cast(rotation) * position;
	}

	PlayerRenderer::~PlayerRenderer()
	{ }

	void PlayerRenderer::Init(bool displayStarpower, bool displayScoreModifier)
	{
		m_displayStarpower = displayStarpower;
		m_displayScoreMod = displayScoreModifier;

		m_noteRenderer.Init();
		m_catcherRenderer.Init();
	}

	void PlayerRenderer::RenderPlayer(Player &player, double time, const glm::vec2 &pos, const glm::vec2 &scale)
	{
		Instrument *instrument = player.GetInstrument();

		glm::mat4x4 screen = glm::mat4x4(1.0f);
		screen = glm::translate(screen, glm::vec3(pos + (((instrument->GetEffectBounce().GetPosition() + instrument->GetEffectWobble().GetPosition()) * scale * 20.0f) * glm::vec2(1.0f/IO_Window_GetWidth(), 1.0f/IO_Window_GetHeight())), 0));
		screen = glm::scale(screen, glm::vec3(scale, 1));
		
		float animTime = Util::GetTime();
		if (animTime - g_fcAnimLastTime > g_fcAnimFPS)
		{
			// The FC anim utilizes the note counter (the one that counts 10 notes for the score modifier)
			g_fcAnimLastTime = animTime;
			g_fcAnimIndex++;
			if (g_fcAnimIndex >= 10)
				g_fcAnimIndex = 0;
		}
		PlayerSettings *settings = player.GetSettings();

		CatcherManager *cManager = instrument->GetCatcherManager();
		std::vector<Note> *notes = instrument->GetNotes();
		std::vector<Beatline> *beatlines = StageSystem::GetInstance()->GetBeatlines();

		Rendering::VertexBatcher *batcher = RenderManager::GetBatcher();
		Rendering::Shader *fretboard = RenderManager::GetShader(SHADER_TYPE_DEFAULT);
		Rendering::Shader *billboard = RenderManager::GetShader(SHADER_TYPE_BILLBOARD);
		SheetReference highway_elements = RenderManager::GetSheet("highway_elements");

		// Rendering the fretboard
		fretboard->Use();
		fretboard->SetTexture(0);
		fretboard->SetProjection(m_highwayProjection);
		fretboard->SetModelView(m_highwayModelView);
		fretboard->SetScreenPos(screen);
		fretboard->SetFogPos(g_themeVars.highway_camera.fogPos);
		player.GetFretboardTexture()->Bind();

		batcher->Use();
		batcher->Clear();

		float noteSpeed = (float)settings->noteSpeed;
		if (StageSystem::GetInstance()->GetPlaybackSpeed() >= 1.0f) noteSpeed /= StageSystem::GetInstance()->GetPlaybackSpeed();

		float fretboardOffset = time / 4.0f * noteSpeed;	// song_time / (fretboard_size / fretboard_texture_y_size) * note_speed
																			// or time / ((12.7f - -2.0f) / (1.0f - -2.675f)) * note_speed
																			// time / (14.7f / 3.675f) * note_speed
																			// time / 4f * note_speed

		// Adding the vertices of the fretboard to the batcher
		batcher->AddVertex(-1, -2.0f, 0, 0, 1 - fretboardOffset, 1, 1, 1, 1);
		batcher->AddVertex(1, -2.0f, 0, 1, 1 - fretboardOffset, 1, 1, 1, 1);
		batcher->AddVertex(1, 12.7f, 0, 1, -2.675f - fretboardOffset, 1, 1, 1, 1);
		batcher->AddVertex(1, 12.7f, 0, 1, -2.675f - fretboardOffset, 1, 1, 1, 1);
		batcher->AddVertex(-1, 12.7f, 0, 0, -2.675f - fretboardOffset, 1, 1, 1, 1);
		batcher->AddVertex(-1, -2.0f, 0, 0, 1 - fretboardOffset, 1, 1, 1, 1);

		// Update and render
		batcher->Update();
		batcher->Draw();

		if (settings->showHitRange)
		{
			batcher->Clear();
			TextureReference white = RenderManager::GetTexture("white");
			white->Bind();
			// Adds a range view
			// Gets the range adjusted for the notespeed
			glm::vec4 range = instrument->GetRange() * noteSpeed;

			const static float rangeHitAlpha = 0.3f;

			batcher->AddVertex(-1, range.x, 0,	0, 1,	1, 1, 1, rangeHitAlpha);
			batcher->AddVertex(1, range.x, 0,	1, 1,	1, 1, 1, rangeHitAlpha);
			batcher->AddVertex(1, range.y, 0,	1, 0,	1, 1, 1, rangeHitAlpha);
			batcher->AddVertex(1, range.y, 0,	1, 0,	1, 1, 1, rangeHitAlpha);
			batcher->AddVertex(-1, range.y, 0,	0, 0,	1, 1, 1, rangeHitAlpha);
			batcher->AddVertex(-1, range.x, 0,	0, 1,	1, 1, 1, rangeHitAlpha);

			batcher->AddVertex(-1, range.z, 0, 0, 1, 1, 1, 1, rangeHitAlpha);
			batcher->AddVertex(1, range.z, 0, 1, 1, 1, 1, 1, rangeHitAlpha);
			batcher->AddVertex(1, range.w, 0, 1, 0, 1, 1, 1, rangeHitAlpha);
			batcher->AddVertex(1, range.w, 0, 1, 0, 1, 1, 1, rangeHitAlpha);
			batcher->AddVertex(-1, range.w, 0, 0, 0, 1, 1, 1, rangeHitAlpha);
			batcher->AddVertex(-1, range.z, 0, 0, 1, 1, 1, 1, rangeHitAlpha);

			batcher->Update();
			batcher->Draw();
		}

		bool six = (instrument->GetInstrumentType() == INSTRUMENT_TYPE_6FRET || instrument->GetInstrumentType() == INSTRUMENT_TYPE_6FRETBASS);
		

		billboard->Use();
		billboard->SetTexture(0);
		billboard->SetFogPos(g_themeVars.highway_camera.fogPos);

		highway_elements.data->Bind();
		billboard->SetProjection(m_highwayProjection);
		billboard->SetModelView(m_highwayModelView);
		
		// Draw the sides
		billboard->SetPointCenter(glm::vec3(0, 0, 0));
		billboard->SetScreenPos(screen * glm::rotate(glm::mat4x4(1), glm::radians(-24.0f), glm::vec3(0, 0, 1)));


		batcher->Clear();
		highway_elements->AddSpriteScaled(39, 0, 0.25f, 25.0f, glm::vec2(-0.61f, -0.5f), glm::vec2(0.02f, 2.0f), glm::vec4(1), false);
		batcher->Update();
		batcher->Draw();

		billboard->SetScreenPos(screen * glm::rotate(glm::mat4x4(1), glm::radians(24.0f), glm::vec3(0, 0, 1)));
		batcher->Clear();
		highway_elements->AddSpriteScaled(39, 0, 0.25f, 25.0f, glm::vec2(0.61f, -0.5f), glm::vec2(0.02f, 2.0f), glm::vec4(1), true);
		batcher->Update();
		batcher->Draw();


		billboard->SetScreenPos(screen);
		
		m_noteRenderer.RenderBeatlines(*beatlines, time, noteSpeed);

		fretboard->Use();
		const std::vector<unsigned int> &activeSustains = instrument->GetActiveSustains();
		const std::vector<SustainMissData> &missedSustains = instrument->GetMissedSustains();
		const std::deque<WhammyEffectData> &whammyEffect = instrument->GetWhammyEffectData();
		m_noteRenderer.RenderSustains(*cManager, *notes, time, instrument->GetNoteCutoff(), activeSustains, missedSustains, whammyEffect, instrument->IsStarpowerActive(), (bool)settings->flip, six, noteSpeed);

		billboard->Use();

		m_catcherRenderer.RenderCatchers(*cManager, false, (bool)settings->flip);
		m_noteRenderer.RenderNotes(*cManager, *notes, time, instrument->GetNoteCutoff(), instrument->IsStarpowerActive(), (bool)settings->flip, noteSpeed);
		m_catcherRenderer.RenderCatchers(*cManager, true, (bool)settings->flip);

		billboard->SetPointCenter(glm::vec3(0, -0.45f, 0));
		batcher->Clear();

		int streak = instrument->GetStreak();

		bool full = streak >= (10 * instrument->GetMaxScoreModifier() - 10) || ((streak % 10 == 0) && !streak == 0);

		glm::vec2 pos2 = glm::vec2(-0.2f, 0.2f);
		glm::vec2 size2 = glm::vec2(0.4f, -0.2f);

		glm::vec2 tile = g_themeVars.highway.notecounter_tiles[10];
		
		if (full)
		{
			highway_elements.data->AddSprite(tile.x, tile.y, 3, 2, pos2, size2, { 1,1,1,1 });
		}
		else
		{
			glm::vec2 tile = g_themeVars.highway.notecounter_tiles[0];
			highway_elements.data->AddSprite(tile.x, tile.y, 3, 2, pos2, size2, { 1,1,1,1 });
			if (streak > 0)
			{
				while (streak > 10)
					streak -= 10;
				
				tile = g_themeVars.highway.notecounter_tiles[streak];
				highway_elements.data->AddSprite(tile.x, tile.y, 3, 2, pos2, size2, { 1,1,1,1 });
			}
		}

		// Draw FC anim

		if (instrument->IsFC())
		{
			const glm::vec4 goldColor = { 1,1,0,1 };
			const glm::vec4 botColor = { 0,1,1,1 };
			tile = g_themeVars.highway.fcAnim_tiles[g_fcAnimIndex];
			highway_elements.data->AddSprite(tile.x, tile.y, 3, 2, pos2, size2, instrument->IsBotControlling() ? botColor : goldColor);

			int second = g_fcAnimIndex + 5;
			if (second >= 10) second -= 10;

			tile = g_themeVars.highway.fcAnim_tiles[second];
			highway_elements.data->AddSprite(tile.x, tile.y, 3, 2, pos2, size2, instrument->IsBotControlling() ? botColor : goldColor);
		}

		// Draw modifier base

		tile = g_themeVars.highway.scoremod_base_tile;
		highway_elements.data->AddSprite(tile.x, tile.y, 3, 2, pos2, size2, { 1,1,1,1 });

		// Draw modifier
		int scoreModifier = instrument->GetScoreModifier();
		if (scoreModifier > 6)
		{
			if (scoreModifier == 8)
				scoreModifier = 6;
			if (scoreModifier == 10)
				scoreModifier = 7;
			if (scoreModifier == 12)
				scoreModifier = 8;
		}
		else
			scoreModifier--;

		tile = g_themeVars.highway.scoremod_tiles[scoreModifier];

		highway_elements.data->AddSprite(tile.x, tile.y, 3, 2, pos2, size2, instrument->IsStarpowerActive() ? glm::vec4(0, 1, 1, 1) : g_white);

		batcher->Update();
		batcher->Draw();
		// Add the pillars for starpower indicator

		billboard->SetPointCenter(glm::vec3(0, -0.35f, 0));
		batcher->Clear();
		
		tile = glm::vec2(22, 13);
		highway_elements.data->AddSprite(tile.x, tile.y, 2, 2, glm::vec2(-1.15f, 0.4f), glm::vec2( 0.4f, -0.4f), { 1,1,1,1 });
		highway_elements.data->AddSprite(tile.x, tile.y, 2, 2, glm::vec2( 1.15f, 0.4f), glm::vec2(-0.4f, -0.4f), { 1,1,1,1 });
		
		batcher->Update();
		batcher->Draw();


		// Fill the starpower

		TextureReference white = RenderManager::GetTexture("white");
		white.data->Bind();

		batcher->Clear();

		float starpowerAmount = instrument->GetStarpowerAmount();
		bool starpowerActive = instrument->IsStarpowerActive();

		bool brighter = starpowerAmount >= 0.5f || starpowerActive;

		highway_elements.data->AddSprite(0, 0, 1, 1, glm::vec2(-0.95, 0.12f), glm::vec2(1.9f, 0.077f) * glm::vec2(starpowerAmount, 1), brighter ? glm::vec4(0, 1, 1, 0.6f) : glm::vec4(0, 1, 1, 0.2f));

		batcher->Update();
		batcher->Draw();

		// Draw animators (star and hit)

		Animator *starPowerAnims = instrument->GetAnimsStarpower();
		Animator *hitAnims = instrument->GetAnimsHit();
		highway_elements.data->Bind();
		batcher->Clear();
		billboard->SetPointCenter(glm::vec3(0, g_themeVars.highway.catchers_offset, 0));

		glm::vec2 animXY = glm::vec2(-0.2f, 0.4f);
		glm::vec2 animSize = glm::vec2(0.4f, -0.4f);

		bool doFour = instrument->GetCatcherManager()->Size() == 4;
		
		if (doFour)
		{
			starPowerAnims[0].Draw(animXY + glm::vec2(CATCHER_POS_4_1 * (settings->flip ? -1 : 1), 0), animSize, glm::vec4(0, 1, 1, 1), false);
			starPowerAnims[1].Draw(animXY + glm::vec2(CATCHER_POS_4_2 * (settings->flip ? -1 : 1), 0), animSize, glm::vec4(0, 1, 1, 1), false);
			starPowerAnims[2].Draw(animXY + glm::vec2(CATCHER_POS_4_3 * (settings->flip ? -1 : 1), 0), animSize, glm::vec4(0, 1, 1, 1), false);
			starPowerAnims[3].Draw(animXY + glm::vec2(CATCHER_POS_4_4 * (settings->flip ? -1 : 1), 0), animSize, glm::vec4(0, 1, 1, 1), false);
			starPowerAnims[5].Draw(glm::vec2(-1.4f, 0.2f), glm::vec2(2.8f, -0.2f), glm::vec4(0, 1, 1, 1), false);
		}
		else if (instrument->GetCatcherManager()->Size() == 6) {
			starPowerAnims[0].Draw(animXY + glm::vec2(CATCHER_POS_6_1 * (settings->flip ? -1 : 1), 0), animSize, glm::vec4(0, 1, 1, 1), false);
			starPowerAnims[1].Draw(animXY + glm::vec2(CATCHER_POS_6_2, 0), animSize, glm::vec4(0, 1, 1, 1), false);
			starPowerAnims[2].Draw(animXY + glm::vec2(CATCHER_POS_6_3 * (settings->flip ? -1 : 1), 0), animSize, glm::vec4(0, 1, 1, 1), false);
			starPowerAnims[3].Draw(animXY + glm::vec2(CATCHER_POS_6_4 * (settings->flip ? -1 : 1), 0), animSize, glm::vec4(0, 1, 1, 1), false);
			starPowerAnims[4].Draw(animXY + glm::vec2(CATCHER_POS_6_5, 0), animSize, glm::vec4(0, 1, 1, 1), false);
			starPowerAnims[5].Draw(animXY + glm::vec2(CATCHER_POS_6_6 * (settings->flip ? -1 : 1), 0), animSize, glm::vec4(0, 1, 1, 1), false);
			starPowerAnims[6].Draw(glm::vec2(-1.4f, 0.2f), glm::vec2(2.8f, -0.2f), glm::vec4(0, 1, 1, 1), false);
		}
		else
		{
			starPowerAnims[0].Draw(animXY + glm::vec2(CATCHER_POS_5_1 * (settings->flip ? -1 : 1), 0), animSize, glm::vec4(0, 1, 1, 1), false);
			starPowerAnims[1].Draw(animXY + glm::vec2(CATCHER_POS_5_2 * (settings->flip ? -1 : 1), 0), animSize, glm::vec4(0, 1, 1, 1), false);
			starPowerAnims[2].Draw(animXY + glm::vec2(CATCHER_POS_5_3, 0), animSize, glm::vec4(0, 1, 1, 1), false);
			starPowerAnims[3].Draw(animXY + glm::vec2(CATCHER_POS_5_4 * (settings->flip ? -1 : 1), 0), animSize, glm::vec4(0, 1, 1, 1), false);
			starPowerAnims[4].Draw(animXY + glm::vec2(CATCHER_POS_5_5 * (settings->flip ? -1 : 1), 0), animSize, glm::vec4(0, 1, 1, 1), false);
			starPowerAnims[5].Draw(glm::vec2(-1.4f, 0.2f), glm::vec2(2.8f, -0.2f), glm::vec4(0, 1, 1, 1), false);
		}
		
		animXY.y = 0.65;
		animSize.y = -0.8;

		if (doFour)
		{
			hitAnims[0].Draw(animXY + glm::vec2(CATCHER_POS_4_1 * (settings->flip ? -1 : 1), 0), animSize, glm::vec4(1, 1, 1, 1.5f), false); //starpowerActive ? glm::vec4(0, 1, 1, 1) : g_colors[0], false);
			hitAnims[1].Draw(animXY + glm::vec2(CATCHER_POS_4_2 * (settings->flip ? -1 : 1), 0), animSize, glm::vec4(1, 1, 1, 1.5f), false); //starpowerActive ? glm::vec4(0, 1, 1, 1) : g_colors[1], false);
			hitAnims[2].Draw(animXY + glm::vec2(CATCHER_POS_4_3 * (settings->flip ? -1 : 1), 0), animSize, glm::vec4(1, 1, 1, 1.5f), false); //starpowerActive ? glm::vec4(0, 1, 1, 1) : g_colors[2], false);
			hitAnims[3].Draw(animXY + glm::vec2(CATCHER_POS_4_4 * (settings->flip ? -1 : 1), 0), animSize, glm::vec4(1, 1, 1, 1.5f), false); //starpowerActive ? glm::vec4(0, 1, 1, 1) : g_colors[3], false);
			hitAnims[5].Draw(glm::vec2(-1.4f, 0.2f), glm::vec2(2.8f, -0.2f), starpowerActive ? glm::vec4(0, 1, 1, 1) : g_colors[5], false);
		}
		else if (instrument->GetCatcherManager()->Size() == 6) {
			hitAnims[0].Draw(animXY + glm::vec2(CATCHER_POS_6_1 * (settings->flip ? -1 : 1), 0), animSize, glm::vec4(1, 1, 1, 1.5f), false); //starpowerActive ? glm::vec4(0, 1, 1, 1) : g_colors[1], false);
			hitAnims[1].Draw(animXY + glm::vec2(CATCHER_POS_6_2, 0), animSize, glm::vec4(1, 1, 1, 1.5f), false); //starpowerActive ? glm::vec4(0, 1, 1, 1) : g_colors[2], false);
			hitAnims[2].Draw(animXY + glm::vec2(CATCHER_POS_6_3 * (settings->flip ? -1 : 1), 0), animSize, glm::vec4(1, 1, 1, 1.5f), false); //starpowerActive ? glm::vec4(0, 1, 1, 1) : g_colors[3], false);
			hitAnims[3].Draw(animXY + glm::vec2(CATCHER_POS_6_4 * (settings->flip ? -1 : 1), 0), animSize, glm::vec4(1, 1, 1, 1.5f), false); //starpowerActive ? glm::vec4(0, 1, 1, 1) : g_colors[1], false);
			hitAnims[4].Draw(animXY + glm::vec2(CATCHER_POS_6_5, 0), animSize, glm::vec4(1, 1, 1, 1.5f), false); //starpowerActive ? glm::vec4(0, 1, 1, 1) : g_colors[2], false);
			hitAnims[5].Draw(animXY + glm::vec2(CATCHER_POS_6_6 * (settings->flip ? -1 : 1), 0), animSize, glm::vec4(1, 1, 1, 1.5f), false); //starpowerActive ? glm::vec4(0, 1, 1, 1) : g_colors[3], false);
			hitAnims[6].Draw(glm::vec2(-1.4f, 0.2f), glm::vec2(2.8f, -0.2f), starpowerActive ? glm::vec4(0, 1, 1, 1) : g_colors[5], false);
		}
		else
		{
			hitAnims[0].Draw(animXY + glm::vec2(CATCHER_POS_5_1 * (settings->flip ? -1 : 1), 0), animSize, glm::vec4(1, 1, 1, 1.5f), false); //starpowerActive ? glm::vec4(0, 1, 1, 1) : g_colors[0], false);
			hitAnims[1].Draw(animXY + glm::vec2(CATCHER_POS_5_2 * (settings->flip ? -1 : 1), 0), animSize, glm::vec4(1, 1, 1, 1.5f), false); //starpowerActive ? glm::vec4(0, 1, 1, 1) : g_colors[1], false);
			hitAnims[2].Draw(animXY + glm::vec2(CATCHER_POS_5_3, 0), animSize, glm::vec4(1, 1, 1, 1.5f), false); //starpowerActive ? glm::vec4(0, 1, 1, 1) : g_colors[2], false);
			hitAnims[3].Draw(animXY + glm::vec2(CATCHER_POS_5_4 * (settings->flip ? -1 : 1), 0), animSize, glm::vec4(1, 1, 1, 1.5f), false); //starpowerActive ? glm::vec4(0, 1, 1, 1) : g_colors[3], false);
			hitAnims[4].Draw(animXY + glm::vec2(CATCHER_POS_5_5 * (settings->flip ? -1 : 1), 0), animSize, glm::vec4(1, 1, 1, 1.5f), false); //starpowerActive ? glm::vec4(0, 1, 1, 1) : g_colors[4], false);
			hitAnims[5].Draw(glm::vec2(-1.4f, 0.2f), glm::vec2(2.8f, -0.2f), starpowerActive ? glm::vec4(0, 1, 1, 1) : g_colors[5], false);
		}

		batcher->Update();
		batcher->Draw();

	}
}
