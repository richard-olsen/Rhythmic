#ifndef COLOR_H_
#define COLOR_H_

#include <glm/glm.hpp>

namespace Rhythmic
{
	const glm::vec4 g_white = glm::vec4(1, 1, 1, 1); // WHITE
	const glm::vec4 g_black = glm::vec4(0, 0, 0, 1); // BLACK
	const glm::vec4 g_dgrey = glm::vec4(0.05, 0.05, 0.06, 1); // DARK GREY
	const glm::vec4 g_grey = glm::vec4(0.21, 0.23, 0.23, 1); // GREY
	const glm::vec4 g_lgrey = glm::vec4(0.8, 0.8, 0.8, 1); //LIGHT GREY
	const glm::vec4 g_lbsp = glm::vec4(0.46f, 0.73f, 0.93f, 1); //LIGHT BLUE FOR HOPO
	const glm::vec4 g_lb = glm::vec4(0.35f, 0.58f, 0.7f, 1); //LIGHT BLUE


	const glm::vec4 g_inv = glm::vec4(0, 0, 0, 0);// INVISIBLE


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

