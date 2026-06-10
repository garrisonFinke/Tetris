#pragma once
#include "Renderer.h"
#include <vector>
#include <chrono>

class Game {

public:

	Game();

	void HandleInput(HANDLE input, Renderer& renderer);
	void Create(Renderer& renderer);
	void Update(Renderer& renderer);
	bool GetExit();
	void Reset();

private:

	static constexpr int WIDTH = 10;
	static constexpr int HEIGHT = 22;
	std::vector<char> board;

	std::chrono::steady_clock::time_point previous;
	double accumulator = 0.0;

	int current;
	int next;
	int rotation;
	int xPos;
	int yPos;

	bool exitGame;
	bool gameover;
	bool paused;
	bool showNext;
	int top;
	int score;
	int lines;
	int level;
	int speed;
	double msPerTick;
	std::vector<int> statistics;

	void WriteTetrominoToScreen(Renderer& renderer, int t, int x, int y, int r);
	void EraseTetrominoFromScreen(Renderer& renderer, int t, int x, int y, int r);

	void UpdateBoard(Renderer& renderer);

	void WriteTetrominoToBoard();
	void EraseTetrominoFromBoard();

	void MoveRight();
	void MoveDown();
	void MoveLeft();
	void RotateClockwise();
	void RotateCounterClockwise();

	bool CanMoveDown();
	bool IsGameOver();

	void ClearLines(Renderer& renderer);
	void UpdateLines(Renderer& renderer);
	void UpdateScore(Renderer& renderer);
	void UpdateLevel(Renderer& renderer);
	void UpdateStatistics(Renderer& renderer);
	void GetNextTetromino();

	void LoadHighScore();
	void SaveHighScore();
	void Init();
};


