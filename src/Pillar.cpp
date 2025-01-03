#include "Pillar.h"
#include "utils.h"

Pillar::Pillar(ThreeBlade pillarPos) :
	m_PillarPosition{ pillarPos }
{
}

Pillar::~Pillar()
{
}

void Pillar::Draw(Color4f pillarColor) const 
{
	utils::SetColor(pillarColor); 
	utils::FillRect(m_PillarPosition[0], m_PillarPosition[1], m_PillarDimensions, m_PillarDimensions);
}

ThreeBlade Pillar::GetPillarPosition() const
{
	return m_PillarPosition;
}
