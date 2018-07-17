#pragma once
#include <glm\vec4.hpp>

namespace glm
{
	static const vec4 Black(0, 0, 0, 1) ;
	static const vec4 White(1, 1, 1, 1);
	static const vec4 Grey(0.5f, 0.5f, 0.5f, 1);
	static const vec4 LightGrey(0.25f, 0.25f, 0.25f, 1);
	static const vec4 DarkGrey(0.75f, 0.75f, 0.75f, 1);

	static const vec4 Red(1, 0, 0, 1);
	static const vec4 Green(0, 1, 0, 1);
	static const vec4 Blue(0, 0, 1, 1);

	static const vec4 Yellow(1, 1, 0, 1);
	static const vec4 Magenta(1, 0, 1, 1);
	static const vec4 Cyan(0, 1, 1, 1);

	static const vec4 DarkRed(0.5f, 0, 0, 1);
	static const vec4 DarkGreen(0, 0.5f, 0, 1);
	static const vec4 DarkBlue(0, 0, 0.5f, 1);
}