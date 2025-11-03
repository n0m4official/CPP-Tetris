#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <sstream>

#include "ConsoleUtils.h"

// If you can read this, you’re officially in the highscore zone.
// May your score be ever higher than your caffeine intake.

// --- Highscore entry struct ---
struct HighscoreEntry {
    std::string name;
    int score;
    // Name must be short, like my patience for cheaters.
};

// --- Load highscores from file ---
// If this fails, just pretend you’re #1.
std::vector<HighscoreEntry> loadHighscores(const std::string& filename);

// --- Save highscores to file ---
// If your score isn’t saved, it’s not my fault. Blame Murphy’s Law.
void saveHighscores(const std::vector<HighscoreEntry>& highscores, const std::string& filename);

// --- Add a new highscore entry (keep top 10) ---
// Only the best survive. Sorry, 11th place.
void addHighscore(std::vector<HighscoreEntry>& highscores, const HighscoreEntry& newEntry);

// --- Display highscores centered in console ---
// Centering: because your ego should be, too.
void showHighscores(const std::vector<HighscoreEntry>& highscores, const std::string& latestName);
