#include "Enemy.h"
#include "utils.h"
#include "ScoreManager.h"

Enemy::Enemy(ThreeBlade position, Point2f window)
{
	m_EntityDimensions = 30.f;
	m_EntityColor = Color4f{ 0.f, 0.f, 1.f, 1.f };
	m_MovementDirection = TwoBlade{ 1, 0, 0, 0, 0, 400 };
	m_EntityPosition = position;
	m_WindowDimentions = window;

	// Randomize Movement Direction
	srand(unsigned(time(nullptr))); 
	m_MovementDirection = utils::RotateBladeDirection(m_MovementDirection, float(rand() % 60));
}

Enemy::~Enemy()
{
}

void Enemy::Update(float elapsedSec, Player& player)
{
	Motor transformationMotor{ utils::MakeTranslationMotor(m_MovementDirection, elapsedSec) };
	m_EntityPosition = (transformationMotor * m_EntityPosition * ~transformationMotor).Grade3();

	// Check Collisions with Player
	const float playerDimensions{ player.GetDimensions() };
	const ThreeBlade& playerPos{ player.GetPosition() }; 

	if (CheckEntityCollisions(playerPos)[3] <= playerDimensions && CheckEntityCollisions(playerPos)[4] <= playerDimensions)
	{
		m_MovementDirection *= -1;
		player.SetMovementDirection(-1);

		ScoreManager::GetInstance().AddScore(-10);
	}
}

void Enemy::Draw() const
{
	utils::SetColor(m_EntityColor); 
	utils::FillRect(m_EntityPosition[0], m_EntityPosition[1], m_EntityDimensions, m_EntityDimensions);
}

void Enemy::LeftRightPlaneCollisions(OneBlade plane, const float distance)
{
	const float offset{ 5.f };

	// if going left
	if (m_MovementDirection[0] <= 0)
	{
		// distance between left and wall = 0 or smaller
		if (distance <= offset)
		{
			m_MovementDirection = (plane * m_MovementDirection * ~plane).Grade2();
			m_EntityPosition[0] = offset;
		}
	}
	else
	{
		// distance between right and wall = width or smaller
		if (distance <= offset)
		{
			m_MovementDirection = (plane * m_MovementDirection * ~plane).Grade2();
			m_EntityPosition[0] = m_WindowDimentions.x - m_EntityDimensions - offset;
		}
	}
}

void Enemy::TopBottomPlaneCollisions(OneBlade plane, const float distance)
{
	const float offset{ 5.f };

	// if going top
	if (m_MovementDirection[1] <= 0)
	{
		// distance between top and wall = 0 or smaller
		if (distance <= offset)
		{
			m_MovementDirection = (plane * m_MovementDirection * ~plane).Grade2();
			m_EntityPosition[1] = offset;
		}
	}
	else
	{
		// distance between bottom and wall = width or smaller
		if (distance <= offset)
		{
			m_MovementDirection = (plane * m_MovementDirection * ~plane).Grade2();
			m_EntityPosition[1] = m_WindowDimentions.y - m_EntityDimensions - offset;
		}
	}
}

float Enemy::GetDimensions() const
{
	return m_EntityDimensions;
}

TwoBlade Enemy::GetMovementDirection() const
{
	return m_MovementDirection; 
}

TwoBlade Enemy::CheckEntityCollisions(const ThreeBlade& entity)
{
	TwoBlade distance{ utils::ComputeDistance(m_EntityPosition, entity) };
	return utils::Abs(distance);
}

ThreeBlade Enemy::GetPosition() const
{
	return m_EntityPosition;
}