#include "Precompiled.h"
#include "PhysicsDebugDraw.h"

using namespace IExeEngine;
using namespace IExeEngine::Physics;
using namespace IExeEngine::Graphics;

void PhysicsDebugDraw::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
{
    SimpleDraw::AddLine(ToVector3(from), ToVector3(to), ToColor(color));
}

void PhysicsDebugDraw::drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
{
    SimpleDraw::AddLine(ToVector3(PointOnB), ToVector3(PointOnB + normalOnB * distance), ToColor(color));
}

void PhysicsDebugDraw::reportErrorWarning(const char* warningString)
{
    LOG("[PhysicsErrorWarning] %s", warningString);
}

void PhysicsDebugDraw::draw3dText(const btVector3& location, const char* textString)
{
    // Not Implemented as I don't have a way to draw 3D text in this framework
}

void PhysicsDebugDraw::setDebugMode(int debugMode)
{
    mDebugMode = debugMode;
}

int PhysicsDebugDraw::getDebugMode() const
{
    return mDebugMode;
}
