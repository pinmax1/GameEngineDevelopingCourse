#pragma once

#include <RenderObject.h>
#include <RenderThread.h>
#include <Vector.h>
#include <Input/InputHandler.h>

namespace GameEngine
{
	class GameObject
	{
	public:
		GameObject() = default;

		GameObject(Math::Vector3f pos) {
			m_Position = pos;
		}

	public:
		Render::RenderObject** GetRenderObjectRef() { return &m_RenderObject; }

		void SetPosition(Math::Vector3f position, size_t frame)
		{
			m_Position = position;

			if (m_RenderObject) [[likely]]
				{
					m_RenderObject->SetPosition(position, frame);
				}
		}

		Math::Vector3f GetPosition()
		{
			return m_Position;
		}

		virtual void Update(float dt) {};

	protected:
		Render::RenderObject* m_RenderObject = nullptr;

		Math::Vector3f m_Position = Math::Vector3f::Zero();
	};

	class MovingGameObject : public GameObject {
	public:
		MovingGameObject() {
			Core::g_InputHandler->RegisterCallback("GoForwardCube", [&]() { Move(Math::Vector3f(0, 0, 1.0f)); });
			Core::g_InputHandler->RegisterCallback("GoBackCube", [&]() { Move(Math::Vector3f(0, 0, -1.0f)); });
			Core::g_InputHandler->RegisterCallback("GoRightCube", [&]() { Move(Math::Vector3f(1.0f, 0, 0)); });
			Core::g_InputHandler->RegisterCallback("GoLeftCube", [&]() { Move(Math::Vector3f(-1.0f, 0, 0)); });
		}

		MovingGameObject(Math::Vector3f pos): MovingGameObject() {
			m_Position = pos;
		}

		void Move(Math::Vector3f dir) {
			m_CurrentMoveDir = m_CurrentMoveDir + dir;
		}

		void Update(float dt) override {
			m_Position = m_Position + m_CurrentMoveDir.Normalized() * m_MovementSpeed * dt;
			m_CurrentMoveDir = Math::Vector3f::Zero();
		}
	private:
		Math::Vector3f m_CurrentMoveDir = Math::Vector3f::Zero();
		float m_MovementSpeed = 10.0f;
	};

	class JumpingGameObject : public GameObject {
	public:
		JumpingGameObject() {
			Core::g_InputHandler->RegisterCallback("Jump", [&]() { Jump(); });
		}

		JumpingGameObject(Math::Vector3f pos) : JumpingGameObject() {
			m_Position = pos;
		}

		void Jump() {
			if (!m_isJump) {
				m_speed = 10.0f;
				m_isJump = true;
			}
		}

		void Update(float dt) override {
			m_Position.y += m_speed * dt;
			m_speed -= m_gravity * dt;
			if (m_Position.y < 0) {
				m_Position.y = 0;
				m_isJump = false;
			}
		}
	private:
		const float m_gravity = 10.0f;
		float m_speed = 0;
		bool m_isJump = false;
	};

	class SelfMovingGameObject : public GameObject {
	public:
		SelfMovingGameObject() = default;

		SelfMovingGameObject(Math::Vector3f pos) {
			m_Position = pos;
			m_beginPosition = m_Position;
		}

		void Update(float dt) override {
			m_Position = m_Position + Math::Vector3f(0, 0, 1.0f) * dir * speed * dt;
			if (abs(m_Position.z - m_beginPosition.z) > 1.0f) {
				dir *= -1.0f;
			}
		}
	private:
		Math::Vector3f m_beginPosition;
		float speed = 5.0f;
		float dir = 1.0f;
	};

}