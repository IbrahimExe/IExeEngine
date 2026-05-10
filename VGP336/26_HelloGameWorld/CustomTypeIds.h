#pragma once

#include <IExeEngine/Inc/TypeIds.h>

enum class CustomComponentId
{
    CustomDebugDraw = static_cast<int>(IExeEngine::ComponentId::Count)
};

enum class CustomServiceId
{
    CustomDebugDrawDisplay = static_cast<int>(IExeEngine::ServiceId::Count)
};