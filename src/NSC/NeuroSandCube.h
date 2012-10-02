#pragma once

#include "SharedStateDistributor.h"
#include "game.h"

class NeuroSandCube
{
public:
	NeuroSandCube(void);
	~NeuroSandCube(void);

	void Initialize(fpsent* player);
	void Update();

	void ResetFrame();		

private:

	SharedStateDistributor distributor;
	fpsent* player;
};

