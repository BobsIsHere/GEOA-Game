#include <iostream>
#include <algorithm>
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_ttf.h>
#include <chrono>
#include "Game.h"
#include "utils.h"
#include "structs.h"

Game::Game(const Window& window) :
	m_Window{ window },
	m_Viewport{ 0, 0, window.width, window.height },
	m_pWindow{ nullptr },
	m_pContext{ nullptr },
	m_Initialized{ false },
	m_MaxElapsedSeconds{ 0.1f },
	m_Player{ 200.f, 200.f, Point2f{ window.width, window.height } },
	m_Pickup{ Point2f{ window.width, window.height } },
	m_Enemy{ ThreeBlade{ 400.f, 400.f, 0.f }, Point2f{ window.width, window.height } } 
{
	InitializeGameEngine();
	InitializeGameVariables();
}

Game::~Game()
{
	CleanupGameEngine();
}

void Game::InitializeGameEngine()
{
	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::cerr << "BaseGame::Initialize( ), error when calling SDL_Init: " << SDL_GetError() << std::endl;
		return;
	}

	// Use OpenGL 2.1
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

	// Create window
	m_pWindow = SDL_CreateWindow(
		m_Window.title.c_str(),
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		int(m_Window.width),
		int(m_Window.height),
		SDL_WINDOW_OPENGL);
	if (m_pWindow == nullptr)
	{
		std::cerr << "BaseGame::Initialize( ), error when calling SDL_CreateWindow: " << SDL_GetError() << std::endl;
		return;
	}

	// Create OpenGL context 
	m_pContext = SDL_GL_CreateContext(m_pWindow);
	if (m_pContext == nullptr)
	{
		std::cerr << "BaseGame::Initialize( ), error when calling SDL_GL_CreateContext: " << SDL_GetError() << std::endl;
		return;
	}

	// Set the swap interval for the current OpenGL context,
	// synchronize it with the vertical retrace
	if (m_Window.isVSyncOn)
	{
		if (SDL_GL_SetSwapInterval(1) < 0)
		{
			std::cerr << "BaseGame::Initialize( ), error when calling SDL_GL_SetSwapInterval: " << SDL_GetError() << std::endl;
			return;
		}
	}
	else
	{
		SDL_GL_SetSwapInterval(0);
	}

	// Set the Projection matrix to the identity matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Set up a two-dimensional orthographic viewing region.
	glOrtho(0, m_Window.width, 0, m_Window.height, -1, 1); // y from bottom to top

	// Set the viewport to the client window area
	// The viewport is the rectangular region of the window where the image is drawn.
	glViewport(0, 0, int(m_Window.width), int(m_Window.height));

	// Set the Modelview matrix to the identity matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Enable color blending and use alpha blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Initialize SDL_ttf
	if (TTF_Init() == -1)
	{
		std::cerr << "BaseGame::Initialize( ), error when calling TTF_Init: " << TTF_GetError() << std::endl;
		return;
	}

	m_Initialized = true;
}

void Game::InitializeGameVariables()
{
	m_LeftPlane = OneBlade{ 0, 1, 0, 0 }; 
	m_RightPlane = OneBlade{ -m_Viewport.width, 1, 0, 0 };   

	m_TopPlane = OneBlade{ -m_Viewport.height, 0, 1, 0 }; 
	m_BottomPlane = OneBlade{ 0, 0, 1, 0 };

	m_ViewportPlanes.push_back(m_LeftPlane);
	m_ViewportPlanes.push_back(m_RightPlane);
	m_ViewportPlanes.push_back(m_TopPlane);
	m_ViewportPlanes.push_back(m_BottomPlane);
}

void Game::Run()
{
	if (!m_Initialized)
	{
		std::cerr << "BaseGame::Run( ), BaseGame not correctly initialized, unable to run the BaseGame\n";
		std::cin.get();
		return;
	}

	// Main loop flag
	bool quit{ false };

	// Set start time
	std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();

	//The event loop
	SDL_Event e{};
	while (!quit)
	{
		// Poll next event from queue
		while (SDL_PollEvent(&e) != 0)
		{
			// Handle the polled event
			switch (e.type)
			{
			case SDL_QUIT:
				quit = true;
				break;
			case SDL_KEYDOWN:
				this->ProcessKeyDownEvent(e.key);
				break;
			case SDL_KEYUP:
				this->ProcessKeyUpEvent(e.key);
				break;
			case SDL_MOUSEMOTION:
				e.motion.y = int(m_Window.height) - e.motion.y;
				this->ProcessMouseMotionEvent(e.motion);
				break;
			case SDL_MOUSEBUTTONDOWN:
				e.button.y = int(m_Window.height) - e.button.y;
				this->ProcessMouseDownEvent(e.button);
				break;
			case SDL_MOUSEBUTTONUP:
				e.button.y = int(m_Window.height) - e.button.y;
				this->ProcessMouseUpEvent(e.button);
				break;
			}
		}

		if (!quit)
		{
			// Get current time
			std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();

			// Calculate elapsed time
			float elapsedSeconds = std::chrono::duration<float>(t2 - t1).count();

			// Update current time
			t1 = t2;

			// Prevent jumps in time caused by break points
			elapsedSeconds = std::min(elapsedSeconds, m_MaxElapsedSeconds);

			// Call the BaseGame object 's Update function, using time in seconds (!)
			this->Update(elapsedSeconds);

			// Draw in the back buffer
			this->Draw();

			// Update screen: swap back and front buffer
			SDL_GL_SwapWindow(m_pWindow);
		}
	}
}

void Game::CleanupGameEngine()
{
	SDL_GL_DeleteContext(m_pContext);

	SDL_DestroyWindow(m_pWindow);
	m_pWindow = nullptr;

	//Quit SDL subsystems
	TTF_Quit();
	SDL_Quit();
}

void Game::ViewPortCollisionDetection(Entity& entity)
{
	const TwoBlade& entityDirection{ entity.GetMovementDirection() }; 
	const ThreeBlade& entityPosition{ entity.GetPosition() };

	float distance{}; 

	for (OneBlade plane : m_ViewportPlanes)
	{
		if (plane == m_LeftPlane || plane == m_RightPlane)
		{
			if (entityDirection[0] <= 0) 
			{
				distance = utils::ComputeDistance(entityPosition, plane);
			}
			else
			{
				const float right{ entityPosition[0] + entity.GetDimensions() };
				const ThreeBlade rightPos{ right, entityPosition[1], entityPosition[2] }; 

				distance = utils::ComputeDistance(rightPos, plane);
			}

			entity.LeftRightPlaneCollisions(m_LeftPlane, distance); 
		}
		else if (plane == m_TopPlane || plane == m_BottomPlane)
		{
			if (entityDirection[1] <= 0) 
			{
				distance = utils::ComputeDistance(entityPosition, plane);
			}
			else
			{
				const float top{ entityPosition[1] + entity.GetDimensions() };
				const ThreeBlade topPos{ entityPosition[1], top, entityPosition[2] }; 

				distance = utils::ComputeDistance(topPos, plane);
			}

			entity.TopBottomPlaneCollisions(m_BottomPlane, distance); 
		}
	}
}

void Game::Update(float elapsedSec)
{
	// Check for collision with the viewport
	ViewPortCollisionDetection(m_Player);   
	ViewPortCollisionDetection(m_Enemy);   

	// Get current pillar index
	const int currentPillarIndex{ m_Player.GetCurrentPillarIndex() };
	const ThreeBlade currentPillarPos{ PillarManager::GetInstance().GetPillarPosition(currentPillarIndex) }; 

	// Update the player
	m_Player.Update(elapsedSec, currentPillarPos); 

	// Update Enemy
	m_Enemy.Update(elapsedSec, m_Player); 

	// Update the pickup
	m_Pickup.Update(elapsedSec, m_Player);
} 

void Game::Draw() const
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	m_Player.Draw();
	m_Enemy.Draw();  
	m_Pickup.Draw();

	PillarManager::GetInstance().Draw(m_Player.GetCurrentPillarIndex());
}
