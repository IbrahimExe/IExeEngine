#pragma once

namespace IExeEngine
{
    class GameObjectHandle
    {
    public:
        GameObjectHandle() = default;

    private:
        friend class GameWorld;
        int mIndex = -1; // Is going to be the index of the slot in the game world
        int mGeneration = -1; // to verify if item is already tagged for deletion/ removal
    };
}
