#define NOMINMAX
#include <windows.h>
#include <iostream>
#include <chrono>
#include <vector>
#include <algorithm>


HANDLE hConsole = CreateConsoleScreenBuffer(
	GENERIC_READ | GENERIC_WRITE,
	0,
	nullptr,
	CONSOLE_TEXTMODE_BUFFER,
	nullptr
);

HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);

const int screenWidth = 120;
const int screenHeight = 30;

CHAR_INFO cells[screenWidth * screenHeight]{};
SMALL_RECT writeRegion = { 0, 0, screenWidth - 1, screenHeight - 1 };

const float fixedDelta = 1.0f / 30.0f;
float accumulator = 0.0f;

const std::string statistics = "STATISTICS";
const std::string lines = "LINES - 000";
const std::string top = "TOP";
const std::string score = "SCORE ";
const std::string next = "NEXT";
const std::string level = "LEVEL";
const std::string sixDigits = "000000";
const std::string threeDigits = "000";

const std::vector<std::string> horizontalTetrominos = { 
"     0 0 0  0  ",			   // T
"     0 0 0    0",			   // J
"     0 0    0 0",			   // Z
"0 00 0",					   // O
"       0 00 0  ",			   // S
"     0 0 00    ",			   // L	
"              0 0 0 0       " // I
};
const std::vector<std::string> verticalTetrominos = {
"  0  0 0    0  ",			   // T
"  0    0  0 0  ",			   // J
"  0  0 0  0    ",			   // Z
"0 00 0",					   // O
"0    0 0    0  ",			   // S
"0 0    0    0  ",			   // L
"  0      0      0      0    " // I
};
const std::vector<WORD> colors = {
	FOREGROUND_BLUE | FOREGROUND_RED,
	FOREGROUND_BLUE,
	FOREGROUND_RED,
	FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY,
	FOREGROUND_GREEN | FOREGROUND_INTENSITY,
	FOREGROUND_RED | FOREGROUND_GREEN,
	FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY
};

struct Gamestate {
	bool quit = false;
	bool paused = false;
	int tetromino = 0;
	int x = screenWidth / 2 - 1;
	int y = 6;
	int rotation = 0;
	int tetrominoNext = 0;
	int xNext = x;
	int yNext = y;
	int rotationNext = rotation;
	int speed = 5000;
	int score = 0;
};

enum Direction {DOWN, LEFT, RIGHT};

Gamestate gamestate;

void WriteCell(int x, int y, char c, WORD color) {
	int i = y * screenWidth + x;
	cells[i].Char.AsciiChar = c;
	cells[i].Attributes = color;
}

void WriteText(std::string text, int x, int y, WORD color) {
	for (int i = 0; i < text.length(); i++) {
		WriteCell(x + i, y, text[i], color);
	}
}

void WriteBorders() {

	for (int x = 0; x < screenWidth; x++) {
		for (int y = 0; y < screenHeight; y++) {
			WriteCell(x, y, ' ', 0);
		}
	}

	for (int y = 1; y < screenHeight - 1; y++) {
		for (int x = screenWidth / 2 - 32; x <= screenWidth / 2 + 32; x++) {
			if (y == 1 || y == screenHeight - 2 ||
				x == screenWidth / 2 - 11 || x == screenWidth / 2 + 11 ||
				x == screenWidth / 2 - 10 || x == screenWidth / 2 + 10 ||
				x == screenWidth / 2 - 32 || x == screenWidth / 2 + 32) {
				WriteCell(x, y, '#', FOREGROUND_INTENSITY);
			}
		}
	}

	for (int x = screenWidth / 2 - 32; x < screenWidth / 2 + 10; x++) {
		WriteCell(x, 5, '#', FOREGROUND_INTENSITY);
	}

	for (int x = screenWidth / 2 - 9; x < screenWidth / 2 + 10; x++) {
		WriteCell(x, 6, '#', FOREGROUND_INTENSITY);
		WriteCell(x, 27, '#', FOREGROUND_INTENSITY);
	}

	for (int x = screenWidth / 2 + 10; x < screenWidth / 2 + 32; x++) {
		WriteCell(x, 11, '#', FOREGROUND_INTENSITY);
		WriteCell(x, 20, '#', FOREGROUND_INTENSITY);
	}
}

void WriteTetromino(int tetromino, int x, int y, int rotation) {
	std::string t;
	rotation % 2 == 0 ? t = horizontalTetrominos[tetromino] : t = verticalTetrominos[tetromino];
	int s = (1 + static_cast<int>(std::sqrt(8 * static_cast<int>(t.length()) + 1))) / 4;
	for (int i = 0; i < s; i++) {
		for (int j = 0; j < 2 * s - 1; j++) {
			if (t[i * (2 * s - 1) + j] == '0') {
				if (rotation == 0 || rotation == 1) {
					WriteCell(x + j, y + i, '0', colors[tetromino]);
				}
				else {
					WriteCell(x + (2 * s - 2) - j, y + (s - 1) - i, '0', colors[tetromino]);
				}
			}
		}
	}
}

void EraseTetromino(int tetromino, int x, int y, int rotation) {
	std::string t;
	rotation % 2 == 0 ? t = horizontalTetrominos[tetromino] : t = verticalTetrominos[tetromino];
	int s = (1 + static_cast<int>(std::sqrt(8 * static_cast<int>(t.length()) + 1))) / 4;
	for (int i = 0; i < s; i++) {
		for (int j = 0; j < 2 * s - 1; j++) {
			if (t[i * (2 * s - 1) + j] == '0') {
				if (rotation == 0 || rotation == 1) {
					WriteCell(x + j, y + i, ' ', 0);
				}
				else {
					WriteCell(x + (2 * s - 2) - j, y + (s - 1) - i, ' ', 0);
				}
			}
		}
	}
}

bool isValidMove(Direction direction) {

	if (gamestate.paused) {
		return false;
	}

	std::string t;
	gamestate.rotation % 2 == 0 ? t = horizontalTetrominos[gamestate.tetromino] : t = verticalTetrominos[gamestate.tetromino];
	int s = (1 + static_cast<int>(std::sqrt(8 * static_cast<int>(t.length()) + 1))) / 4;

	switch (direction) {
	case DOWN:
		for (int x = 0; x < 2 * s - 1; x++) {
			int y = s - 1;
			if (gamestate.rotation == 0 || gamestate.rotation == 1) {
				while (t[y * (2 * s - 1) + x] == ' ' && y > 0) {
					y--;
				}
			}
			else {
				while (t[(s - 1 - y) * (2 * s - 1) + (2 * s - 2) - x] == ' ' && y > 0) {
					y--;
				}
			}

			if (y > 0 && cells[(gamestate.y + y + 1) * screenWidth + (gamestate.x + x)].Char.AsciiChar != ' ') {
				return false;
			}
		}
		break;
	case LEFT:
		for (int y = 0; y < s; y++) {
			int x = 0;
			if (gamestate.rotation == 0 || gamestate.rotation == 1) {
				while (t[y * (2 * s - 1) + x] == ' ' && x < 2 * s - 2) {
					x++;
				}
			}
			else {
				while (t[(s - 1 - y) * (2 * s - 1) + (2 * s - 2) - x] == ' ' && x < 2 * s - 2) {
					x++;
				}
			}
			if (x < 2 * s - 1 && cells[(gamestate.y + y) * screenWidth + (gamestate.x + x - 2)].Char.AsciiChar != ' ') {
				return false;
			}
		}
		break;
	case RIGHT:
		for (int y = 0; y < s; y++) {
			int x = 2 * s - 2;
			if (gamestate.rotation == 0 || gamestate.rotation == 1) {
				while (t[y * (2 * s - 1) + x] == ' ' && x > 0) {
					x--;
				}
			}
			else {
				while (t[(s - 1 - y) * (2 * s - 1) + (2 * s - 2) - x] == ' ' && x > 0) {
					x--;
				}
			}
			if (x > 0 && cells[(gamestate.y + y) * screenWidth + (gamestate.x + x + 2)].Char.AsciiChar != ' ') {
				return false;
			}
		}
		break;
	}
	return true;
}

bool isValidRotation() {
	return true;
}

void ClearLines() {
	int s = (1 + static_cast<int>(std::sqrt(8 * static_cast<int>(horizontalTetrominos[gamestate.tetromino].length()) + 1))) / 4;
	int lines = 0;
	for (int y = 0; y < s; y++) {
		if (gamestate.y + y < 27) {
			bool line = true;
			for (int x = screenWidth / 2 - 9; x < screenWidth / 2 + 10; x += 2) {
				if (cells[(gamestate.y + y) * screenWidth + x].Char.AsciiChar == ' ') {
					line = false;
					break;
				}
			}
			if (line) {
				for (int j = gamestate.y + y; j > 7; j--) {
					for (int i = screenWidth / 2 - 9; i < screenWidth / 2 + 10; i += 2) {
						WriteCell(i, j, cells[(j - 1) * screenWidth + i].Char.AsciiChar, cells[(j - 1) * screenWidth + i].Attributes);
					}
				}
			}
		}
	}
}

void HandleInput() {
	INPUT_RECORD record;
	DWORD eventsRead;
	while (PeekConsoleInput(hInput, &record, 1, &eventsRead) && eventsRead) {
		ReadConsoleInput(hInput, &record, 1, &eventsRead);
		if (record.EventType == KEY_EVENT) {
			auto& key = record.Event.KeyEvent;
			if (key.bKeyDown) {
				switch (key.wVirtualKeyCode) {
				case VK_ESCAPE:
					gamestate.quit = true;
					break;
				case 'A':
					if (isValidMove(LEFT)) {
						gamestate.xNext = gamestate.x - 2;
					}
					break;
				case 'S':
					if (isValidMove(DOWN)) {
						gamestate.yNext = gamestate.y + 1;
					}
					break;
				case 'D':
					if (isValidMove(RIGHT)) {
						gamestate.xNext = gamestate.x + 2;
					}
					break;
				case 'K':
					gamestate.rotationNext = (gamestate.rotation + 3) % 4;
					break;
				case 'L':
					gamestate.rotationNext = (gamestate.rotation + 1) % 4;
					break;
				case 'H':
					gamestate.paused = !gamestate.paused;
					break;
				}
			}
		}
	}
}

int main() {

	CONSOLE_CURSOR_INFO cursorInfo;
	GetConsoleCursorInfo(hConsole, &cursorInfo);
	cursorInfo.bVisible = FALSE;
	SetConsoleCursorInfo(hConsole, &cursorInfo);

	SetConsoleActiveScreenBuffer(hConsole);

	srand(static_cast<unsigned int>(time(0)));
	int ticks = 0;

	WriteBorders();

	for (int i = 0; i < 7; i++) {
		int length = static_cast<int>(verticalTetrominos[i].length());
		int s = (1 + static_cast<int>(std::sqrt(8 * length + 1))) / 4;
		int y = 9 - s + 3 * i;
		if (s > 3) y++;
		WriteTetromino(i, 37 - s, y, 0);
		WriteText(threeDigits, 42, (8 + 3 * i), FOREGROUND_INTENSITY);
	}

	WriteText(statistics, screenWidth / 2 - static_cast<int>(statistics.length()) - 16, 3, FOREGROUND_INTENSITY);
	WriteText(lines, +(1 + screenWidth - static_cast<int>(lines.length())) / 2, 3, FOREGROUND_INTENSITY);
	WriteText(top, screenWidth / 2 + 19, 4, FOREGROUND_INTENSITY);
	WriteText(sixDigits, screenWidth / 2 + 19, 5, FOREGROUND_INTENSITY);
	WriteText(score, screenWidth / 2 + 19, 7, FOREGROUND_INTENSITY);
	WriteText(sixDigits, screenWidth / 2 + 19, 8, FOREGROUND_INTENSITY);
	WriteText(next, screenWidth / 2 + 19, 14, FOREGROUND_INTENSITY);
	WriteText(level, screenWidth / 2 + 19, 23, FOREGROUND_INTENSITY);

	auto past = std::chrono::steady_clock::now();

	gamestate.tetromino = rand() % 7;
	gamestate.tetrominoNext = rand() % 7;
	WriteTetromino(gamestate.tetrominoNext, screenWidth - 41, 18 - (1 + static_cast<int>(std::sqrt(8 * static_cast<int>(horizontalTetrominos[gamestate.tetrominoNext].length()) + 1))) / 4, 0);
	if (gamestate.tetromino == 3) {
		gamestate.y++;
	}
	else if (gamestate.tetromino == 6) {
		gamestate.x -= 2;
		gamestate.y--;
	}
	gamestate.xNext = gamestate.x;
	gamestate.yNext = gamestate.y;

	while (!gamestate.quit) {

		auto now = std::chrono::steady_clock::now();
		std::chrono::duration<float> frameTime = now - past;
		past = now;
		accumulator += frameTime.count();
		accumulator = std::min(accumulator, fixedDelta);

		while (accumulator >= fixedDelta) {

			HandleInput();

			if (ticks >= gamestate.speed) {
				ticks = 0;
				if (isValidMove(DOWN)) {
					gamestate.yNext++;
				}
				else {
					ClearLines();

					EraseTetromino(gamestate.tetrominoNext, screenWidth - 41, 18 - (1 + static_cast<int>(std::sqrt(8 * static_cast<int>(horizontalTetrominos[gamestate.tetrominoNext].length()) + 1))) / 4, 0);
					gamestate.tetromino = gamestate.tetrominoNext;
					gamestate.tetrominoNext = rand() % 7;
					WriteTetromino(gamestate.tetrominoNext, screenWidth - 41, 18 - (1 + static_cast<int>(std::sqrt(8 * static_cast<int>(horizontalTetrominos[gamestate.tetrominoNext].length()) + 1))) / 4, 0);
					gamestate.rotation = 0;
					gamestate.x = screenWidth / 2 - 1;
					gamestate.y = 6;
					if (gamestate.tetromino == 3) {
						gamestate.y++;
					}
					else if (gamestate.tetromino == 6) {
						gamestate.x -= 2;
						gamestate.y--;
					}
					gamestate.xNext = gamestate.x;
					gamestate.yNext = gamestate.y;
					gamestate.rotationNext = gamestate.rotation;
				}
			}

			EraseTetromino(gamestate.tetromino, gamestate.x, gamestate.y, gamestate.rotation);
			gamestate.x = gamestate.xNext;
			gamestate.y = gamestate.yNext;
			gamestate.rotation = gamestate.rotationNext;
			WriteTetromino(gamestate.tetromino, gamestate.x, gamestate.y, gamestate.rotation);

			accumulator -= fixedDelta;
		}

		WriteConsoleOutput(
			hConsole,
			cells,
			{ screenWidth, screenHeight },
			{ 0, 0 },
			&writeRegion
		);

		ticks++;
	}

	CloseHandle(hConsole);
	CloseHandle(hInput);

	return 0;
} 