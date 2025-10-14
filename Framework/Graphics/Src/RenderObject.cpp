#include "Precompiled.h"
#include "RenderObject.h"

using namespace IExeEngine;
using namespace IExeEngine::Graphics;

void RenderObject::Terminate()
{
    meshBuffer.Terminate();
}