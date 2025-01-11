#include <algorithm>

#include "PillarManager.h"
#include "Player.h"
#include "utils.h"

Player::Player(float xPos, float yPos, Point2f window) : 
	m_HasShiftBeenPressed{ false },
	m_ShouldReflect{ false },
	m_IsRotating{ false }, 
	m_CurrentPillarIndex{},
	m_CooldownTimer{},
	m_PlayerVelocity{ m_BaseVelocity }, 
	m_PlayerMovementDirection{ m_BaseMovementDirection }, 
	m_PlayerColor{ 0.f, 1.f, 0.f, 1.f },
	m_WindowDimentions{ window } 
{
	m_PlayerPosition = ThreeBlade{ xPos, yPos, m_PlayerEnergy, 1 }; 
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

		m_PlayerMovementDirection = utils::RotateVelocity(m_PlayerMovementDirection, rotationAngle); 
		m_PlayerVelocity = utils::RotateVelocity(m_PlayerVelocity, rotationAngle); 
	}
	else if (m_ShouldReflect)
	{
		Motor transformationMotor{ utils::MakeTranslationMotor(m_PlayerMovementDirection, elapsedSec) };
		m_PlayerPosition = (-pillarPos * -transformationMotor * m_PlayerPosition * ~transformationMotor * ~pillarPos).Grade3(); 
		ClampToViewport(); 

		m_PlayerPosition[2] *= -1;  
		m_ShouldReflect = false;  
	}
	else
	{
		Motor transformationMotor{ utils::MakeTranslationMotor(m_PlayerMovementDirection, elapsedSec) };
		m_PlayerPosition = (transformationMotor * m_PlayerPosition * ~transformationMotor).Grade3();
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
			m_PlayerMovementDirection = m_BaseMovementDirection;  
			m_PlayerVelocity = m_BaseVelocity;  
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
	utils::SetColor(m_PlayerColor);
	utils::FillRect(m_PlayerPosition[0], m_PlayerPosition[1], m_PlayerDimensions, m_PlayerDimensions); 
}

void Player::PlayerKeyDownEvent(const SDL_KeyboardEvent& e)
{
	if (e.keysym.sym == SDLK_LSHIFT)
	{
		if (!m_HasShiftBeenPressed && m_CooldownTimer <= 0.0f && m_PlayerPosition[2] > m_PlayerMinEnergy) 
		{
			m_HasShiftBeenPressed = true;
			m_PlayerVelocity *= 2;
			m_PlayerMovementDirection *= 2; 
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
		const size_t pillarAmount{ PillarManager::GetInstance().GetPillarAmount() };

		// Switch between pillars
		if (pillarAmount >= 0) 
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
		m_HasShiftBeenPressed = false;
	}
}

void Player::PlayerMouseDownEvent(const SDL_MouseButtonEvent& e)
{
	if (e.button == SDL_BUTTON_LEFT)
	{
		SpawnPillar(ThreeBlade{ float(e.x), float(e.y), 0.f });
	}
}

void Player::PlaneCollisions(OneBlade plane, const float distance)
{
	if (distance < m_PlayerDimensions)
	{
		m_PlayerMovementDirection = (plane * m_PlayerMovementDirection * ~plane).Grade2(); 

		if (m_IsRotating)
		{
			m_PlayerVelocity = -m_PlayerVelocity;
		}
	}
}

int Player::GetCurrentPillarIndex() const
{
	return m_CurrentPillarIndex; 
}

ThreeBlade Player::GetPlayerPosition() const
{
	return m_PlayerPosition; 
}

void Player::IncreasePlayerEnergy(const float energy)
{
	m_PlayerEnergy += energy;  
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

void Player::ClampToViewport()
{
	// Clamp the player position to the viewport
	m_PlayerPosition[0] = std::clamp(m_PlayerPosition[0], m_PlayerDimensions, static_cast<float>(m_WindowDimentions.x - m_PlayerDimensions));
	m_PlayerPosition[1] = std::clamp(m_PlayerPosition[1], m_PlayerDimensions, static_cast<float>(m_WindowDimentions.y - m_PlayerDimensions));
}

void Player::SpawnPillar(const ThreeBlade& spawnPosition)
{
	PillarManager::GetInstance().SpawnPillar(spawnPosition);
}