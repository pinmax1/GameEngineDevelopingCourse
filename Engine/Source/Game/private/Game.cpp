#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
#include <WindowsX.h>
#include <wrl.h>

#include <Camera.h>
#include <DefaultGeometry.h>
#include <Game.h>
#include <GameObject.h>
#include <INIReader.h>

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

		// How many objects do we want to create
		for (int i = 0; i < 3; ++i)
		{
			m_Objects.push_back(new GameObject());
			Render::RenderObject** renderObject = m_Objects.back()->GetRenderObjectRef();
			m_renderThread->EnqueueCommand(Render::ERC::CreateRenderObject, RenderCore::DefaultGeometry::Cube(), renderObject);
		}
		INIReader reader("../../../../../Controls.ini");
		UpButton = reader.Get("Controls", "Up", "UNKNOWN")[0];
		DownButton = reader.Get("Controls", "Down", "UNKNOWN")[0];
		
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

			Update(dt);
			
			// The most common idea for such a loop is that it returns false when quit is required, or true otherwise
			quit = !PlatformLoop();

			m_renderThread->OnEndFrame();
		}
	}

	void Game::Update(float dt)
	{
		for (int i = 0; i < m_Objects.size(); ++i)
		{
			Math::Vector3f pos = m_Objects[i]->GetPosition();
			float direction = 0;
			if (GetAsyncKeyState(DownButton))
			{
				direction = 1.0f;
			}
			else if (GetAsyncKeyState(UpButton)) {
				direction = -1.0f;
			}
			if (i == 0)
			{
				pos.x += 0.5f * dt * direction;
			}
			else if (i == 1)
			{
				pos.y -= 0.5f * dt * direction;
			}
			else if (i == 2)
			{
				pos.x += 0.5f * dt * direction;
				pos.y -= 0.5f * dt * direction;
			}
			m_Objects[i]->SetPosition(pos, m_renderThread->GetMainFrame());
		}
	}
}