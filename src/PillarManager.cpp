#include "PillarManager.h"

PillarManager::PillarManager() :
	m_PillarColor{ Color4f{ 1.f, 0.f, 1.f, 1.f } },
	m_SelectedPillarColor{ Color4f{ 1.f, 1.f, 0.f, 1.f } }
{
}

PillarManager::~PillarManager()
{
	for (Pillar* pillar : m_Pillars)
	{
		pillar = nullptr;
		delete pillar;
	}

	m_Pillars.clear();
}

void PillarManager::SpawnPillar(const ThreeBlade& position)
{
	Pillar* newPillar{ new Pillar(position) };
	m_Pillars.push_back(newPillar);
}

void PillarManager::Update()
{
	
}

void PillarManager::Draw(size_t selectedPillarIndex) const
{
	for (Pillar* pillar : m_Pillars)
	{
		for (size_t idx = 0; idx < m_Pillars.size(); ++idx)
		{
			if (idx == selectedPillarIndex) 
			{
				m_Pillars[idx]->Draw(m_SelectedPillarColor);
			}
			else
			{
				m_Pillars[idx]->Draw(m_PillarColor);
			}
		}
	}
}

void PillarManager::RemoveFirstPillar()
{
	if (!m_Pillars.empty())
	{
		// Remove first pillar from container
		m_Pillars.erase(m_Pillars.begin());
	}
}

size_t PillarManager::GetPillarAmount() const
{
	return m_Pillars.size();
}

ThreeBlade PillarManager::GetPillarPosition(size_t index) const
{
	if (index < m_Pillars.size())
	{
		return m_Pillars[index]->GetPillarPosition();
	}

	return ThreeBlade(0.f, 0.f, 0.f);
}
