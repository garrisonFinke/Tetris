#pragma once
#include "Tetrominos.h"
#include "Title.h"
#include "Game.h"
#include "Controls.h"


class Tetris {

public:

	Tetris();
	~Tetris();

	void Run();

private:

	Renderer renderer;
	HANDLE input;
	Title title;
	Game game;
	Controls controls;
};