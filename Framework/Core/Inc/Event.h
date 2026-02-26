#pragma once

namespace IExeEngine::Core
{
    using EventTypeId = std::size_t;

    class Event
    {
    public:
        Event() = default;
        virtual ~Event() = default;

        virtual EventTypeId GetTypeId() const = 0;
    };
}

#define SET_EVENT_TYPE_ID(id)\
static IExeEngine::Core::EventTypeId GetStaticTypeId() { return static_cast<IExeEngine::Core::EventTypeId>(id); }
IExeEngine::Core::EventTypeId GetTypeId() const override { return StaticGetTypeId(); }