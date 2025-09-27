# CPP-Tetris

A simple Tetris clone built in C++ for the Windows console. Includes basic gameplay, scoring, levels, line clearing, a start menu, and a preview of the next piece.

> **Note:** Tetris® is a registered trademark of The Tetris Company. This project is a fan-made clone for educational purposes only and is not affiliated with or endorsed by The Tetris Company.

---

## Features

- **Classic Tetris Gameplay:** Enjoy the familiar mechanics of Tetris, including piece rotation, movement, and line clearing.
- **Scoring System:** Rack up points for clearing lines and try to beat your high score.
- **Level Progression:** Challenge yourself as the game speeds up with each level.
- **Start Menu:** Navigate a simple start menu to begin your game.
- **Next Piece Preview:** See which tetromino is coming next to plan your moves.
- **Console Interface:** Runs entirely in the Windows console for a retro experience.

## Screenshots

<img width="1440" height="657" alt="image" src="https://github.com/user-attachments/assets/fe0b254f-9785-4b60-803e-9f7a51da7879" />

<img width="1429" height="699" alt="image" src="https://github.com/user-attachments/assets/b5865ada-4abb-413e-b7e3-a919ccd03852" />

## Getting Started

### Prerequisites

- Windows OS
- C++ compiler (such as MinGW, MSVC/Visual Studio, etc.)

### Building the Project

1. **Clone the repository:**
    ```bash
    git clone https://github.com/n0m4official/CPP-Tetris.git
    cd CPP-Tetris
    ```

2. **Compile the code:**

    If using g++ (MinGW):
    ```bash
    g++ -o tetris main.cpp
    ```

3. **Run the game:**
    ```bash
    tetris.exe
    ```

### Controls

| Key            | Action               |
|----------------|---------------------|
| a    | Move left           |
| d   | Move right          |
| s     | Soft drop           |
| w       | Rotate piece        |
| Spacebar       | Hard drop|
| p              | Pause   |
| c             | Hold piece    |
| q           | Quit                |

## Project Structure

```
CPP-Tetris/
├── Release/
|    ├── Tetris.exe
|    └── Tetris.pdb
├── .gitattributes
├── .gitignore
├── README.md
├── Tetris.sln
├── Tetris.vcxproj
├── Tetris.vcxproj.filters
└── main.cpp
```

## License

This project is for educational purposes only and is not affiliated with or endorsed by The Tetris Company.

## Acknowledgements

- Inspired by the original [Tetris](https://tetris.com/) game.
- Tetris® is a registered trademark of The Tetris Company.

---

Happy stacking!
