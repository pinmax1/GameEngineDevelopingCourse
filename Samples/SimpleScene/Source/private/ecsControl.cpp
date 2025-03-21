#include <Camera.h>
#include <ecsControl.h>
#include <ECS/ecsSystems.h>
#include <ecsPhys.h>
#include <flecs.h>
#include <Input/Controller.h>
#include <Input/InputHandler.h>
#include <Vector.h>
#include <DefaultGeometry.h>
#include <RenderObject.h>

using namespace GameEngine;

void RegisterEcsControlSystems(flecs::world& world)
{
	world.system<Position, CameraPtr, WasPressedLbm, const Speed, const ControllerPtr>()
		.each([&](flecs::entity e, Position& position, CameraPtr& camera, WasPressedLbm& wasPressed, const Speed& speed, const ControllerPtr& controller)
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
		position.x = position.x + currentMoveDir.Normalized().x * speed.value * world.delta_time();
		position.y = position.y + currentMoveDir.Normalized().y * speed.value * world.delta_time();
		position.z = position.z + currentMoveDir.Normalized().z * speed.value * world.delta_time();
		camera.ptr->SetPosition(Math::Vector3f(position.x, position.y, position.z));
		if (controller.ptr->IsPressed("Shoot") && !wasPressed.flag)
		{
			flecs::entity bullet = world.entity()
				.set(Position{position.x, position.y, position.z})
				.set(Velocity{ camera.ptr->GetViewDir().x * 5.0f, camera.ptr->GetViewDir().y * 5.0f, camera.ptr->GetViewDir().z * 5.0f })
				.set(Gravity{ 0.f, -9.8065f, 0.f })
				.set(BouncePlane{ 0.f, 1.f, 0.f, 5.f })
				.set(Bounciness{ 0.7f })
				.set(EntitySystem::ECS::GeometryPtr{ RenderCore::DefaultGeometry::Platform(0.1f, 0.1f, 0.1f) })
				.set(EntitySystem::ECS::RenderObjectPtr{ new Render::RenderObject() })
				.set(Health{ 1.0f })
				.set(IsDead{false})
				.set(Size{ 0.1f });
		}
		wasPressed.flag = controller.ptr->IsPressed("Shoot");
	});

	world.system<const Position, Velocity, const ControllerPtr, const BouncePlane, const JumpSpeed>()
		.each([&](const Position& pos, Velocity& vel, const ControllerPtr& controller, const BouncePlane& plane, const JumpSpeed& jump)
	{
		constexpr float planeEpsilon = 0.1f;
		if (plane.x * pos.x + plane.y * pos.y + plane.z * pos.z < plane.w + planeEpsilon)
		{
			if (controller.ptr->IsPressed("Jump"))
			{
				vel.y = jump.value;
			}
		}
	});
}

