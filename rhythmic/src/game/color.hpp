#ifndef COLOR_H_
#define COLOR_H_

#include <glm/glm.hpp>

namespace Rhythmic
{
	const glm::vec4 g_white = glm::vec4(1, 1, 1, 1);
	const glm::vec4 g_colors[6] = {
		glm::vec4(0,1,0,1),			// GREEN
		glm::vec4(1,0,0,1),			// RED
		glm::vec4(1,1,0,1),			// YELLOW
		glm::vec4(0.2f,0.5f,1,1),	// BLUE
		glm::vec4(1,0.5f,0,1),		// ORANGE
		glm::vec4(1,0,0.75f,1)		// PURPLE
	};
	const glm::vec4 g_colorsSP[6] = {
		glm::vec4(0,1,1,1),			// BLUE SP
		glm::vec4(0,1,1,1),			// GREEN SP
		glm::vec4(0,1,1,1),			// YELLOW SP
		glm::vec4(0,1,1,1),			// ORANGE SP
		glm::vec4(0,1,1,1),			// RED SP
		glm::vec4(0,1,1,1)			// PURPLE SP
	};
}

#endif

