#pragma once

#include "Color.h"

namespace IExeEngine::Graphics
{
	struct DirectionalLight
	{
		Color ambient = Colors::White;    // Any light color without light contact
		Color diffuse = Colors::White;    // Base light color
		Color specular = Colors::White;   // Highlight light color
		Math::Vector3 direction = Math::Vector3::ZAxis; // Direction of the light
		float padding = 0.0f; // Padding to maintain the 16 byte alignment
	};
}