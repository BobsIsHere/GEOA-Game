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
	m_PlayerVelocity{ TwoBlade{ 0, 0, 0, 0, 0, 1 } } 
{
	m_EntityDimensions = 40.f;
	m_EntityColor = Color4f{ 0.f, 1.f, 0.f, 1.f };
	m_MovementDirection = TwoBlade{ 1, 0, 0, 0, 0, 400 }; 
	m_EntityPosition = ThreeBlade{ xPos, yPos, m_PlayerEnergy, 1 }; 
	m_WindowDimentions = window;
}

Player::~Player()
{
}

void Player::Update(float elapsedSec, ThreeBlade pillarPos)
{
	if (m_IsRotating)
	{
		const float rotationAngle{ 45.f * elapsedSec };
		m_EntityPosition = utils::RotateAroundPillar(m_EntityPosition, pillarPos, m_PlayerVelocity, rotationAngle);

		m_MovementDirection = utils::RotateBladeDirection(m_MovementDirection, rotationAngle);
		m_PlayerVelocity = utils::RotateBladeDirection(m_PlayerVelocity, rotationAngle); 
	}
	else if (m_ShouldReflect && PillarManager::GetInstance().GetPillarAmount() != 0)
	{
		Motor transformationMotor{ utils::MakeTranslationMotor(m_MovementDirection, elapsedSec) };
		m_EntityPosition = (-pillarPos * -transformationMotor * m_EntityPosition * ~transformationMotor * ~pillarPos).Grade3();
		ClampToViewport(); 

		m_EntityPosition[2] *= -1; 
		m_ShouldReflect = false;  
	}
	else
	{
		Motor transformationMotor{ utils::MakeTranslationMotor(m_MovementDirection, elapsedSec) };
		m_EntityPosition = (transformationMotor * m_EntityPosition * ~transformationMotor).Grade3(); 
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
		if (m_EntityPosition[2] > m_PlayerMinEnergy)
		{
			m_EntityPosition[2] = std::max(m_PlayerMinEnergy, m_EntityPosition[2] - m_EnergyDrainSpeed * elapsedSec);
		}
		else
		{
			m_HasShiftBeenPressed = false; 
			m_MovementDirection /= 2;  
			m_PlayerVelocity /= 2;   
			m_CooldownTimer = m_CooldownDuration;  
		}
	}
	else
	{
		if (m_EntityPosition[2] < m_PlayerEnergy)
		{
			m_EntityPosition[2] = std::min(m_EntityPosition[2] + m_EnergyDrainSpeed * elapsedSec, m_PlayerEnergy);
		}
	}
}

void Player::Draw() const
{
	utils::SetColor(m_EntityColor);
	utils::FillRect(m_EntityPosition[0], m_EntityPosition[1], m_EntityDimensions, m_EntityDimensions);
}

void Player::PlayerKeyDownEvent(const SDL_KeyboardEvent& e)
{
	const size_t pillarAmount{ PillarManager::GetInstance().GetPillarAmount() };

	if (e.keysym.sym == SDLK_LSHIFT)
	{
		if (!m_HasShiftBeenPressed && m_CooldownTimer <= 0.0f && m_EntityPosition[2] > m_PlayerMinEnergy)
		{
			m_HasShiftBeenPressed = true;
			m_PlayerVelocity *= 2;
			m_MovementDirection *= 2;  
		}
	}

	if (e.keysym.sym == SDLK_SPACE)
	{
		m_ShouldReflect = !m_ShouldReflect;
	}

	if (e.keysym.sym == SDLK_r)
	{
		if (pillarAmount > 0)
		{
			m_IsRotating = !m_IsRotating; 
		}
	}

	if (e.keysym.sym == SDLK_q)
	{
		// Switch between pillars
		if (pillarAmount > 0) 
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
		m_PlayerVelocity /= 2;
		m_MovementDirection /= 2;
	}
}

void Player::PlayerMouseDownEvent(const SDL_MouseButtonEvent& e)
{
	if (e.button == SDL_BUTTON_LEFT)
	{
		SpawnPillar(ThreeBlade{ float(e.x), float(e.y), 0.f });
	}
}

void Player::LeftRightPlaneCollisions(OneBlade plane, const float distance)
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

			if (m_IsRotating)
			{
				m_PlayerVelocity = -m_PlayerVelocity;
			}
		}
	}
	else
	{
		// distance between right and wall = width or smaller
		if (distance <= offset)
		{
			m_MovementDirection = (plane * m_MovementDirection * ~plane).Grade2();
			m_EntityPosition[0] = m_WindowDimentions.x - m_EntityDimensions - offset; 

			if (m_IsRotating)
			{
				m_PlayerVelocity = -m_PlayerVelocity;
			}
		}
	}
}

void Player::TopBottomPlaneCollisions(OneBlade plane, const float distance)
{
	const float offset{ 5.f };

	// if going bottom
	if (m_MovementDirection[1] <= 0)
	{
		// distance between bottom and wall = 0 or smaller
		if (distance <= offset)
		{
			m_MovementDirection = (plane * m_MovementDirection * ~plane).Grade2();
			m_EntityPosition[1] = offset;

			if (m_IsRotating)
			{
				m_PlayerVelocity = -m_PlayerVelocity;
			}
		}
	}
	else
	{
		// distance between top and wall = height or smaller
		if (distance <= offset)
		{
			m_MovementDirection = (plane * m_MovementDirection * ~plane).Grade2();
			m_EntityPosition[1] = m_WindowDimentions.y - m_EntityDimensions - offset;

			if (m_IsRotating)
			{
				m_PlayerVelocity = -m_PlayerVelocity;
			}
		}
	}
}

int Player::GetCurrentPillarIndex() const
{
	return m_CurrentPillarIndex; 
}

float Player::GetDimensions() const
{
	return m_EntityDimensions; 
}

TwoBlade Player::GetMovementDirection() const
{
	return m_MovementDirection; 
}

TwoBlade Player::GetVelocity() const
{
	return m_PlayerVelocity;
}

ThreeBlade Player::GetPosition() const
{
	return m_EntityPosition;  
}

void Player::SetMovementDirection(const float directionMultiplier)
{
	m_MovementDirection *= directionMultiplier; 
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
	float energyRatio = (m_EntityPosition[2] - m_PlayerMinEnergy) / (m_PlayerEnergy - m_PlayerMinEnergy); 

	// Interpolate the color based on the energy ratio
	m_EntityColor.r = lowEnergyColor.r + energyRatio * (highEnergyColor.r - lowEnergyColor.r);
	m_EntityColor.g = lowEnergyColor.g + energyRatio * (highEnergyColor.g - lowEnergyColor.g);
	m_EntityColor.b = lowEnergyColor.b + energyRatio * (highEnergyColor.b - lowEnergyColor.b);
}

void Player::ClampToViewport()
{
	// Clamp the player position to the viewport
	m_EntityPosition[0] = std::clamp(m_EntityPosition[0], m_EntityDimensions, static_cast<float>(m_WindowDimentions.x - m_EntityDimensions));
	m_EntityPosition[1] = std::clamp(m_EntityPosition[1], m_EntityDimensions, static_cast<float>(m_WindowDimentions.y - m_EntityDimensions));
}

void Player::SpawnPillar(const ThreeBlade& spawnPosition)
{
	PillarManager& pillarManager = PillarManager::GetInstance();
	const size_t maxPillars{ 3 };

	if (pillarManager.GetPillarAmount() >= maxPillars)
	{
		pillarManager.RemoveFirstPillar(); 
	}

	pillarManager.SpawnPillar(spawnPosition); 
}