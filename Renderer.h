#pragma once
#include <windows.h>
#include <vector>
#include <string>

class Renderer {

public:

	Renderer(int width, int height);
	~Renderer();

	void WriteCell(char c, WORD color, int x, int y);
	void WriteText(std::string_view text, WORD color, int x, int y);
	void Wipe();
	void Refresh();


private:

	std::vector<CHAR_INFO> CELLS;
	const int WIDTH;
	const int HEIGHT;
	SMALL_RECT writeRegion;
	HANDLE hConsole;

};
