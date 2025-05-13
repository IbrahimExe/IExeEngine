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
			Colors::Blue,
			Colors::CadetBlue,
			Colors::DarkBlue,
			Colors::CornflowerBlue,
			Colors::LightSkyBlue,
			Colors::DodgerBlue,
			Colors::LavenderBlush,
			Colors::BlueViolet,
			Colors::MidnightBlue,
			Colors::RoyalBlue,
			Colors::SteelBlue,
			Colors::WhiteSmoke,
			Colors::GhostWhite,
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

	int index = rand() % 100; // To Identify Vertices easier

	const float hs = size * 0.5f;

	// Front
	mesh.vertices.push_back({ { -hs, -hs, -hs }, GetNextColor(index) });
	mesh.vertices.push_back({ { -hs,  hs, -hs }, GetNextColor(index) });
	mesh.vertices.push_back({ {  hs,  hs, -hs }, GetNextColor(index) });
	mesh.vertices.push_back({ {  hs, -hs, -hs }, GetNextColor(index) });

	// Back
	mesh.vertices.push_back({ { -hs, -hs,  hs }, GetNextColor(index) });
	mesh.vertices.push_back({ { -hs,  hs,  hs }, GetNextColor(index) });
	mesh.vertices.push_back({ {  hs,  hs,  hs }, GetNextColor(index) });
	mesh.vertices.push_back({ {  hs, -hs,  hs }, GetNextColor(index) });

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

MeshPC MeshBuilder::CreatePyramid(float size)
{
	MeshPC mesh;

	int index = rand() % 100; 

	const float hs = size * 0.5f;

	// All Vertices for a Pyramid:
	// Base (square)
	mesh.vertices.push_back({ { -hs, 0.0f, -hs }, GetNextColor(index) });
	mesh.vertices.push_back({ {  hs, 0.0f, -hs }, GetNextColor(index) });
	mesh.vertices.push_back({ {  hs, 0.0f,  hs }, GetNextColor(index) });
	mesh.vertices.push_back({ { -hs, 0.0f,  hs }, GetNextColor(index) });
	// Peak
	mesh.vertices.push_back({ { 0.0f, size, 0.0f }, GetNextColor(index) });

	// All Indices for a Pyramid:
	mesh.indices = {
		// Base 
		1, 2, 0,
		2, 3, 0,
		// Sides 
		1, 0, 4, // Back Face
		2, 1, 4, // Right Face
		3, 2, 4, // Front Face
		0, 3, 4  // Left Face
	};

	return mesh;
}

MeshPC MeshBuilder::CreateRectanglePC(float width, float height, float depth)
{
	MeshPC mesh;

	int index = rand() % 100; 

	const float hw = width * 0.5f;
	const float hh = height * 0.5f;
	const float hd = depth * 0.5f;

	// All Vertices for a Rectangle:
	// Back face
	mesh.vertices.push_back({ { -hw, -hh, -hd }, GetNextColor(index) }); // 0
	mesh.vertices.push_back({ { -hw,  hh, -hd }, GetNextColor(index) }); // 1
	mesh.vertices.push_back({ {  hw,  hh, -hd }, GetNextColor(index) }); // 2
	mesh.vertices.push_back({ {  hw, -hh, -hd }, GetNextColor(index) }); // 3
	// Front face
	mesh.vertices.push_back({ { -hw, -hh,  hd }, GetNextColor(index) }); // 4
	mesh.vertices.push_back({ { -hw,  hh,  hd }, GetNextColor(index) }); // 5
	mesh.vertices.push_back({ {  hw,  hh,  hd }, GetNextColor(index) }); // 6
	mesh.vertices.push_back({ {  hw, -hh,  hd }, GetNextColor(index) }); // 7

	// All Indices for a Rectangle:
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
