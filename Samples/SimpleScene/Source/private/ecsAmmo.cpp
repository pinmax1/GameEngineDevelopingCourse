#include <ecsAmmo.h>
#include <ecsMesh.h>
#include <ecsPhys.h>
#include <ECS/ecsSystems.h>
#include <flecs.h>
#include <Geometry.h>
#include <RenderThread.h>
#include <RenderObject.h>
#include <Vector.h>

using namespace GameEngine;

void RegisterEcsAmmoSystems(flecs::world& world)
{


	static const EntitySystem::ECS::RenderThreadPtr* renderThread = world.get<EntitySystem::ECS::RenderThreadPtr>();


	world.system<BulletLifetime, RenderObjectPtr, Position>()
		.with<Bullet>()
		.each([&](flecs::entity e, BulletLifetime& bulletLifeTime, RenderObjectPtr& ptr, Position& pos)
			{
				bulletLifeTime.lifetime -= world.delta_time();
				if (bulletLifeTime.lifetime < 0.0f) {
					pos.value = Math::Vector3f(1000.0f, 1000.0f, 1000.0f);

				}
			});

	world.system<Position>()
		.with<Bullet>()
		.each([&](flecs::entity e, Position& pos)
			{
				world.each([&](flecs::entity other, Position& otherPos, CollisionBox& otherBox) {
					if ((std::abs(pos.value.x - otherPos.value.x) <= 1.1f) &&
						(std::abs(pos.value.y - otherPos.value.y) <= 1.1f) &&
						(std::abs(pos.value.z - otherPos.value.z) <= 1.1f)) {
						otherPos.value = Math::Vector3f(2000.0f, 2000.0f, 2000.0f);
						if (other.has<BonusBullets>()) {
							flecs::entity camera = world.lookup("camera");
;							Ammo* ammo = camera.get_mut<Ammo>();
							ammo->count += other.get_mut<BonusBullets>()->count;
						}
					}
					});
			});
}