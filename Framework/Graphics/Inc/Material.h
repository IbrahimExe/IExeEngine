#pragma once

#include "Color.h"

namespace IExeEngine::Graphics
{
	struct Material
	{
		Color emissive = Colors::Black;  // Color where light is absent
        Color ambient = Colors::White;   // Color where light is minimal
        Color diffuse = Colors::White;   // Base Light Color
        Color specular = Colors::White;  // Color for shiny areas
        float shininess = 10.0f;         // Intensity of the specular highlight
        float padding[3] = { 0.0f };     // Padding to make the structure 16-byte aligned
	};
}