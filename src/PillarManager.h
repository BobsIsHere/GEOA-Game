#pragma once
#include <vector>
#include "structs.h"
#include "FlyFish.h"
#include "Pillar.h"

class PillarManager final
{
public:
	static PillarManager& GetInstance()
	{
		static PillarManager instance{};
		return instance;
	}

	virtual ~PillarManager();

	PillarManager(const PillarManager& other) = delete;
	PillarManager& operator=(const PillarManager& other) = delete;
	PillarManager(PillarManager&& other) = delete;
	PillarManager& operator=(PillarManager&& other) = delete;

	void SpawnPillar(const ThreeBlade& position); 
	void Update();
	void Draw(size_t selectedPillarIndex) const;
	void RemoveFirstPillar();

	size_t GetPillarAmount() const;
	ThreeBlade GetPillarPosition(size_t index) const;

protected:
	PillarManager();

private:
	Color4f m_PillarColor;
	Color4f m_SelectedPillarColor; 

	std::vector<Pillar*> m_Pillars;
};