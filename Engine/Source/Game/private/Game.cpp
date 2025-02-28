#include <Camera.h>
#include <DefaultGeometry.h>
#include <Game.h>
#include <GameObject.h>
#include <Input/InputHandler.h>

namespace GameEngine
{
	Game::Game(
		std::function<bool()> PlatformLoopFunc
	) :
		PlatformLoop(PlatformLoopFunc)
	{
		Core::g_MainCamera = new Core::Camera();
		Core::g_MainCamera->SetPosition(Math::Vector3f(0.0f, 6.0f, -6.0f));
		Core::g_MainCamera->SetViewDir(Math::Vector3f(0.0f, -6.0f, 6.0f).Normalized());

		m_renderThread = std::make_unique<Render::RenderThread>();
		m_platform = new GameObject(Math::Vector3f(0,-2.0f,0));
		Render::RenderObject** renderObject = m_platform->GetRenderObjectRef();
		m_renderThread->EnqueueCommand(Render::ERC::CreateRenderObject, RenderCore::DefaultGeometry::Platform(30.0f, 1.0f, 30.0f), renderObject);
		srand(time(0));
		// How many objects do we want to create
		float x = -20.0f;
		float z = -20.0f;
		for (int i = 0; i < 100; ++i)
		{

			int type = rand() % 3;
			if (type == 0) {
				m_Objects.push_back(new MovingGameObject(Math::Vector3f(x, 0, z)));
			}
			else if (type == 1) {
				m_Objects.push_back(new JumpingGameObject(Math::Vector3f(x, 0, z)));
			}
			else if (type == 2) {
				m_Objects.push_back(new SelfMovingGameObject(Math::Vector3f(x, 0, z)));
			}
			renderObject = m_Objects.back()->GetRenderObjectRef();
			m_renderThread->EnqueueCommand(Render::ERC::CreateRenderObject, RenderCore::DefaultGeometry::Cube(), renderObject);
			x += 4.0f;
			if (x > 16.0f) {
				x = -20.0f;
				z += 4.0f;
			}
		}

		Core::g_InputHandler->RegisterCallback("GoForward", [&]() { Core::g_MainCamera->Move(Core::g_MainCamera->GetViewDir()); });
		Core::g_InputHandler->RegisterCallback("GoBack", [&]() { Core::g_MainCamera->Move(-Core::g_MainCamera->GetViewDir()); });
		Core::g_InputHandler->RegisterCallback("GoRight", [&]() { Core::g_MainCamera->Move(Core::g_MainCamera->GetRightDir()); });
		Core::g_InputHandler->RegisterCallback("GoLeft", [&]() { Core::g_MainCamera->Move(-Core::g_MainCamera->GetRightDir()); });
	}

	void Game::Run()
	{
		assert(PlatformLoop != nullptr);

		m_GameTimer.Reset();

		bool quit = false;
		while (!quit)
		{
			m_GameTimer.Tick();
			float dt = m_GameTimer.GetDeltaTime();

			Core::g_MainWindowsApplication->Update();
			Core::g_InputHandler->Update();
			Core::g_MainCamera->Update(dt);

			Update(dt);

			m_renderThread->OnEndFrame();

			// The most common idea for such a loop is that it returns false when quit is required, or true otherwise
			quit = !PlatformLoop();
		}
	}

	void Game::Update(float dt)
	{
		for (int i = 0; i < m_Objects.size(); ++i)
		{
			m_Objects[i]->Update(dt);
			m_Objects[i]->SetPosition(m_Objects[i]->GetPosition(), m_renderThread->GetMainFrame());
			m_platform->Update(dt);
			m_platform->SetPosition(m_platform->GetPosition(), m_renderThread->GetMainFrame());
		}
	}
}