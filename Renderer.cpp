#include "Renderer.h"

Renderer::Renderer(int width, int height) :
	WIDTH(width),
	HEIGHT(height),
	CELLS(width * height),
	writeRegion { 0, 0, static_cast<short>(width - 1), static_cast<short>(height - 1)}
{
	hConsole = (CreateConsoleScreenBuffer(
		GENERIC_READ | GENERIC_WRITE,
		0,
		nullptr,
		CONSOLE_TEXTMODE_BUFFER,
		nullptr
	));
	
	CONSOLE_CURSOR_INFO cursorInfo;
	GetConsoleCursorInfo(hConsole, &cursorInfo);
	cursorInfo.bVisible = FALSE;
	SetConsoleCursorInfo(hConsole, &cursorInfo);
	SetConsoleActiveScreenBuffer(hConsole);
}
Renderer::~Renderer() {
	CloseHandle(hConsole);
}

void Renderer::WriteCell(char c, WORD color, int x, int y) {
	int i = y * WIDTH + x;
	CELLS[i].Char.AsciiChar = c;
	CELLS[i].Attributes = color;
}
void Renderer::WriteText(std::string_view text, WORD color, int x, int y) {
	for (int i = 0; i < text.length(); i++) {
		WriteCell(text[i], color, x + i, y);
	}
}
void Renderer::Wipe() {
	std::fill(CELLS.begin(), CELLS.end(), CHAR_INFO{});
}
void Renderer::Refresh() {
	WriteConsoleOutput(
		hConsole,
		CELLS.data(),
		{ static_cast<short>(WIDTH), static_cast<short>(HEIGHT) },
		{ 0, 0 },
		&writeRegion
	);
}