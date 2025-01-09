#include "SDL_opengl.h"
#include "SDL.h"
#include "FlyFish.h"
#include "structs.h"

class Player
{
public:
	explicit Player(float xPos, float yPos, Point2f window);
	~Player();

	Player(const Player& other) = delete;
	Player& operator=(const Player& other) = delete;
	Player(Player&& other) = delete;
	Player& operator=(Player&& other) = delete;
	
	void Update(float elapsedSec, ThreeBlade pillarPos);
	void Draw() const;

	void PlayerKeyDownEvent(const SDL_KeyboardEvent& e, const size_t pillarAmount); 
	void PlayerKeyUpEvent(const SDL_KeyboardEvent& e);
	void PlaneCollisions(OneBlade plane, const float distance);  

	int GetCurrentPillarIndex() const;
	ThreeBlade GetPlayerPosition() const;

	void IncreasePlayerEnergy(const float energy);

private:
	void UpdatePlayerColor(); 
	void ClampToViewport(); 

	bool m_HasShiftBeenPressed;
	bool m_ShouldReflect;
	bool m_IsRotating;

	int m_CurrentPillarIndex;

	const float m_PlayerDimensions = 40.f;
	const float m_PlayerMinEnergy = 0.f;
	const float m_EnergyDrainSpeed = 60.f;
	const float m_CooldownDuration = 1.0f;

	float m_PlayerEnergy = 100.f; 
	float m_CooldownTimer;

	Point2f m_WindowDimentions;

	const TwoBlade m_BaseVelocity = TwoBlade{ 0, 0, 0, 0, 0, 1 };
	const TwoBlade m_BaseMovementDirection = TwoBlade{ 1, 0, 0, 0, 0, 400 }; 

	TwoBlade m_PlayerVelocity;
	TwoBlade m_PlayerMovementDirection;

	ThreeBlade m_PlayerPosition; 

	Color4f m_PlayerColor; 
};