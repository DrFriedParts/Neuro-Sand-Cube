#pragma once

#include "StateDistributor.h"
#include "game.h"

class NeuroSandCube
{
public:
	NeuroSandCube(void);
	~NeuroSandCube(void);

	void Initialize(fpsent* player);
	void Update();

private:

	StateDistributor distributor;
	fpsent* player;
};

