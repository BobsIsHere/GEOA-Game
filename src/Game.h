#pragma once
#include <vector>
#include "FlyFish.h"
#include "structs.h"
#include "SDL.h"
#include "SDL_opengl.h"

class Game
{
public:
	explicit Game(const Window& window);
	Game(const Game& other) = delete;
	Game& operator=(const Game& other) = delete;
	Game(Game&& other) = delete;
	Game& operator=(Game&& other) = delete;

	~Game();

	void Run();

	void Update(float elapsedSec);

	void Draw() const;

	// Event handling
	void ProcessKeyDownEvent(const SDL_KeyboardEvent& e)
	{
		if (e.keysym.sym == SDLK_LSHIFT) 
		{
			if (!m_HasShiftBeenPressed && m_CooldownTimer <= 0.0f && m_Player[2] > m_PlayerMinEnergy) 
			{
				m_HasShiftBeenPressed = true;
				m_PlayerVelocity *= 2;
			}
		}

		if (e.keysym.sym == SDLK_SPACE) 
		{
			m_ShouldReflect = !m_ShouldReflect;
		}

		if (e.keysym.sym == SDLK_r)
		{
			m_ShouldRotate = !m_ShouldRotate; 
		}

		if (e.keysym.sym == SDLK_q)
		{
			// Switch between pillars
			if (!m_Pillars.empty()) 
			{
				// Increment and wrap around
				m_CurrentPillarIndex = (m_CurrentPillarIndex + 1) % m_Pillars.size();
			}
		}
	}
	void ProcessKeyUpEvent(const SDL_KeyboardEvent& e)
	{
		if (e.keysym.sym == SDLK_LSHIFT)
		{
			if (m_HasShiftBeenPressed)
			{
				m_HasShiftBeenPressed = false;
				m_PlayerVelocity /= 2;
			}
		}
	}
	void ProcessMouseMotionEvent(const SDL_MouseMotionEvent& e)
	{
		
	}
	void ProcessMouseDownEvent(const SDL_MouseButtonEvent& e)
	{
		
	}
	void ProcessMouseUpEvent(const SDL_MouseButtonEvent& e)
	{
		
	}

	const Rectf& GetViewPort() const
	{
		return m_Viewport;
	}

private:
	// DATA MEMBERS
	// The window properties
	const Window m_Window;
	const Rectf m_Viewport;
	// The window we render to
	SDL_Window* m_pWindow;
	// OpenGL context
	SDL_GLContext m_pContext;
	// Init info
	bool m_Initialized;
	// Prevent timing jumps when debugging
	const float m_MaxElapsedSeconds;

	bool m_HasShiftBeenPressed;
	bool m_ShouldReflect;
	bool m_ShouldRotate;

	int m_CurrentPillarIndex; 

	const float m_PlayerDimensions = 40.f; 
	const float m_PillarDimensions = 20.f;

	const float m_PlayerMinEnergy = 0.f;
	const float m_PlayerMaxEnergy = 100.0f;
	const float m_EnergyDrainSpeed = 60.f;
	const float m_CooldownDuration = 1.0f;

	float m_PlayerVelocity;
	float m_CooldownTimer;

	std::vector<ThreeBlade> m_Pillars; 
	ThreeBlade m_Player;

	Color4f m_PlayerColor; 
	Color4f m_PillarColor; 
	Color4f m_SelectedPillarColor; 
	
	// FUNCTIONS
	void InitializeGameEngine();
	void InitializeGameVariables();
	void CleanupGameEngine();
	void ViewPortCollisionDetection();

	void UpdatePlayerColor();

	Motor MakeTranslationMotor(float velocity, float elapsedSec);
	ThreeBlade RotateAroundPillar(ThreeBlade player, ThreeBlade pillar, float angle);
};
