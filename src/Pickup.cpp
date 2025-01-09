#include <iostream>
#include "ScoreManager.h"
#include "structs.h"
#include "Pickup.h"
#include "Player.h"
#include "utils.h"


Pickup::Pickup(Point2f window) :
	m_PickupPosition{ 0.f, 0.f, 0.f, 1.f },
	m_WindowDimentions{ window } 
{
	// Get different random number each time program runs
	std::srand((unsigned)std::time(0));

	const float randomPosX{ m_WindowDimentions.x - m_PickupDimensions };
	const float randomPosY{ m_WindowDimentions.y - m_PickupDimensions };

	m_PickupPosition[0] = float(rand() % static_cast<int>(randomPosX));
	m_PickupPosition[1] = float(rand() % static_cast<int>(randomPosY));
}

Pickup::~Pickup()
{
}

void Pickup::Update(float elapsedSec, Player& player) 
{
	const float padding{ 40.f };
	const float randomPosX{ m_WindowDimentions.x - m_PickupDimensions };
	const float randomPosY{ m_WindowDimentions.y - m_PickupDimensions };
	const ThreeBlade& playerPos{ player.GetPlayerPosition() };

	if (CheckCollision(playerPos)[3] <= padding && CheckCollision(playerPos)[4] <= padding) 
	{
		m_PickupPosition[0] = float(rand() % static_cast<int>(randomPosX));
		m_PickupPosition[1] = float(rand() % static_cast<int>(randomPosY));

		ScoreManager::GetInstance().AddScore(10);
		player.IncreasePlayerEnergy(10.f);
	}
}

void Pickup::Draw() const
{
	utils::SetColor(Color4f{ 1.f, 0.f, 0.f, 1.f });
	utils::FillEllipse(m_PickupPosition[0], m_PickupPosition[1], m_PickupDimensions, m_PickupDimensions);
}

TwoBlade Pickup::CheckCollision(const ThreeBlade& playerPos)
{
	TwoBlade distance{ utils::ComputeDistance(playerPos, m_PickupPosition) }; 
	return utils::Abs(distance);
}