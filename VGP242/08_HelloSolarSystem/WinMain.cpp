#include "GameState.h"

using namespace IExeEngine;

int WINAPI WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int)
{
	AppConfig config;
	config.appName = L"Hello Solar System";

	config.winWidth = 1920;
	config.winHeight = 1080;

	App& myApp = MainApp();

	myApp.AddState<GameState>("GameState");

	myApp.Run(config);

	return 0;
}