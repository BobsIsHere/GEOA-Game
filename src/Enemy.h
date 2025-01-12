#pragma once
#include "Entity.h"
#include "structs.h"
#include "FlyFish.h" 
#include "Player.h"

class Enemy final : public Entity
{
public:
	explicit Enemy(ThreeBlade position);
	~Enemy();

	Enemy(const Enemy& other) = delete;
	Enemy& operator=(const Enemy& other) = delete;
	Enemy(Enemy&& other) = delete;
	Enemy& operator=(Enemy&& other) = delete;

	void Update(float elapsedSec) override;
	void Update(float elapsedSec, Player& player);
	void Draw() const override;
	void PlaneCollisions(OneBlade plane, const float distance) override;

	TwoBlade CheckEntityCollisions(const ThreeBlade& entity); 

	ThreeBlade GetPosition() const override;

private:

};