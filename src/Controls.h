#pragma once
#include <windows.h>
#include "Renderer.h"

class Controls {

public:

	Controls();

	void Create(Renderer& renderer);
	void HandleInput(HANDLE input);
	bool GetExit();
	void Reset();

private:

	bool exit;
};
