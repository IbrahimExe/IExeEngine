#pragma once

#include <IExeEngine/Inc/IExeEngine.h>

enum class GameEventType
{
    PressSpace = 1,

    PressEnter
};

class PressSpaceEvent : public IExeEngine::Core::Event
{
public:
    PressSpaceEvent() 
        : Event(static_cast<IExeEngine::Core::EventTypeId>(GameEventType::PressSpace))
    {

    }
};

class PressEnterEvent : public IExeEngine::Core::Event
{
public:
    PressEnterEvent()
        : Event(static_cast<IExeEngine::Core::EventTypeId>(GameEventType::PressEnter))
    {

    }
};