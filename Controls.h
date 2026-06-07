#pragma once
#include <windows.h>
#include "Renderer.h"

class Controls {

public:

	Controls();

	void Write(Renderer& renderer);
	void HandleInput(HANDLE input);
	bool getExit();
	void Reset();

private:

	bool exit;
};
