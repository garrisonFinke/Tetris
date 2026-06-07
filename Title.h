#pragma once
#include <string>
#include <vector>
#include <windows.h>
#include "Renderer.h"

class Title {

public:

	Title();

	void HandleInput(HANDLE input);
	void WriteBigLetter(Renderer& renderer, std::string letter, WORD color, int x, int y);
	void Write(Renderer& renderer);
	void Update(Renderer& renderer);
	int getSelected();
	void Reset();

private:

	const std::string bigT = "0000000000    00        00        00        00        00        00    ";
	const std::string bigE = "000000000000        00        000000000000        00        0000000000";
	const std::string bigR = "00000000  00      0000      0000000000  00      0000      0000      00";
	const std::string bigI = "0000000000    00        00        00        00        00    0000000000";
	const std::string bigS = "  000000  00      0000          000000          0000      00  000000  ";
	const std::vector<std::string> options = { "  START   ", " CONTROLS ", "   QUIT   " };
	int hovered;
	int selected;
};



