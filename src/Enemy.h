#pragma once
#include "structs.h"
#include "FlyFish.h" 

class Enemy final
{
public:
	explicit Enemy(ThreeBlade position);
	~Enemy();

	Enemy(const Enemy& other) = delete;
	Enemy& operator=(const Enemy& other) = delete;
	Enemy(Enemy&& other) = delete;
	Enemy& operator=(Enemy&& other) = delete;

	void Update(float elapsedSec);
	void Draw() const;
	void PlaneCollisions(OneBlade plane, const float distance);

	ThreeBlade GetPosition() const;

private:
	const float m_EnemyDimensions = 30.f;

	Color4f m_EnemyColor;

	TwoBlade m_MovementDirection;
	ThreeBlade m_EnemyPosition;
};