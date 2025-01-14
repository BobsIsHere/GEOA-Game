#pragma once
#include "SDL_opengl.h"
#include "SDL.h"

#include "Entity.h"
#include "FlyFish.h"
#include "structs.h"

class Player final : public Entity 
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

	void PlayerKeyDownEvent(const SDL_KeyboardEvent& e); 
	void PlayerKeyUpEvent(const SDL_KeyboardEvent& e);
	void PlayerMouseDownEvent(const SDL_MouseButtonEvent& e);
	
	void LeftRightPlaneCollisions(OneBlade plane, const float distance) override;  
	void TopBottomPlaneCollisions(OneBlade plane, const float distance) override;  
	void SetMovementDirection(const float directionMultiplier);
	void IncreasePlayerEnergy(const float energy);

	int GetCurrentPillarIndex() const;

	float GetDimensions() const override;

	TwoBlade GetMovementDirection() const override;
	TwoBlade GetVelocity() const;
	ThreeBlade GetPosition() const override;

private:
	void UpdatePlayerColor(); 
	void ClampToViewport(); 
	void SpawnPillar(const ThreeBlade& spawnPosition); 

	bool m_HasShiftBeenPressed;
	bool m_ShouldReflect;
	bool m_IsRotating;

	int m_CurrentPillarIndex;

	const float m_PlayerMinEnergy = 0.f;
	const float m_EnergyDrainSpeed = 60.f;
	const float m_CooldownDuration = 1.0f;

	float m_PlayerEnergy = 100.f; 
	float m_CooldownTimer;

	TwoBlade m_PlayerVelocity;
};