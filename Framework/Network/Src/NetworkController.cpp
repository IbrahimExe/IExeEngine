#include "Precompiled.h"
#include "NetworkController.h"

using namespace IExeEngine;
using namespace IExeEngine::Network;


void NetworkController::SetPosition(const Math::Vector3& position)
{
	mPosition = position;
	mNewPosition = true;
}

bool NetworkController::TryGetNewPosition(Math::Vector3& newPosition)
{
	if (mNewPosition)
	{
		newPosition = mPosition;
		mNewPosition = false;
		return true;
	}
	return false;
}

void NetworkController::SetInput(const EventInput& input)
{
	mInputIsDirty = mInputIsDirty || input.moveX != mInput.moveX;
	mInputIsDirty = mInputIsDirty || input.moveY != mInput.moveY;
	mInputIsDirty = mInputIsDirty || input.jump != mInput.jump;
	mInputIsDirty = mInputIsDirty || input.shiftSpeed != mInput.shiftSpeed;
	mInput = input;
}

bool NetworkController::TryGetDirtyInput(EventInput& input)
{
	if (mInputIsDirty)
	{
		input = mInput;
		mInputIsDirty = false;
		return true;
	}
	return false;
}

const EventInput& NetworkController::GetInput() const
{
	return mInput;
}