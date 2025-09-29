# CPP-Tetris

A polished, feature-rich Tetris® clone for the Windows console, written in modern C++.  
**Created by [n0m4official](https://github.com/n0m4official)**

---

## 🎮 Features

- **Classic Tetris gameplay** — 7 standard tetrominoes, line clearing, scoring, and leveling.
- **Modern Tetris enhancements:**
  - **Ghost Piece** — Preview exactly where your piece will land.
  - **Hold Piece** — Swap the current piece for later use, with a visually displayed hold box.
  - **Next Piece Preview** — See which tetromino comes up next, in a colored box.
- **Colorful, Centered Console UI:**
  - Full ANSI color support for each piece.
  - Board, next, and hold boxes are dynamically centered in your console window.
  - Flicker-minimized drawing using Windows console APIs.
- **Menus & User Experience:**
  - Start menu with controls and animated prompt.
  - Pause menu with dimmed-out board and resume/quit options.
  - Game over and exit screens, all with stylish, centered, colored text.
- **Responsive Controls:**  
  - Non-blocking input (move, rotate, drop, hold, pause, quit) — no waiting between moves!
- **Classic Scoring & Levels:**  
  - Scoring matches the original NES Tetris (single, double, triple, Tetris).
  - Level increases every 10 lines, speeding up the game.
- **Well-Commented, Modular Source Code:**  
  - Designed for readability, learning, and extension.

---

## 🖥️ Screenshots

<p align="center">
  <img src="docs/Screenshot 2025-09-27 170408.png" width="520" alt="Tetris Start Menu">
  <br>
  <em>Start menu</em>
</p>

<p align="center">
  <img src="docs/Screenshot 2025-09-27 170434.png" width="520" alt="Tetris Gameplay">
  <br>
  <em>In-game: Board, next, and hold preview</em>
</p>

<p align="center">
  <img src="docs/Screenshot 2025-09-27 170851.png" width="520" alt="Tetris Paused">
  <br>
  <em>Paused game with dimmed board</em>
</p>

<p align="center">
  <img src="docs/Screenshot 2025-09-27 170907.png" width="520" alt="Tetris Game Over">
  <br>
  <em>Game Over menu</em>
</p>

---

## 🕹️ Controls

| Key      | Action                                  |
|----------|-----------------------------------------|
| **A/D**  | Move left/right                         |
| **W**    | Rotate piece                            |
| **S**    | Soft drop (move piece down)             |
| **Space**| Hard drop (instantly drop piece)        |
| **C**    | Hold current piece / swap with hold box |
| **P**    | Pause / Resume                          |
| **Q**    | Quit (from game or pause menu)          |
| **R**    | Restart (from game over menu)           |

---

## 🚀 How to Build & Run

### Requirements

- Windows OS (uses Windows Console API and `<conio.h>`)
- A C++ compiler (Visual Studio recommended)

### Build Instructions

1. Clone this repo:
    ```sh
    git clone https://github.com/n0m4official/CPP-Tetris.git
    cd CPP-Tetris
    ```

2. Build `main.cpp`:
    - **Using Visual Studio:**  
      Open the folder and build the project.
    - **Using g++ (MinGW):**
      ```sh
      g++ main.cpp -o Tetris.exe -std=c++17 -static -luser32 -lkernel32
      ```

3. Run:
    ```sh
    ./Tetris.exe
    ```

---

## 🧑‍💻 Code Structure

- **main.cpp** — All game logic, board, input, drawing, and menus in one modular, well-documented file.

Key modules & functions:
- `Board` class: Board state, collision, line clearing, scoring.
- Tetrominoes: 7-piece system, rotation, spawn, ghost calculation.
- Menus: Start, pause (with dim effect!), game over, ragequit.
- Drawing: Board, next, hold, and interactive text, all centered using Windows console API.
- Input: Non-blocking keyboard handling for smooth gameplay.

---

## ✨ What Sets This Project Apart?

- **Console UI polish:**  
  Dynamically centers the game in your console, uses real ANSI color, and avoids flicker.
- **Modern gameplay:**  
  Ghost piece + hold functionality + next preview — rare for console Tetris!
- **Menus and prompt animations:**  
  Start, pause, and end menus with visually pleasing, interactive prompts.
- **Commented and maintainable:**  
  Source code is full of helpful, honest comments for easier learning and extension.

---

## ⚠️ Disclaimer

Tetris® is a registered trademark of The Tetris Company.  
This project is a fan-made clone for educational purposes only and is **not affiliated with or endorsed by The Tetris Company**.

---

## 📜 License

[MIT License](LICENSE)

---

*Have fun! Feel free to open issues or PRs for suggestions, bug fixes, or new features!*
