#include "FlyFish.h"

class Pickup
{
public:
	explicit Pickup(Point2f window);
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
	Point2f m_WindowDimentions;

	const float m_PickupDimensions = 10.f; 
};