#pragma once
#include "FlyFish.h"
#include "structs.h"
#include "SDL.h"
#include "SDL_opengl.h"

class Game
{
public:
	explicit Game( const Window& window );
	Game( const Game& other ) = delete;
	Game& operator=( const Game& other ) = delete;
	Game(Game&& other) = delete;
	Game& operator=(Game&& other) = delete;

	~Game();

	void Run( );

	void Update(float elapsedSec);

	void Draw() const;

	// Event handling
	void ProcessKeyDownEvent(const SDL_KeyboardEvent& e)
	{
		if (e.keysym.sym == SDLK_LSHIFT && !m_IsShiftStillPressed) 
		{
			m_IsShiftStillPressed = true;
			m_PlayerVelocity[0] *= 2;
		}
	}
	void ProcessKeyUpEvent(const SDL_KeyboardEvent& e)
	{
		if (e.keysym.sym == SDLK_LSHIFT && m_IsShiftStillPressed)
		{
			m_IsShiftStillPressed = false;
			m_PlayerVelocity[0] /= 2;
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

	const int m_PlayerDimensions = 40;

	bool m_IsShiftStillPressed;

	ThreeBlade m_PlayerPosition;
	ThreeBlade m_PillarPosition;
	ThreeBlade m_PlayerVelocity;
	
	// FUNCTIONS
	void InitializeGameEngine();
	void InitializeGameVariables();
	void CleanupGameEngine();

	ThreeBlade Translate(ThreeBlade player, ThreeBlade velocity, float elapsedSec);
	ThreeBlade RotateAroundPillar(ThreeBlade player, ThreeBlade pillar, float angle);
};
