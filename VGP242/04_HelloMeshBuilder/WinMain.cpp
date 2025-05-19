#include <IExeEngine/Inc/IExeEngine.h>
#include "ShapeState.h"

int WINAPI WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int)
{
    IExeEngine::AppConfig config;
    config.appName = L"Hello Cube";

    IExeEngine::App& myApp = IExeEngine::MainApp();

    myApp.AddState<ShapeState>("ShapeState");
    myApp.AddState<CubeState>("Cube");
    myApp.AddState<PyramidState>("Pyramid");
    myApp.AddState<RectangleState>("Rectangle");

    myApp.Run(config);

    return 0;
}