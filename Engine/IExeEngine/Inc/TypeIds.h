#pragma once

namespace IExeEngine
{
    enum class ComponentId 
    {
        Invalid,       // default value, not a valid component
               
        Transform,          // transform component for local data
        Camera,             // contains the camera for viewing
        FPSCamera,          // Move the camera component with FPS controls
        TPCamera,           // Camera that hovers over a controlled object (3rd Person)
        Mesh,               // Add a mesh shape on the game object to render
        Model,              // Add a model to the game object to render
        Animator,           // Adds an animation controller to the game object
        RigidBody,          // Adds a physics object to the game object
        PlayerController,   // Input used to move a character/ mesh/ model in the world
        ZombieController,   // Camera & Input controls for the zombie type character
        HumanEnemy,         // Logic for a Human Enemy Character (If time permits)
        SoundEffect,        // Adds a sound effect to an object
        SoundBank,          // Adds a group of sounds to an object
        UIText,             // Adds a UI Text component
        UISprite,           // Adds a UI sprite Component
        UIButton,           // Adds a UI button component 
        Count               // last value, can be used to chain custom components
    };

    enum class ServiceId
    {
        Invalid,            // Default value, not valid Id
        Camera,				// Manages the cameras in the world
        Render,             // Renders the renderobjects into the world
        Physics,            // Registers & monitors physics objects
        UIRender,           // Renders UI components
        Count               // Last value, can be used to chain custom services
    };
}

#define SET_TYPE_ID(id)\
    static uint32_t StaticGetTypeId() { return static_cast<uint32_t>(id); }\
    uint32_t GetTypeId() const override { return StaticGetTypeId(); }
