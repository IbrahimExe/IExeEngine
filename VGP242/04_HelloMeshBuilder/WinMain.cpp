#include <IExeEngine/Inc/IExeEngine.h>
#include "ShapeState.h"

int WINAPI WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int)
{
    IExeEngine::AppConfig config;
    config.appName = L"Hello MeshBuilder";

    IExeEngine::App& myApp = IExeEngine::MainApp();

    myApp.AddState<ShapeState>("ShapeState");

    myApp.Run(config);

    return 0;
}