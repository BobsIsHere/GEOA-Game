#include <iostream>
#include "structs.h"
#include "Pickup.h"
#include "utils.h"


Pickup::Pickup(const ThreeBlade& position, float energyValue) :
	m_PickupPosition{ position },
	m_EnergyValue{ energyValue }
{
}

Pickup::~Pickup()
{
}

void Pickup::Update(float elapsedSec, const ThreeBlade& playerPos)
{
	if (CheckCollision(playerPos)[3] <= 25 && CheckCollision(playerPos)[4] <= 25)
	{
		std::cout << "Collision detected" << std::endl; 
	}
}

void Pickup::Draw() const
{
	utils::SetColor(Color4f{ 1.f, 0.f, 0.f, 1.f });
	utils::FillEllipse(m_PickupPosition[0], m_PickupPosition[1], 10, 10);
}

TwoBlade Pickup::CheckCollision(const ThreeBlade& playerPos)
{
	TwoBlade distance{ utils::ComputeDistance(playerPos, m_PickupPosition) }; 
	return utils::Abs(distance);
}