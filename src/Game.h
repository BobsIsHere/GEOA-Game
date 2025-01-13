#pragma once
#include <vector>
#include "FlyFish.h"
#include "structs.h"
#include "SDL.h"
#include "SDL_opengl.h"

#include "PillarManager.h"
#include "Player.h"
#include "Pillar.h"
#include "Pickup.h"
#include "Entity.h"
#include "Enemy.h"

class Game
{
public:
	explicit Game(const Window& window);
	~Game();

	Game(const Game& other) = delete;
	Game& operator=(const Game& other) = delete;
	Game(Game&& other) = delete;
	Game& operator=(Game&& other) = delete;

	void Run();
	void Update(float elapsedSec);
	void Draw() const;

	// Event handling
	void ProcessKeyDownEvent(const SDL_KeyboardEvent& e)
	{
		m_Player.PlayerKeyDownEvent(e); 
	}
	void ProcessKeyUpEvent(const SDL_KeyboardEvent& e)
	{
		m_Player.PlayerKeyUpEvent(e); 
	}
	void ProcessMouseMotionEvent(const SDL_MouseMotionEvent& e)
	{
	}
	void ProcessMouseDownEvent(const SDL_MouseButtonEvent& e)
	{
		m_Player.PlayerMouseDownEvent(e); 
	}
	void ProcessMouseUpEvent(const SDL_MouseButtonEvent& e)
	{
	}

	const Rectf& GetViewPort() const
	{
		return m_Viewport;
	}

private:
	// FUNCTIONS
	void InitializeGameEngine();
	void InitializeGameVariables();
	void CleanupGameEngine();
	void ViewPortCollisionDetection(Entity& entity);

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

	std::vector<OneBlade> m_ViewportPlanes;

	OneBlade m_LeftPlane;
	OneBlade m_RightPlane;
	OneBlade m_TopPlane;
	OneBlade m_BottomPlane;

	Player m_Player; 
	Enemy m_Enemy;
	Pickup m_Pickup;
};

/*
Documentation of Flying Fish :
	* -> geometric product
	& -> inner product
	| -> outer product 
	^ -> meet
	~ -> inverse
	! -> poincare dual
*/