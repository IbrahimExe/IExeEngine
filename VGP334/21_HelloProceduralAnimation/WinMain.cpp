#include "GameState.h"

using namespace IExeEngine;

int WINAPI WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int)
{
	AppConfig config;
	config.appName = L"Hello Procedural Generation";

	App& myApp = MainApp();

	myApp.AddState<GameState>("GameState");

	myApp.Run(config);

	return 0;
}