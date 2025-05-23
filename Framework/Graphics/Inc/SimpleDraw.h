#pragma once

#include "Color.h"

namespace IExeEngine::Graphics
{
	class Camera;

	namespace SimpleDraw
	{
		void StaticInitialize(uint32_t maxVertexCount);

		void StaticTerminate();

		void AddLine(const Math::Vector3& v0, const Math::Vector3& v1, const Color& color);

		void AddFace(const Math::Vector3& v0, const Math::Vector3& v1, const Math::Vector3& v2, const Color& color);
	}
}