// ReSharper disable CppInconsistentNaming
#pragma once

struct Color
{
	float r = 0.0f;
	float g = 0.0f;
	float b = 0.0f;
	float a = 0.0f;

	Color() = default;

	Color(const float r, const float g, const float b, const float a) :
		r(r),
		g(g),
		b(b),
		a(a) {}

	explicit Color(const uint32_t color)
	{
		const uint8_t aChar = (color & 0xFF000000) >> 24;
		const uint8_t bChar = (color & 0x00FF0000) >> 16;
		const uint8_t gChar = (color & 0x0000FF00) >> 8;
		const uint8_t rChar = color & 0x000000FF;

		r = (float) rChar / 255.0f;
		g = (float) gChar / 255.0f;
		b = (float) bChar / 255.0f;
		a = (float) aChar / 255.0f;
	}

	static Color red;
	static Color green;
	static Color blue;
	static Color yellow;
	static Color magenta;
	static Color cyan;
	static Color white;
	static Color black;
};
