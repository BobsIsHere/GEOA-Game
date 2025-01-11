#include "Enemy.h"
#include "utils.h"

Enemy::Enemy(ThreeBlade position) :
	m_EnemyPosition{ position },
	m_EnemyColor{ Color4f{ 0.f, 0.f, 1.f, 1.f } },
	m_MovementDirection{ TwoBlade{ 1, 0, 0, 0, 0, 400 } } 
{
}

Enemy::~Enemy()
{
}

void Enemy::Update(float elapsedSec)
{
	Motor transformationMotor{ utils::MakeTranslationMotor(m_MovementDirection, elapsedSec) };
	m_EnemyPosition = (transformationMotor * m_EnemyPosition * ~transformationMotor).Grade3();

	//m_MovementDirection = utils::RotateVelocity(m_MovementDirection, 45.f * elapsedSec);
}

void Enemy::Draw() const
{
	utils::SetColor(m_EnemyColor);
	utils::FillRect(m_EnemyPosition[0], m_EnemyPosition[1], m_EnemyDimensions, m_EnemyDimensions);
}

void Enemy::PlaneCollisions(OneBlade plane, const float distance)
{
	if (distance < m_EnemyDimensions)
	{
		m_MovementDirection = (plane * m_MovementDirection * ~plane).Grade2();
	}
}

ThreeBlade Enemy::GetPosition() const
{
	return m_EnemyPosition;
}