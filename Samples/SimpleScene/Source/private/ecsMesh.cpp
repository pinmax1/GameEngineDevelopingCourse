#include <ecsMesh.h>
#include <ecsPhys.h>
#include <ecsMesh.h>
#include <ecsPhys.h>
#include <ECS/ecsSystems.h>
#include <flecs.h>
#include <Geometry.h>
#include <RenderThread.h>
#include <RenderObject.h>
#include <DefaultGeometry.h>

using namespace GameEngine;

void RegisterEcsMeshSystems(flecs::world& world)
{
	static const EntitySystem::ECS::RenderThreadPtr* renderThread = world.get<EntitySystem::ECS::RenderThreadPtr>();
	world.system<EntitySystem::ECS::RenderObjectPtr, const Position>()
		.each([&](EntitySystem::ECS::RenderObjectPtr& renderObject, const Position& position)
	{
		renderObject.ptr->SetPosition(Math::Vector3f(position.x, position.y, position.z), renderThread->ptr->GetMainFrame());
	});

	world.system<EntitySystem::ECS::RenderObjectPtr, const Timer>()
		.each([&](flecs::entity e, EntitySystem::ECS::RenderObjectPtr& renderObject, const Timer& timer)
	{
		if (timer.leftTime < 0.0f) {
			renderThread->ptr->EnqueueCommand<RenderCore::Geometry*, Render::RenderObject*>(Render::ERC::CreateRenderObject, RenderCore::DefaultGeometry::Cube(), renderObject.ptr);
			e.destruct();
		}
	});
}


