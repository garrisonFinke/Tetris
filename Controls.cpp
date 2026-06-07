#include "Controls.h"

Controls::Controls() {
	exit = false;
}

void Controls::Write(Renderer& renderer) {
	renderer.WriteText("   CONTROLS", FOREGROUND_INTENSITY, 53, 8);
	renderer.WriteText("  A      MOVE LEFT", FOREGROUND_INTENSITY, 53, 10);
	renderer.WriteText("  D      MOVE RIGHT", FOREGROUND_INTENSITY, 53, 11);
	renderer.WriteText("  S      MOVE DOWN", FOREGROUND_INTENSITY, 53, 12);
	renderer.WriteText("  G      HIDE NEXT", FOREGROUND_INTENSITY, 53, 13);
	renderer.WriteText("  H      START / PAUSE", FOREGROUND_INTENSITY, 53, 14);
	renderer.WriteText("  K      ROTATE COUNTERCLOCKWISE", FOREGROUND_INTENSITY, 53, 15);
	renderer.WriteText("  L      ROTATE CLOCKWISE / CONFIRM", FOREGROUND_INTENSITY, 53, 16);
	renderer.WriteText("ENTER    CONFIRM", FOREGROUND_INTENSITY, 53, 17);
	renderer.WriteText(" ESC     QUIT", FOREGROUND_INTENSITY, 53, 18);

	renderer.WriteText(" CLOSE ", BACKGROUND_INTENSITY, 56, 20);
}
void Controls::HandleInput(HANDLE input) {
	INPUT_RECORD record;
	DWORD eventsRead;
	while (PeekConsoleInput(input, &record, 1, &eventsRead) && eventsRead) {
		ReadConsoleInput(input, &record, 1, &eventsRead);
		if (record.EventType == KEY_EVENT) {
			auto& key = record.Event.KeyEvent;
			if (key.bKeyDown) {
				switch (key.wVirtualKeyCode) {
				case VK_ESCAPE:
				case 'L':
				case VK_RETURN:
					exit = true;
					break;
				}
			}
		}
	}
}
bool Controls::getExit() {
	return exit;
}
void Controls::Reset() {
	exit = false;
}