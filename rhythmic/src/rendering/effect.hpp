#ifndef RHYTHMIC_EFFECTS_H_
#define RHYTHMIC_EFFECTS_H_

#include <functional>

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

namespace Rhythmic
{
	enum EffectType
	{
		EFFECT_TYPE_WOBBLE,
		EFFECT_TYPE_BOUNCE,
		EFFECT_TYPE_SWELL,
		EFFECT_TYPE_size
	};

	enum EffectFlag
	{
		EFFECT_FLAG_REPEAT,
		EFFECT_FLAG_STOP_ON_END
	};

	class Effect
	{
	public:
		Effect();
		~Effect();

		void Animate(const float &delta);

		void SetSpeed(float speed);

		void PlayEffect(EffectType type, EffectFlag flag);
		void Reset();

		const glm::vec2 &GetPosition() const;
		const glm::vec2 &GetSize() const;
		const glm::vec4 &GetColor() const;
		const float &GetAngle() const;
	private:
		std::function<void()>	ef_Init;
		std::function<void()>	ef_Update;

		bool					m_paused;
		bool					m_repeat;
		float					m_speed;
		float					m_animSpeed;
		float					m_step;
		float					m_targetTime;

		// Values that can be affected
		glm::vec2				m_position;
		glm::vec2				m_scale;
		float					m_angle;
		glm::vec4				m_color;
	};
}

#endif
