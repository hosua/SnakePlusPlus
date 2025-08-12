#ifndef SAVE_H
#define SAVE_H

#include <array>
#include <memory>
#include <iostream>
#include <fstream>
#include <cstdint>

#define SAVE_PATH "save.bin"
#define NUM_DIFFS 10 // Number of difficulties

struct SaveData {
	std::array<uint16_t, NUM_DIFFS> _bank; // Data bank (high scores for each level stored here)
} __attribute__((packed)); // https://cplusplus.com/forum/general/278865/

extern std::unique_ptr<SaveData> g_savedata;
int getHighScore(int level); // Returns the high score of the specified level
void saveInit(); // Initialize SaveData struct with zeros and creates save.bin if it does not already exist
void saveUpdate(int level, int score); // After game over, checks score for the level, and updates save data 
				   // if score is higher than before
std::unique_ptr<SaveData> saveLoad();  // Returns nullptr if there was no file to load

#endif // SAVE_H
