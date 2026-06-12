# Tetris

A console-based Tetris clone for Windows, built in C++ using Win32 APIs.

<img width="1110" height="587" alt="Screenshot 2026-06-09 215500" src="https://github.com/user-attachments/assets/a9da1ed0-091e-48a2-8502-d012fe19c336" />

## Download

Grab the Console Tetris folder and run Tetris.exe. The .exe is unsigned so Windows may pop up with a warning, but it is safe to run anyway.

## Building from Source

**Requirements**

- Windows
- Visual Studio 2022 (or later) with the Desktop development with C++ workload

**Steps**
1. Clone the repository
2. Open `Tetris.sln` in Visual Studio
3. Set the build configuration to **Release** and platform to **x64**
4. Build → Build Solution (`Ctrl+Shift+B`)

The executable will be output to `x64/Release/Tetris.exe`.

## How to Play

Use the controls below to play.

| Key | Action |
|-----|--------|
| A | Move left |
| D | Move right |
| S | Move down |
| K | Rotate counterclockwise |
| L | Rotate clockwise / Confirm |
| Enter | Confirm |
| G | Hide / show next piece |
| H | Start / Pause |
| Esc | Quit |

**Scoring** follows the original NES Tetris system:

| Lines Cleared | Points |
|---------------|--------|
| 1 | 40 × (level + 1) |
| 2 | 100 × (level + 1) |
| 3 | 300 × (level + 1) |
| 4 (Tetris) | 1200 × (level + 1) |

Soft-dropping a piece (holding **S**) earns 1 bonus point per row.

The high score is saved to `top.txt` in the same directory as the executable.

## License

[MIT](LICENSE)
