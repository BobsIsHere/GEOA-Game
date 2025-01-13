#include "Enemy.h"
#include "utils.h"
#include "ScoreManager.h"

Enemy::Enemy(ThreeBlade position)
{
	m_Dimensions = 30.f;
	m_Color = Color4f{ 0.f, 0.f, 1.f, 1.f };
	m_MovementDirection = TwoBlade{ 1, 0, 0, 0, 0, 400 };
	m_Position = position;

	// Randomize Movement Direction
	srand(time(nullptr));
	m_MovementDirection = utils::RotateBladeDirection(m_MovementDirection, rand() % 60);
}

Enemy::~Enemy()
{
}

void Enemy::Update(float elapsedSec, Player& player)
{
	Motor transformationMotor{ utils::MakeTranslationMotor(m_MovementDirection, elapsedSec) };
	m_Position = (transformationMotor * m_Position * ~transformationMotor).Grade3();

	// Check Collisions with Player
	const float playerDimensions{ player.GetDimensions() };
	const ThreeBlade& playerPos{ player.GetPosition() }; 

	if (CheckEntityCollisions(playerPos)[3] <= playerDimensions && CheckEntityCollisions(playerPos)[4] <= playerDimensions)
	{
		m_MovementDirection *= -1;
		ScoreManager::GetInstance().AddScore(-10);
	}
}

void Enemy::Draw() const
{
	utils::SetColor(m_Color); 
	utils::FillRect(m_Position[0], m_Position[1], m_Dimensions, m_Dimensions);
}

void Enemy::PlaneCollisions(OneBlade plane, const float distance)
{
	if (distance < m_Dimensions)
	{
		m_MovementDirection = (plane * m_MovementDirection * ~plane).Grade2();
	}
}

float Enemy::GetDimensions() const
{
	return m_Dimensions;
}

TwoBlade Enemy::CheckEntityCollisions(const ThreeBlade& entity)
{
	TwoBlade distance{ utils::ComputeDistance(m_Position, entity) };
	return utils::Abs(distance);
}

ThreeBlade Enemy::GetPosition() const
{
	return m_Position;
}