#include "FlyFish.h"

class Pickup
{
public:
	explicit Pickup(const ThreeBlade& position, float energyValue); 
	~Pickup();

	Pickup(const Pickup& other) = delete;
	Pickup& operator=(const Pickup& other) = delete;
	Pickup(Pickup&& other) = delete;
	Pickup& operator=(Pickup&& other) = delete;

	void Update(float elapsedSec, const ThreeBlade& playerPos);
	void Draw() const;

	TwoBlade CheckCollision(const ThreeBlade& playerPos);

private:
	ThreeBlade m_PickupPosition;
	float m_EnergyValue;
};