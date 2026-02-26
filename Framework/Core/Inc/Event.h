#pragma once

namespace IExeEngine::Core
{
    using EventTypeId = std::size_t;

    class Event
    {
    public:
        Event() = default;
        Event(EventTypeId id) : mEventTypeId(id) {}
        virtual ~Event() = default;

        EventTypeId GetTypeId() const { return mEventTypeId; }

    private:
        EventTypeId mEventTypeId = 0;
    };
}