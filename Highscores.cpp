#include "Highscores.h"

// Welcome to the land of high scores.
// If you’re not on this list, try again. Or bribe the developer.

// --- Load highscores from file ---
std::vector<HighscoreEntry> loadHighscores(const std::string& filename) {
    std::vector<HighscoreEntry> highscores;
    std::ifstream file(filename);
    if (!file.is_open()) return highscores; // File not found? You must be new here.

    HighscoreEntry entry;
    while (file >> entry.name >> entry.score) {
        if (entry.name.length() > 3) {
            entry.name = entry.name.substr(0, 3); // Name too long? Truncate like my attention span.
        }
        highscores.push_back(entry);
    }

    return highscores;
}

// --- Save highscores to file ---
void saveHighscores(const std::vector<HighscoreEntry>& highscores, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) return; // If this fails, just tell everyone you’re #1.

    for (const auto& entry : highscores) {
        file << entry.name << " " << entry.score << "\n";
        // If you’re reading this file, congrats on hacking the leaderboard.
    }
}

// --- Add a new highscore entry ---
void addHighscore(std::vector<HighscoreEntry>& highscores, const HighscoreEntry& newEntry) {
    highscores.push_back(newEntry);

    std::sort(highscores.begin(), highscores.end(), [](const HighscoreEntry& a, const HighscoreEntry& b) {
        return a.score > b.score; // descending order, like my mood after losing
        });

    if (highscores.size() > 10) {
        highscores.resize(10); // keep top 10, because 11 is just too mainstream
    }
}

// --- Display highscores centered in console ---
void showHighscores(const std::vector<HighscoreEntry>& highscores, const std::string& latestName) {
    system("cls");
    resetCursor();

    // If you’re not on this list, you’re not trying hard enough.
    std::vector<std::string> lines;
    lines.push_back("=== HIGHSCORES ===");
    lines.push_back("Rank | Name     | Score");

    int rank = 1;
    for (const auto& entry : highscores) {
        std::ostringstream line;
        line << rank << "    | "
            << entry.name << (entry.name.length() < 8 ? std::string(8 - entry.name.length(), ' ') : "")
            << " | " << entry.score;

        if (!latestName.empty() && entry.name == latestName) {
            lines.push_back("\033[1;32m" + line.str() + "\033[0m"); // highlight latest entry, because you’re special
        }
        else {
            lines.push_back(line.str());
        }

        rank++;
        if (rank > 10) break; // Only the chosen ten
    }

    lines.push_back("\nPress ENTER to return...");

    int startY = 5;
    for (size_t i = 0; i < lines.size(); i++) {
        printCentered(lines[i], startY + static_cast<int>(i));
    }

    // Wait for user to accept their fate
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}
