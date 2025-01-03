#include "structs.h"
#include "FlyFish.h"

class Pillar
{
public:
	explicit Pillar();
	~Pillar(); 

	Pillar(const Pillar& other) = delete;
	Pillar& operator=(const Pillar& other) = delete;
	Pillar(Pillar&& other) = delete;
	Pillar& operator=(Pillar&& other) = delete;

	void Draw(Color4f pillarColor) const;
	
private:
	const float m_PillarDimensions = 20.f; 

	ThreeBlade m_PillarPosition;
};