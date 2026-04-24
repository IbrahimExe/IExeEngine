#pragma once

#include "Common.h"

#include "App.h"
#include "AppState.h"

// Game Object Info
#include "GameObject.h"
#include "GameWorld.h"
#include "GameObjectHandle.h"
#include "GameObjectFactory.h"

// Component Info
#include "TypeIds.h"
#include "Component.h"
#include "TransformComponent.h"
#include "CameraComponent.h"
#include "FPSCameraComponent.h"

// Services
#include "Service.h"

namespace IExeEngine
{
	App& MainApp();
}