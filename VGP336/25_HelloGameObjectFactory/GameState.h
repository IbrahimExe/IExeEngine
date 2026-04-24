#pragma once

#include <IExeEngine/Inc/IExeEngine.h>

class GameState : public IExeEngine::AppState
{
public:
	void Initialize() override;

	void Terminate() override;
	
	void Update(float deltaTime) override;

	void Render() override;

	void DebugUI() override;

private:
    IExeEngine::GameWorld mGameWorld;
};