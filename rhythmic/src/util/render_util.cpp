#include "render_util.hpp"

#include <glm/glm.hpp>

namespace Rhythmic
{
	namespace RenderUtil
	{
		float Interpolate(float previous, float target, float alpha)
		{
			//return (target * alpha) + (previous * (1.0f - alpha));
			//return (previous * alpha) + (target * (1.0f - alpha));
			return glm::mix(previous, target, alpha);
		}

		glm::vec2 Interpolate(const glm::vec2 &previous, const glm::vec2 &target, float alpha)
		{
			return (target * alpha) + (previous * (1.0f - alpha));
		}
	}
}
