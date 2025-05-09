#include <engine/inc/engine.h>
#include "shape_state.h"

int WINAPI WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int)
{
    Engine::AppConfig config;
    config.appName = L"Hello Shapes";
    config.winWidth = 1200;
    config.winHeight = 720;

    Engine::App& myApp = Engine::MainApp();
    myApp.AddState<ShapeState>("ShapeState");
    myApp.AddState<TriForce>("TriForce");
    myApp.AddState<Crystal>("Crystal");
    myApp.AddState<Heart>("Heart");
    myApp.Run(config);

    return 0;
}
