#include "effect.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace Rhythmic
{
	Effect::Effect() :	m_position(),
						m_scale(1),
						m_angle(0),
						m_color(1),
						m_speed(1), // 1 = normal, 0.5 = half, 2 = double
						m_animSpeed(1), // The default speed set by the effect
						m_paused(true),
						m_repeat(false),
						m_targetTime(0),
						m_step(0)
	{ }
	Effect::~Effect()
	{ }

	void Effect::Animate(const float &delta)
	{
		if (!m_paused)
		{
			m_step += delta * m_animSpeed * m_speed;

			if (m_step >= m_targetTime)
			{
				if (!m_repeat)
				{
					Reset();
					
					return;
				}
				else
				{
					m_step -= m_targetTime;
				}
			}

			ef_Update();
		}
	}

	void Effect::SetSpeed(float speed) { m_speed = speed; }

	void Effect::PlayEffect(EffectType type, EffectFlag flag)
	{
		Reset();
		switch (type)
		{
		case EFFECT_TYPE_WOBBLE:
			ef_Init = std::function<void()>([this]() { m_targetTime = 20.0f; m_animSpeed = 25.0f; });
			ef_Update = std::function<void()>([this]()
				{
					//m_position.y = -sin(m_step * 0.5f);
					//m_position.x = sin(m_step);
					const float constAmpX = 2.0f;
					const float constAmpY = 3.0f;
					float amplitude = glm::min(constAmpX, constAmpX / (m_step * 0.3f));

					float waveLength = m_step / 10.0f;
					m_position.x = amplitude * glm::sin(glm::two_pi<float>() * waveLength);
					
					waveLength = m_step / 20.0f;
					amplitude = glm::min(constAmpY, constAmpY / (m_step * 0.3f));
					m_position.y = amplitude * -glm::sin(glm::two_pi<float>() * waveLength);
				});
			break;
		case EFFECT_TYPE_BOUNCE:
			ef_Init = std::function<void()>([this]() {m_targetTime = 1.0f; m_animSpeed = 8.0f; });
			ef_Update = std::function<void()>([this]()
				{
					m_position.y = -1.0f + m_step;
				});
			break;
		}

		ef_Init();

		if (flag == EFFECT_FLAG_REPEAT)
			m_repeat = true;

		m_paused = false;
	}
	void Effect::Reset()
	{
		m_angle = 0;
		m_color = glm::vec4(1);
		m_paused = true;
		m_position = glm::vec2();
		m_scale = glm::vec2(1);
		m_speed = 1;
		m_animSpeed = 1;
		m_targetTime = 0;
		m_step = 0;
		m_repeat = false;
	}


	const glm::vec2 &Effect::GetPosition() const { return m_position; }
	const glm::vec2 &Effect::GetSize() const { return m_scale; }
	const glm::vec4 &Effect::GetColor() const { return m_color; }
	const float &Effect::GetAngle() const { return m_angle; }
}

