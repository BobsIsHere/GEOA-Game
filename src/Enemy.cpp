#include "Enemy.h"
#include "utils.h"

Enemy::Enemy(ThreeBlade position)
{
	m_Dimensions = 30.f;
	m_Color = Color4f{ 0.f, 0.f, 1.f, 1.f };
	m_MovementDirection = TwoBlade{ 1, 0, 0, 0, 0, 400 };
	m_Position = position;
}

Enemy::~Enemy()
{
}

void Enemy::Update(float elapsedSec)
{
	Motor transformationMotor{ utils::MakeTranslationMotor(m_MovementDirection, elapsedSec) };
	m_Position = (transformationMotor * m_Position * ~transformationMotor).Grade3();

	//m_MovementDirection = utils::RotateVelocity(m_MovementDirection, 45.f * elapsedSec);
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

ThreeBlade Enemy::GetPosition() const
{
	return m_Position;
}