#include "Game.h"
#include "Tetrominos.h"
#include <windows.h>
#include <string>
#include <fstream>

Game::Game() :
	board(WIDTH * HEIGHT) {
	srand(static_cast<unsigned int>(time(0)));
	Init();
}

void Game::HandleInput(HANDLE input, Renderer& renderer) {
	INPUT_RECORD record;
	DWORD eventsRead;
	while (PeekConsoleInput(input, &record, 1, &eventsRead) && eventsRead) {
		ReadConsoleInput(input, &record, 1, &eventsRead);
		if (record.EventType == KEY_EVENT) {
			auto& key = record.Event.KeyEvent;
			if (key.bKeyDown) {
				switch (key.wVirtualKeyCode) {
				case 'A':
					if (!paused) {
						MoveLeft();
					}
					break;
				case 'S':
					if (!paused) {
						if (CanMoveDown()) {
							score++;
						}
						MoveDown();
					}
					break;
				case 'D':
					if (!paused) {
						MoveRight();
					}
					break;
				case 'G':
					showNext = !showNext;
					if (showNext) {
						WriteTetrominoToScreen(renderer, next, 77, 15, 0);
					}
					else {
						EraseTetrominoFromScreen(renderer, next, 77, 15, 0);
					}
					break;
				case 'H':
					if (gameover) {
						exitGame = true;
					}
					else {
						paused = !paused;
						if (paused) {
							for (int y = 7; y < 27; y++) {
								for (int x = 51; x < 70; x++) {
									renderer.WriteCell('\0', 0, x, y);
								}
							}
							renderer.WriteText("P A U S E D", FOREGROUND_INTENSITY, 55, 16);
						}
						else {
							for (int x = 55; x < 66; x++) {
								renderer.WriteCell('\0', 0, x, 16);
							}
						}
					}
					break;
				case 'K':
					if (!paused) {
						RotateCounterClockwise();
					}
					break;
				case 'L':
					if (!paused) {
						RotateClockwise();
					}
					break;
				case VK_ESCAPE:
					exitGame = true;
					break;
				}
			}
		}
	}
}
void Game::Create(Renderer& renderer) {

	for (int y = 1; y < 29; y++) {
		renderer.WriteCell('#', FOREGROUND_INTENSITY, 27, y);
		renderer.WriteCell('#', FOREGROUND_INTENSITY, 28, y);
		renderer.WriteCell('#', FOREGROUND_INTENSITY, 49, y);
		renderer.WriteCell('#', FOREGROUND_INTENSITY, 50, y);
		renderer.WriteCell('#', FOREGROUND_INTENSITY, 70, y);
		renderer.WriteCell('#', FOREGROUND_INTENSITY, 71, y);
		renderer.WriteCell('#', FOREGROUND_INTENSITY, 92, y);
		renderer.WriteCell('#', FOREGROUND_INTENSITY, 93, y);
	}

	for (int x = 28; x < 93; x++) {
		renderer.WriteCell('#', FOREGROUND_INTENSITY, x, 1);
		renderer.WriteCell('#', FOREGROUND_INTENSITY, x, 28);
	}

	for (int x = 28; x < 70; x++) {
		renderer.WriteCell('#', FOREGROUND_INTENSITY, x, 5);
	}

	for (int x = 51; x < 70; x++) {
		renderer.WriteCell('#', FOREGROUND_INTENSITY, x, 6);
		renderer.WriteCell('#', FOREGROUND_INTENSITY, x, 27);
	}

	for (int x = 72; x < 92; x++) {
		renderer.WriteCell('#', FOREGROUND_INTENSITY, x, 11);
		renderer.WriteCell('#', FOREGROUND_INTENSITY, x, 20);
	}

	for (int i = 0; i < tetrominos.size(); i++) {
		WriteTetrominoToScreen(renderer, i, 32, 6 + 3 * i, 0);
	}
	UpdateStatistics(renderer);

	renderer.WriteText("STATISTICS", FOREGROUND_INTENSITY, 34, 3);
	renderer.WriteText("LINES -", FOREGROUND_INTENSITY, 55, 3);
	UpdateLines(renderer);
	renderer.WriteText("TOP", FOREGROUND_INTENSITY, 79, 4);
	if (top > 999999) {
		renderer.WriteText("999999", FOREGROUND_INTENSITY, 79, 5);
	}
	std::string sTop = std::to_string(top);
	while (sTop.size() < 6) {
		sTop = "0" + sTop;
	}
	renderer.WriteText(sTop, FOREGROUND_INTENSITY, 79, 5);
	renderer.WriteText("SCORE", FOREGROUND_INTENSITY, 79, 7);
	UpdateScore(renderer);
	renderer.WriteText("NEXT", FOREGROUND_INTENSITY, 79, 14);
	renderer.WriteText("LEVEL", FOREGROUND_INTENSITY, 79, 23);
	UpdateLevel(renderer);

	WriteTetrominoToScreen(renderer, next, 77, 15, 0);
	WriteTetrominoToBoard();

	previous = std::chrono::steady_clock::now();
}
void Game::Update(Renderer& renderer, HANDLE input) {

	if (!paused && !gameover) {

		auto now = std::chrono::steady_clock::now();
		double delta = std::chrono::duration<double, std::milli>(now - previous).count();
		previous = now;
		accumulator += delta;

		if (accumulator >= msPerTick) {
			accumulator -= msPerTick;
			if (CanMoveDown()) {
				MoveDown();
			}
			else {
				ClearLines(renderer);
				UpdateLines(renderer);
				UpdateScore(renderer);
				UpdateLevel(renderer);
				EraseTetrominoFromScreen(renderer, next, 77, 15, 0);
				GetNextTetromino();
				if (showNext) {
					WriteTetrominoToScreen(renderer, next, 77, 15, 0);
				}
				UpdateStatistics(renderer);
				gameover = IsGameOver();
				WriteTetrominoToBoard();
				FlushConsoleInputBuffer(input);
			}
		}

		UpdateBoard(renderer);

		if (gameover) {
			renderer.Refresh();
			Sleep(1000);
			for (int y = 7; y < 5 + HEIGHT; y++) {
				for (int x = 51; x < 70; x++) {
					renderer.WriteCell('=', FOREGROUND_INTENSITY, x, y);
				}
				renderer.Refresh();
				Sleep(100);
			}
			SaveHighScore();
		}
	}
}
bool Game::GetExit() {
	return exitGame;
}
void Game::Reset() {
	std::fill(board.begin(), board.end(), '\0');
	Init();
}

void Game::WriteTetrominoToScreen(Renderer& renderer, int t, int x, int y, int r) {

	if (t == 3 || t == 6) x++; // Shift O and I right
	if (t == 6) y++;           // Shift I down

	for (int i = 0; i < 16; i++) {
		if (tetrominos[t][r][i] != ' ') {
			renderer.WriteCell('0', color[t].second, x + i % 4 * 2, y + i / 4);
		}
	}
}
void Game::EraseTetrominoFromScreen(Renderer& renderer, int t, int x, int y, int r) {

	if (t == 3 || t == 6) x++; // Shift O and I right
	if (t == 6) y++;           // Shift I down

	for (int i = 0; i < 16; i++) {
		if (tetrominos[t][r][i] != ' ') {
			renderer.WriteCell('\0', 0, x + i % 4 * 2, y + i / 4);
		}
	}
}

void Game::UpdateBoard(Renderer& renderer) {
	for (int i = 2; i < HEIGHT; i++) {
		for (int j = 0; j < WIDTH; j++) {
			renderer.WriteCell('\0', 0, 51 + 2 * j, 5 + i);
			for (int k = 0; k < color.size(); k++) {
				if (board[i * WIDTH + j] == color[k].first) {
					renderer.WriteCell('0', color[k].second, 51 + 2 * j, 5 + i);
				}
			}
		}
	}
}

void Game::WriteTetrominoToBoard() {
	std::string tetromino = tetrominos[current][rotation];
	for (int i = 0; i < 16; i++) {
		if (tetromino[i] != ' ') {
			board[(yPos + i / 4) * WIDTH + xPos + i % 4] = tetromino[i];
		}
	}
}
void Game::EraseTetrominoFromBoard() {
	std::string tetromino = tetrominos[current][rotation];
	for (int i = 0; i < 16; i++) {
		if (tetromino[i] != ' ') {
			board[(yPos + i / 4) * WIDTH + xPos + i % 4] = '\0';
		}
	}
}

void Game::MoveRight() {

	EraseTetrominoFromBoard();

	xPos = xPos + 1;
	for (int i = 0; i < 16; i++) {
		if (tetrominos[current][rotation][i] != ' ') {
			if (xPos + i % 4 > WIDTH - 1 ||
				board[(yPos + i / 4) * WIDTH + (xPos + i % 4)] != '\0') {

				xPos = xPos - 1;
				break;
			}
		}
	}

	WriteTetrominoToBoard();
}
void Game::MoveDown() {

	EraseTetrominoFromBoard();

	yPos = yPos + 1;
	for (int i = 0; i < 16; i++) {
		if (tetrominos[current][rotation][i] != ' ') {
			if (yPos + i / 4 > HEIGHT - 1 ||
				board[(yPos + i / 4) * WIDTH + (xPos + i % 4)] != '\0') {

				yPos = yPos - 1;
				break;
			}
		}
	}

	WriteTetrominoToBoard();
}
void Game::MoveLeft() {

	EraseTetrominoFromBoard();

	xPos = xPos - 1;
	for (int i = 0; i < 16; i++) {
		if (tetrominos[current][rotation][i] != ' ') {
			if (xPos + i % 4 < 0 ||
				board[(yPos + i / 4) * WIDTH + (xPos + i % 4)] != '\0') {

				xPos = xPos + 1;
				break;
			}
		}
	}

	WriteTetrominoToBoard();
}
void Game::RotateClockwise() {

	EraseTetrominoFromBoard();

	rotation = (rotation + 1) % 4;
	for (int i = 0; i < 16; i++) {
		if (tetrominos[current][rotation][i] != ' ') {
			if (yPos + i / 4 > HEIGHT - 1 ||
				xPos + i % 4 < 0 ||
				xPos + i % 4 > WIDTH - 1 ||
				board[(yPos + i / 4) * WIDTH + (xPos + i % 4)] != '\0') {

				rotation = (rotation + 3) % 4;
				break;
			}
		}
	}

	WriteTetrominoToBoard();
}
void Game::RotateCounterClockwise() {

	EraseTetrominoFromBoard();

	rotation = (rotation + 3) % 4;
	for (int i = 0; i < 16; i++) {
		if (tetrominos[current][rotation][i] != ' ') {
			if (yPos + i / 4 > HEIGHT - 1 ||
				xPos + i % 4 < 0 ||
				xPos + i % 4 > WIDTH - 1 ||
				board[(yPos + i / 4) * WIDTH + (xPos + i % 4)] != '\0') {

				rotation = (rotation + 1) % 4;
				break;
			}
		}
	}

	WriteTetrominoToBoard();
}
bool Game::CanMoveDown() {

	EraseTetrominoFromBoard();

	bool canMoveDown = true;
	for (int i = 0; i < 16; i++) {
		if (tetrominos[current][rotation][i] != ' ') {
			if (yPos + i / 4 + 1 > HEIGHT - 1 ||
				board[(yPos + i / 4 + 1) * WIDTH + (xPos + i % 4)] != '\0') {
				canMoveDown = false;
				break;
			}
		}
	}

	WriteTetrominoToBoard();

	return canMoveDown;
}
bool Game::IsGameOver() {

	for (int i = 0; i < 16; i++) {
		if (tetrominos[current][rotation][i] != ' ') {
			if (board[(yPos + i / 4) * WIDTH + (xPos + i % 4)] != '\0') {
				return true;
			}
		}
	}
	return false;
}

void Game::ClearLines(Renderer& renderer) {
	std::vector<int> rows;
	for (int y = 0; y < HEIGHT; y++) {
		bool complete = true;
		for (int x = 0; x < WIDTH; x++) {
			if (board[y * WIDTH + x] == '\0') {
				complete = false;
				break;
			}
		}
		if (complete) {
			rows.push_back(y);
		}
	}

	for (int x = 0; x < WIDTH / 2; x++) {
		for (int i = 0; i < rows.size(); i++) {
			board[rows[i] * WIDTH + 4 - x] = '\0';
			board[rows[i] * WIDTH + 5 + x] = '\0';
		}
		UpdateBoard(renderer);
		renderer.Refresh();
		Sleep(100);
	}

	for (int i = 0; i < rows.size(); i++) {
		for (int y = rows[i]; y > 0 ; y--) {
			for (int x = 0; x < WIDTH; x++) {
				board[y * WIDTH + x] = board[(y - 1) * WIDTH + x];
				board[(y - 1) * WIDTH + x] = '\0';
			}
		}
	}
	
	lines += rows.size();

	if (rows.size() == 4) {
		score += 1200 * (level + 1);
	}
	if (rows.size() == 3) {
		score += 300 * (level + 1);
	}
	if (rows.size() == 2) {
		score += 100 * (level + 1);
	}
	if (rows.size() == 1) {
		score += 40 * (level + 1);
	}
}
void Game::UpdateLines(Renderer& renderer) {
	if (lines > 999) {
		renderer.WriteText("999", FOREGROUND_INTENSITY, 63, 3);
	}
	std::string sLines = std::to_string(lines);
	while (sLines.size() < 3) {
		sLines = "0" + sLines;
	}
	renderer.WriteText(sLines, FOREGROUND_INTENSITY, 63, 3);
}
void Game::UpdateScore(Renderer& renderer) {
	if (score > 999999) {
		renderer.WriteText("999999", FOREGROUND_INTENSITY, 79, 8);
	}
	std::string sScore = std::to_string(score);
	while (sScore.size() < 6) {
		sScore = "0" + sScore;
	}
	renderer.WriteText(sScore, FOREGROUND_INTENSITY, 79, 8);
}
void Game::UpdateLevel(Renderer& renderer) {
	int previousLevel = level;
	level = lines / 10;
	if (level > previousLevel) {
		if (level < 9) {
			speed = speed - 5;
		}
		if (level == 9) {
			speed = speed - 2;
		}
		if (level == 10 ||
			level == 13 ||
			level == 16 ||
			level == 19 ||
			level == 29) {
			speed = speed - 1;
		}
		msPerTick = (speed / 60.0) * 1000.0;
	}
	std::string sLevel = std::to_string(level);
	while (sLevel.size() < 2) {
		sLevel = "0" + sLevel;
	}
	renderer.WriteText(sLevel, FOREGROUND_INTENSITY, 81, 25);
}
void Game::UpdateStatistics(Renderer& renderer) {
	statistics[current]++;
	for (int i = 0; i < statistics.size(); i++) {
		if (statistics[i] > 999) {
			renderer.WriteText("999", FOREGROUND_INTENSITY, 42, 8 + 3 * i);
		}
		std::string count = std::to_string(statistics[i]);
		while (count.size() < 3) {
			count = "0" + count;
		}
		renderer.WriteText(count, FOREGROUND_INTENSITY, 42, 8 + 3 * i);
	}
}
void Game::GetNextTetromino() {
	current = next;
	next = rand() % tetrominos.size();
	rotation = 0;
	xPos = 3;
	yPos = 1;
}

void Game::LoadHighScore() {
	std::ifstream f("top.txt");
	if (f.is_open()) {
		f >> top;
	}
	else {
		top = 0;
	}
	f.close();
}
void Game::SaveHighScore() {
	if (score > top) {
		std::ofstream f("top.txt");
		f << score;
	}
}
void Game::Init() {
	current = rand() % tetrominos.size();
	next = rand() % tetrominos.size();
	rotation = 0;
	xPos = 3;
	yPos = 1;

	exitGame = false;
	gameover = false;
	paused = false;
	showNext = true;
	LoadHighScore();
	score = 0;
	lines = 0;
	level = 0;
	speed = 48;
	msPerTick = (speed / 60.0) * 1000.0;
	accumulator = 0.0;
	previous = std::chrono::steady_clock::now();
	statistics = { 0, 0, 0, 0, 0, 0, 0 };
}