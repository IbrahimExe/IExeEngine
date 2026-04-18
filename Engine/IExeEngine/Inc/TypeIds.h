#pragma once

namespace IExeEngine
{
    enum class ComponentId 
    {
        Invalid,    // default value, not a valid component

        Transform,  // transform component for local data
        Camera,     // contains the camera for viewing
        FPSCamera,  // Move the camera component with FPS controls

        Count       // last value, can be used to chain custom components
    };

    enum class ServiceId
    {
        Invalid,            // Default value, not valid Id
        Camera,				// Manages the cameras in the world
        Count               // Last value, can be used to chain custom services
    };
}

#define SET_TYPE_ID(id)\
    static uint32_t StaticGetTypeId() { return static_cast<uint32_t>(id); }\
    uint32_t GetTypeId() const override { return StaticGetTypeId(); }
