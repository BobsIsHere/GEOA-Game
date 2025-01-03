#include "Player.h"
#include "utils.h"

Player::Player() :
	m_HasShiftBeenPressed{ false },
	m_ShouldReflect{ false },
	m_IsRotating{ false }, 
	m_CurrentPillarIndex{},
	m_PlayerDimensions{ 40.f },
	m_PlayerMinEnergy{ 0.f },
	m_PlayerEnergy{ 100.0f },
	m_EnergyDrainSpeed{ 60.f },
	m_CooldownDuration{ 1.0f },
	m_CooldownTimer{},
	m_PlayerVelocity{},
	m_PlayerMovementDirection{},
	m_PlayerColor{ 0.f, 1.f, 0.f, 1.f }
{
	m_PlayerPosition = ThreeBlade{ 200, 200, m_PlayerEnergy, 1 };
}

Player::~Player()
{
}

void Player::Update(float elapsedSec, ThreeBlade pillarPos)
{
	if (m_IsRotating)
	{
		const float rotationAngle{ 45.f * elapsedSec };
		m_PlayerPosition = utils::RotateAroundPillar(m_PlayerPosition, pillarPos, m_PlayerVelocity, rotationAngle);

		m_PlayerMovementDirection = utils::RotateVelocity(m_PlayerMovementDirection, pillarPos, rotationAngle);
		m_PlayerVelocity = utils::RotateVelocity(m_PlayerVelocity, pillarPos, rotationAngle); 
	}
	else if (m_ShouldReflect)
	{
		m_PlayerPosition = (-pillarPos * -utils::MakeTranslationMotor(m_PlayerMovementDirection, elapsedSec)
							* m_PlayerPosition  
							* ~utils::MakeTranslationMotor(m_PlayerMovementDirection, elapsedSec) * ~pillarPos).Grade3();

		m_PlayerPosition[2] *= -1;
		m_ShouldReflect = false;
	}
	else
	{
		m_PlayerPosition = (utils::MakeTranslationMotor(m_PlayerMovementDirection, elapsedSec) 
							* m_PlayerPosition 
							* ~utils::MakeTranslationMotor(m_PlayerMovementDirection, elapsedSec)).Grade3();
	}
	
	// Update the player color
	UpdatePlayerColor();

	// Update cooldown timer if active
	if (m_CooldownTimer > 0.0f)
	{
		m_CooldownTimer -= elapsedSec;
	}

	// Increase or decrease Player's energy
	if (m_HasShiftBeenPressed)
	{
		if (m_PlayerPosition[2] > m_PlayerMinEnergy)
		{
			m_PlayerPosition[2] = std::max(m_PlayerMinEnergy, m_PlayerPosition[2] - m_EnergyDrainSpeed * elapsedSec);
		}
		else
		{
			m_HasShiftBeenPressed = false;
			m_PlayerMovementDirection /= 2;
			m_PlayerVelocity /= 2;
			m_CooldownTimer = m_CooldownDuration;
		}
	}
	else
	{
		if (m_PlayerPosition[2] < m_PlayerEnergy)
		{
			m_PlayerPosition[2] = std::min(m_PlayerPosition[2] + m_EnergyDrainSpeed * elapsedSec, m_PlayerEnergy);
		}
	}
}

void Player::Draw() const
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	utils::SetColor(m_PlayerColor);
	utils::FillRect(m_PlayerPosition[0], m_PlayerPosition[1], m_PlayerDimensions, m_PlayerDimensions); 
}

void Player::PlayerKeyDownEvent(const SDL_KeyboardEvent& e, const int pillarAmount)
{
	if (e.keysym.sym == SDLK_LSHIFT)
	{
		if (!m_HasShiftBeenPressed && m_CooldownTimer <= 0.0f && m_PlayerPosition[2] > m_PlayerMinEnergy) 
		{
			m_HasShiftBeenPressed = true;
			m_PlayerVelocity *= 2;
		}
	}

	if (e.keysym.sym == SDLK_SPACE)
	{
		m_ShouldReflect = !m_ShouldReflect;
	}

	if (e.keysym.sym == SDLK_r)
	{
		m_IsRotating = !m_IsRotating;
	}

	if (e.keysym.sym == SDLK_q)
	{
		// Switch between pillars
		if (pillarAmount <= 0)
		{
			// Increment and wrap around
			m_CurrentPillarIndex = (m_CurrentPillarIndex + 1) % pillarAmount; 
		}
	}
}

void Player::PlayerKeyUpEvent(const SDL_KeyboardEvent& e)
{
	if (e.keysym.sym == SDLK_LSHIFT)
	{
		if (m_HasShiftBeenPressed)
		{
			m_HasShiftBeenPressed = false;
			m_PlayerVelocity /= 2;
		}
	}
}

void Player::UpdatePlayerColor()
{
	// Update the player color 
	Color4f lowEnergyColor = Color4f{ 1.0f, 0.0f, 0.0f, 1.0f };
	Color4f highEnergyColor = Color4f{ 0.0f, 1.0f, 0.0f, 1.0f };

	// Calculate the energy ratio (0.0 to 1.0)
	float energyRatio = (m_PlayerPosition[2] - m_PlayerMinEnergy) / (m_PlayerEnergy - m_PlayerMinEnergy); 

	// Interpolate the color based on the energy ratio
	m_PlayerColor.r = lowEnergyColor.r + energyRatio * (highEnergyColor.r - lowEnergyColor.r);
	m_PlayerColor.g = lowEnergyColor.g + energyRatio * (highEnergyColor.g - lowEnergyColor.g);
	m_PlayerColor.b = lowEnergyColor.b + energyRatio * (highEnergyColor.b - lowEnergyColor.b);
}
