#include <Camera.h>
#include <ecsControl.h>
#include <ECS/ecsSystems.h>
#include <ecsPhys.h>
#include <flecs.h>
#include <Input/Controller.h>
#include <Input/InputHandler.h>
#include <Vector.h>
#include <DefaultGeometry.h>
#include <ecsMesh.h>
#include <RenderObject.h>
#include <ecsAmmo.h>

using namespace GameEngine;

void RegisterEcsControlSystems(flecs::world& world)
{
	world.system<Position, CameraPtr, WasPressedLbm, Ammo, const Speed, const ControllerPtr>()
		.each([&](flecs::entity e, Position& position, CameraPtr& camera, WasPressedLbm& wasPressed, Ammo& ammo, const Speed& speed, const ControllerPtr& controller)
	{
		Math::Vector3f currentMoveDir = Math::Vector3f::Zero();
		if (controller.ptr->IsPressed("GoLeft"))
		{
			currentMoveDir = currentMoveDir - camera.ptr->GetRightDir();
		}
		if (controller.ptr->IsPressed("GoRight"))
		{
			currentMoveDir = currentMoveDir + camera.ptr->GetRightDir();
		}
		if (controller.ptr->IsPressed("GoBack"))
		{
			currentMoveDir = currentMoveDir - camera.ptr->GetViewDir();
		}
		if (controller.ptr->IsPressed("GoForward"))
		{
			currentMoveDir = currentMoveDir + camera.ptr->GetViewDir();
		}
		position.value = position.value + currentMoveDir.Normalized() * speed * world.delta_time();
		camera.ptr->SetPosition(position.value);
		if (controller.ptr->IsPressed("Shoot") && !wasPressed.flag && ammo.count > 0)
		{
			flecs::entity bullet = world.entity()
				.set(position)
				.set(Velocity{ camera.ptr->GetViewDir() * 5.0f})
				.set(Gravity{ Math::Vector3f(0.f, -9.8065f, 0.f) })
				.set(BouncePlane{ Math::Vector4f(0.f, 1.f, 0.f, 5.f) })
				.set(Bounciness{ 0.3f })
				.set(GeometryPtr{ RenderCore::DefaultGeometry::Platform(0.1f, 0.1f, 0.1f) })
				.set(RenderObjectPtr{ new Render::RenderObject() })
				.add<Bullet>();
			ammo.count -= 1;
		}
		wasPressed.flag = controller.ptr->IsPressed("Shoot");
		});

	world.system<Ammo, ReloadTimer>()
		.each([&](flecs::entity e, Ammo& ammo, ReloadTimer& reloadTimer)
			{
				if (ammo.count == 0) {
					if (reloadTimer.timeLeft > 0.0f) {
						reloadTimer.timeLeft -= world.delta_time();
					}
					else {
						reloadTimer.timeLeft = 2.0f;
						ammo.count = 6;
					}
				}
			});

	world.system<const Position, Velocity, const ControllerPtr, const BouncePlane, const JumpSpeed>()
		.each([&](const Position& pos, Velocity& vel, const ControllerPtr& controller, const BouncePlane& plane, const JumpSpeed& jump)
	{
		constexpr float planeEpsilon = 0.1f;
		if (plane.value.x * pos.value.x + plane.value.y * pos.value.y + plane.value.z * pos.value.z < plane.value.w + planeEpsilon)
		{
			if (controller.ptr->IsPressed("Jump"))
			{
				vel.value.y = jump.value;
			}
		}
	});
}

