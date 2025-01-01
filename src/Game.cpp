#include <iostream>
#include <algorithm>
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_ttf.h>
#include <chrono>
#include "Game.h"
#include "utils.h"
#include "structs.h"

Game::Game(const Window& window)
	: m_Window{ window }
	, m_Viewport{ 0,0,window.width,window.height }
	, m_pWindow{ nullptr }
	, m_pContext{ nullptr }
	, m_Initialized{ false }
	, m_MaxElapsedSeconds{ 0.1f }
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
	m_HasShiftBeenPressed = false; 
	m_ShouldReflect = false;  
	m_ShouldRotate = false; 

	m_CurrentPillarIndex = 0; 

	m_LeftPlane = OneBlade{ 0, 1, 0, 0 }; 
	m_RightPlane = OneBlade{ -m_Viewport.width, 1, 0, 0 };   

	m_TopPlane = OneBlade{ -m_Viewport.height, 0, 1, 0 }; 
	m_BottomPlane = OneBlade{ 0, 0, 1, 0 };

	m_ViewportPlanes.push_back(m_LeftPlane);
	m_ViewportPlanes.push_back(m_RightPlane);
	m_ViewportPlanes.push_back(m_TopPlane);
	m_ViewportPlanes.push_back(m_BottomPlane);

	m_Player = ThreeBlade{ 200, 200, m_PlayerEnergy, 1 };  
	m_Pillars.push_back(ThreeBlade{ 400, 300, 0, 1 });
	m_Pillars.push_back(ThreeBlade{ 900, 500, 0, 1 });
	m_PlayerVelocity = TwoBlade{ 1, 0, 0, 0, 0, 400.f }; 

	m_PlayerColor = Color4f{ 0.f, 1.f, 0.f, 1.f };
	m_PillarColor = Color4f{ 1.f, 0.f, 1.f, 1.f };
	m_SelectedPillarColor = Color4f{ 1.f, 1.f, 0.f, 1.f }; 
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

void Game::ViewPortCollisionDetection()
{
	for (OneBlade plane : m_ViewportPlanes) 
	{
		const float distance{ ComputeDistance(m_Player, plane) };

		// Check if the current plane is left or right and has reflection logic
		if (plane == OneBlade{ -m_Viewport.width, 1, 0, 0 })  
		{
			if (distance < m_PlayerDimensions)  
			{
				m_PlayerVelocity = (OneBlade{ 0, 1, 0, 0 } * m_PlayerVelocity * ~OneBlade{ 0, 1, 0, 0 }).Grade2(); 
			}
		}
		else if (plane == OneBlade{ -m_Viewport.height, 0, 1, 0 })
		{
			if (distance < m_PlayerDimensions)
			{
				m_PlayerVelocity = (OneBlade{ 0, 0, 1, 0 } * m_PlayerVelocity * ~OneBlade{ 0, 0, 1, 0 }).Grade2();
			}
		}
		else
		{
			if (distance < m_PlayerDimensions) 
			{
				m_PlayerVelocity = (plane * m_PlayerVelocity * ~plane).Grade2();  
			}
		}
	}
}

void Game::UpdatePlayerColor()
{
	// Update the player color 
	Color4f lowEnergyColor = Color4f{ 1.0f, 0.0f, 0.0f, 1.0f }; 
	Color4f highEnergyColor = Color4f{ 0.0f, 1.0f, 0.0f, 1.0f }; 

	// Calculate the energy ratio (0.0 to 1.0)
	float energyRatio = (m_Player[2] - m_PlayerMinEnergy) / (m_PlayerEnergy - m_PlayerMinEnergy); 

	// Interpolate the color based on the energy ratio
	m_PlayerColor.r = lowEnergyColor.r + energyRatio * (highEnergyColor.r - lowEnergyColor.r); 
	m_PlayerColor.g = lowEnergyColor.g + energyRatio * (highEnergyColor.g - lowEnergyColor.g); 
	m_PlayerColor.b = lowEnergyColor.b + energyRatio * (highEnergyColor.b - lowEnergyColor.b); 
}

float Game::ComputeDistance(OneBlade plane, ThreeBlade player)
{
	return abs(plane & player);    
}

float Game::ComputeDistance(ThreeBlade player, OneBlade plane)
{
	return abs(player & plane); 
}

Motor Game::MakeTranslationMotor(TwoBlade velocity, float elapsedSec)   
{
	float translationAmount{ velocity.Norm() * elapsedSec };
	Motor translator{ Motor::Translation(translationAmount, velocity) };

	return translator;
}

TwoBlade Game::RotateVelocity(TwoBlade velocity, ThreeBlade pillar, float angle)
{
	Motor rotation{ Motor::Rotation(angle, TwoBlade{ 0, 0, 0, 0, 0, 1 }) };
	return (rotation * velocity * ~rotation).Grade2();
}

ThreeBlade Game::RotateAroundPillar(ThreeBlade player, ThreeBlade pillar, float angle)
{
	// Normalize the pillar position
	pillar = pillar.Normalize();

	// Translate to origin
	Motor translatorToOrigin{ Motor::Translation(-1 * pillar.VNorm(), TwoBlade{pillar[0], pillar[1], 0, 0, 0, 0}) };
	player = (translatorToOrigin * player * ~translatorToOrigin).Grade3();	

	// Rotate around origin
	Motor rotation{ Motor::Rotation(angle, TwoBlade{ 0, 0, 0, 0, 0, 1 }) };
	player = (rotation * player * ~rotation).Grade3();

	// Translate back
	Motor translatorBack{ Motor::Translation(pillar.VNorm(), TwoBlade{pillar[0], pillar[1], 0, 0, 0, 0}) };
	player = (translatorBack * player * ~translatorBack).Grade3();

	return player;
}

void Game::Update(float elapsedSec)
{
	if (m_ShouldRotate)
	{
		const float rotationAngle{ 45.f * elapsedSec }; 
		m_Player = RotateAroundPillar(m_Player, m_Pillars[m_CurrentPillarIndex], rotationAngle);

		m_PlayerVelocity = RotateVelocity(m_PlayerVelocity, m_Pillars[m_CurrentPillarIndex], rotationAngle); 
	}
	else if (m_ShouldReflect)  
	{
		m_Player = (-m_Pillars[m_CurrentPillarIndex] * -MakeTranslationMotor(m_PlayerVelocity, elapsedSec) * m_Player * ~MakeTranslationMotor(m_PlayerVelocity, elapsedSec) * ~m_Pillars[m_CurrentPillarIndex]).Grade3();
		m_Player[2] *= -1;
		m_ShouldReflect = false; 
	}
	else
	{
		m_Player = (MakeTranslationMotor(m_PlayerVelocity, elapsedSec) * m_Player * ~MakeTranslationMotor(m_PlayerVelocity, elapsedSec)).Grade3();
	}

	// Check for collision with the viewport
	ViewPortCollisionDetection();   

	// Update the player color
	UpdatePlayerColor(); 

	// Update cooldown timer if active
	if (m_CooldownTimer > 0.0f)
	{
		m_CooldownTimer -= elapsedSec;
	}

	// Increase or decrease Player's energy
	if (m_HasShiftBeenPressed)
	{
		if (m_Player[2] > m_PlayerMinEnergy)
		{
			m_Player[2] = std::max(m_PlayerMinEnergy, m_Player[2] - m_EnergyDrainSpeed * elapsedSec);
		}
		else
		{
			m_HasShiftBeenPressed = false;
			m_PlayerVelocity /= 2;
			m_CooldownTimer = m_CooldownDuration; 
		}
	}
	else
	{
		if (m_Player[2] < m_PlayerEnergy)
		{
			m_Player[2] = std::min(m_Player[2] + m_EnergyDrainSpeed * elapsedSec, m_PlayerEnergy);
		}
	}
} 

void Game::Draw() const
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	utils::SetColor(m_PlayerColor);
	utils::FillRect(m_Player[0], m_Player[1], m_PlayerDimensions, m_PlayerDimensions);

	for (size_t idx = 0; idx < m_Pillars.size(); ++idx)
	{
		if (idx == m_CurrentPillarIndex)
		{
			utils::SetColor(m_SelectedPillarColor); 
		}
		else
		{
			utils::SetColor(m_PillarColor);
		}

		utils::FillRect(m_Pillars[idx][0], m_Pillars[idx][1], m_PillarDimensions, m_PillarDimensions);
	}
}
