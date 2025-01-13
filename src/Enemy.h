#pragma once
#include "Entity.h"
#include "structs.h"
#include "FlyFish.h" 
#include "Player.h"

class Enemy final : public Entity
{
public:
	explicit Enemy(ThreeBlade position, Point2f window); 
	~Enemy();

	Enemy(const Enemy& other) = delete;
	Enemy& operator=(const Enemy& other) = delete;
	Enemy(Enemy&& other) = delete;
	Enemy& operator=(Enemy&& other) = delete;

	void Update(float elapsedSec, Player& player);
	void Draw() const override;
	void LeftRightPlaneCollisions(OneBlade plane, const float distance) override;
	void TopBottomPlaneCollisions(OneBlade plane, const float distance) override;

	float GetDimensions() const override; 

	TwoBlade GetMovementDirection() const override;
	TwoBlade CheckEntityCollisions(const ThreeBlade& entity); 

	ThreeBlade GetPosition() const override;

private:

};