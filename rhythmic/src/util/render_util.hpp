#ifndef RENDER_UTIL_HPP_
#define RENDER_UTIL_HPP_

#include <glm/common.hpp>
#include <glm/vec2.hpp>

namespace Rhythmic
{
	namespace RenderUtil
	{
		float Interpolate(float previous, float target, float alpha);
		glm::vec2 Interpolate(const glm::vec2 &previous, const glm::vec2 &target, float alpha);
	}
}

#endif

