#pragma once

#include <Level.h>
#include <flecs.h>

#include <EditorECS/ecsEditor.h>

namespace GameEngine::EntitySystem::LevelEditorECS
{
	struct PositionDesc
	{
		size_t objectId;
		size_t componentId;
	};

	void RegisterLevelEditorEcsSystems(flecs::world& world, World::Level& level);
}