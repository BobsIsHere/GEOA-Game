#pragma once
#include "FlyFish.h"
#include "structs.h"

class Entity
{
public:
	Entity();
	virtual ~Entity();

	Entity(const Entity&) = default;
	Entity& operator=(const Entity&) = default;

	Entity(Entity&&) noexcept = default;
	Entity& operator=(Entity&&) noexcept = default;

	virtual void Update(float elapsedSec) = 0;
	virtual void Draw() const = 0;
	virtual void PlaneCollisions(OneBlade plane, const float distance) = 0;
	virtual ThreeBlade GetPosition() const = 0;
	 
protected:
	float m_Dimensions;

	Color4f m_Color;
	TwoBlade m_MovementDirection;
	ThreeBlade m_Position;
};