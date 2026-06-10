#define NOMINMAX
#include "Tetris.h"
#include <chrono>
#include <fstream>

Tetris::Tetris() :
	renderer(120, 30) {
	
	input = GetStdHandle(STD_INPUT_HANDLE);
}
Tetris::~Tetris() {
	CloseHandle(input);
}

void Tetris::Run() {
	while (true) {


		renderer.Wipe();
		title.Create(renderer);

		while (title.GetSelected() == -1) {
			title.HandleInput(input);
			title.Update(renderer);
			renderer.Refresh();
		}


		if (title.GetSelected() == 0) {
			
			renderer.Wipe();
			game.Create(renderer);

			while (!game.GetExit()) {
				game.HandleInput(input, renderer);
				game.Update(renderer);
				renderer.Refresh();
			}

			game.Reset();
		}


		if (title.GetSelected() == 1) {

			renderer.Wipe();
			controls.Create(renderer);

			while (!controls.GetExit()) {
				controls.HandleInput(input);
				renderer.Refresh();
			}

			controls.Reset();
		}


		if (title.GetSelected() == 2) {
			break;
		}

		title.Reset();
	}
}
