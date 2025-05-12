#include "Precompiled.h"
#include "MeshBuilder.h"

using namespace IExeEngine;
using namespace IExeEngine::Math;
using namespace IExeEngine::Graphics;

namespace
{
	Color GetNextColor(int& index)
	{
		constexpr Color colorTable[] = {
			Colors::Red,
			Colors::Yellow,
			Colors::Orange,
			Colors::Purple,
			Colors::Magenta,
			Colors::Pink,
			Colors::Blue,
		};

		index = (index + 1) % std::size(colorTable);
		return colorTable[index];
	}
}

MeshPC MeshBuilder::CreateCubePC(float size, const Color& color)
{
	MeshPC mesh;

	const float hs = size * 0.5f;

	// Front
	mesh.vertices.push_back({ { -hs, -hs, -hs }, color });
	mesh.vertices.push_back({ { -hs,  hs, -hs }, color });
	mesh.vertices.push_back({ {  hs,  hs, -hs }, color });
	mesh.vertices.push_back({ {  hs, -hs, -hs }, color });

	// Back
	mesh.vertices.push_back({ { -hs, -hs,  hs }, color });
	mesh.vertices.push_back({ { -hs,  hs,  hs }, color });
	mesh.vertices.push_back({ {  hs,  hs,  hs }, color });
	mesh.vertices.push_back({ {  hs, -hs,  hs }, color });

	mesh.indices = {
		// Front
		0, 1, 2,
		0, 2, 3,
		// Back
		7, 5, 4,
		7, 6, 5,
		// Right
		3, 2, 6,
		3, 6, 7,
		// Left
		4, 5, 1,
		4, 1, 0,
		// Top
		1, 5, 6,
		1, 6, 2,
		// Bottom
		0, 3, 7,
		0, 7, 4
	};

	return mesh;
}

// Outlines
MeshPC MeshBuilder::CreateCubePC(float size)
{
	MeshPC mesh;

	const float hs = size * 0.5f;

	// Front
	mesh.vertices.push_back({ { -hs, -hs, -hs }, color });
	mesh.vertices.push_back({ { -hs,  hs, -hs }, color });
	mesh.vertices.push_back({ {  hs,  hs, -hs }, color });
	mesh.vertices.push_back({ {  hs, -hs, -hs }, color });

	// Back
	mesh.vertices.push_back({ { -hs, -hs,  hs }, color });
	mesh.vertices.push_back({ { -hs,  hs,  hs }, color });
	mesh.vertices.push_back({ {  hs,  hs,  hs }, color });
	mesh.vertices.push_back({ {  hs, -hs,  hs }, color });

	mesh.indices = {
		// Front
		0, 1, 2,
		0, 2, 3,
		// Back
		7, 5, 4,
		7, 6, 5,
		// Right
		3, 2, 6,
		3, 6, 7,
		// Left
		4, 5, 1,
		4, 1, 0,
		// Top
		1, 5, 6,
		1, 6, 2,
		// Bottom
		0, 3, 7,
		0, 7, 4
	};

	return mesh;
}