#ifndef ANIMATION_H_
#define ANIMATION_H_

#include <vector>

#include "sheet.hpp"

namespace Rhythmic
{
	enum AnimControls
	{
		ANIM_CONTROL_STOP_ON_END = (1 << 0),
		ANIM_CONTROL_REPEAT = (1 << 1),
		ANIM_CONTROL_RESET = (1 << 2),
		ANIM_CONTROL_PAUSE = (1 << 3)
	};
	struct AnimFrame
	{
		int tileX;				// Location on the sprite sheet
		int tileY;				// Location on the sprite sheet
		float tileExtensionX;	// How big of a tile to take (default = 1)
		float tileExtensionY;	// How big of a tile to take (default = 1)
	};
	class Animator
	{
	public:
		Animator(const std::vector<AnimFrame> &frames);
		Animator();
		virtual ~Animator();

		void SetSheet(const SheetReference &sheet);

		void SetFrames(const std::vector<AnimFrame> &frames);

		void SetFPS(float targetFrames);

		void SetFrameIndex(unsigned int index);

		void Animate();

		void Play(unsigned int controls);

		void Draw(const glm::vec2 &pos, const glm::vec2 &size, const glm::vec4 &color, bool mirror = false);
	private:
		SheetReference			m_sheetReference;
		std::vector<AnimFrame>	m_frames;

		unsigned int			m_frameIndex;

		bool					m_stopOnFinished;
		bool					m_paused;

		double					m_lastTime;
		double					m_fps;
	};
}

#endif

