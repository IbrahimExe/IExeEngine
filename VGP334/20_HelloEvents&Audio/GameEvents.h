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
    PressSpaceEvent() {}
    SET_EVENT_TYPE_ID(GameEventType::PressSpace)
};

class PressEnterEvent : public IExeEngine::Core::Event
{
public:
    PressEnterEvent() {}
    SET_EVENT_TYPE_ID(GameEventType::PressEnter)
};