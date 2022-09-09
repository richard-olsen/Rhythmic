/*
Element

The purpose of Theming Elements is to provide an abstract over how notes, strikelines, and other elements are displayed

These are only for rendering purposes, and hold no position or rotation value. These values are supplied to the layer
	via Render method, where each layer is rendered appropriately.

Users can attach layers to the element

ElementLayer base class resides in here
*/

#ifndef RHYTHMIC_THEMING_LAYER_HPP_
#define RHYTHMIC_THEMING_LAYER_HPP_

#include <glm/glm.hpp>

namespace Rhythmic
{
	struct ElementLayer
	{
		virtual void UpdateLayer(float delta) = 0;
		virtual void RenderLayer(float interpolation) = 0;

		glm::vec3 m_offset;
		glm::vec3 m_scale;
	};

	class ThemingElement
	{

	};
}

#endif

