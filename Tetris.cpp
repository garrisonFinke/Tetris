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
		title.Write(renderer);

		while (title.getSelected() == -1) {
			title.HandleInput(input);
			title.Update(renderer);
			renderer.Refresh();
		}


		if (title.getSelected() == 0) {
			
			renderer.Wipe();
			game.Write(renderer);

			while (!game.getExit()) {
				game.HandleInput(input, renderer);
				game.Update(renderer);
				renderer.Refresh();
			}

			game.Reset();
		}


		if (title.getSelected() == 1) {

			renderer.Wipe();
			controls.Write(renderer);

			while (!controls.getExit()) {
				controls.HandleInput(input);
				renderer.Refresh();
			}

			controls.Reset();
		}


		if (title.getSelected() == 2) {
			break;
		}

		title.Reset();
	}
}
