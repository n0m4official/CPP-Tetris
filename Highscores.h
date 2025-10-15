#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <sstream>

#include "ConsoleUtils.h"

// --- Highscore entry struct ---
struct HighscoreEntry {
    std::string name;
    int score;
};

// --- Load highscores from file ---
std::vector<HighscoreEntry> loadHighscores(const std::string& filename);

// --- Save highscores to file ---
void saveHighscores(const std::vector<HighscoreEntry>& highscores, const std::string& filename);

// --- Add a new highscore entry (keep top 10) ---
void addHighscore(std::vector<HighscoreEntry>& highscores, const HighscoreEntry& newEntry);

// --- Display highscores centered in console ---
void showHighscores(const std::vector<HighscoreEntry>& highscores, const std::string& latestName);
