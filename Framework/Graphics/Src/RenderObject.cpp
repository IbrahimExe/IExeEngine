#include "Precompiled.h"
#include "RenderObject.h"

using namespace IExeEngine;
using namespace IExeEngine::Graphics;

void RenderObject::Terminate()
{
    meshBuffer.Terminate();
    TextureManager* tm = TextureManager::Get();
    tm->ReleaseTexture(diffuseMapId);
    tm->ReleaseTexture(specMapId);
    tm->ReleaseTexture(normalMapId);
    tm->ReleaseTexture(bumpMapId);
}