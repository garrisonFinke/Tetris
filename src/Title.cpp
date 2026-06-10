#include "Title.h"

Title::Title() {
	hovered = 0;
	selected = -1;
}

void Title::HandleInput(HANDLE input) {
	INPUT_RECORD record;
	DWORD eventsRead;
	while (PeekConsoleInput(input, &record, 1, &eventsRead) && eventsRead) {
		ReadConsoleInput(input, &record, 1, &eventsRead);
		if (record.EventType == KEY_EVENT) {
			auto& key = record.Event.KeyEvent;
			if (key.bKeyDown) {
				switch (key.wVirtualKeyCode) {
				case VK_DOWN:
				case 'S':
					hovered = (hovered + 1) % options.size();
					break;
				case VK_UP:
				case 'W':
					hovered = (hovered + options.size() - 1) % options.size();
					break;
				case VK_RETURN:
				case 'L':
					selected = hovered;
					break;
				case 'H':
					selected = 0;
					break;
				case VK_ESCAPE:
					selected = 2;
					break;
				}
			}
		}
	}
}

void Title::WriteBigLetter(Renderer& renderer, std::string letter, WORD color, int x, int y) {
	for (int i = 0; i < letter.size(); i++) {
		renderer.WriteCell(letter[i], color, x + i % 10, y + i / 10);
	}
}
void Title::Create(Renderer& renderer) {
	WriteBigLetter(renderer, bigT, FOREGROUND_RED, 25, 7);
	WriteBigLetter(renderer, bigE, FOREGROUND_RED | FOREGROUND_GREEN, 37, 7);
	WriteBigLetter(renderer, bigT, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY, 49, 7);
	WriteBigLetter(renderer, bigR, FOREGROUND_GREEN | FOREGROUND_INTENSITY, 61, 7);
	WriteBigLetter(renderer, bigI, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY, 73, 7);
	WriteBigLetter(renderer, bigS, FOREGROUND_BLUE | FOREGROUND_RED, 85, 7);
}

void Title::Update(Renderer& renderer) {
	for (int i = 0; i < options.size(); i++) {
		WORD color = FOREGROUND_INTENSITY;
		if (i == hovered) {
			color = BACKGROUND_INTENSITY;
		}
		renderer.WriteText(options[i], color, 57, 16 + 2 * i);
	}
}

int Title::GetSelected() {
	return selected;
}

void Title::Reset() {
	selected = -1;
}