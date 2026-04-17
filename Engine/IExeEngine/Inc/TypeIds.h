#pragma once

namespace IExeEngine
{
    enum class ComponentId 
    {
        Invalid,    // default value, not a valid component
        Transform,  // transform component for local data
        Count       // last value, can be used to chain custom components

    };
}

#define SET_TYPE_ID(id)\
    static uint32_t StaticGetTypeId() { return static_cast<uint32_t>(id); }\
    uint32_t GetTypeId() const override { return StaticGetTypeId(); }
