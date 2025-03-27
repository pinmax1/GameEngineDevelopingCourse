#pragma once

#include <flecs.h>
#include <RenderThread.h>

struct Timer
{
	float leftTime;
};


void RegisterEcsMeshSystems(flecs::world& world);
