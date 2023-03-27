#ifndef GLOBALS_H
#define GLOBALS_H

#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL.h>
#include <iostream>
#include <deque> // For snake body
// #include <vector>
#include <chrono> // timing
#include <thread>

// For grid to align, GRID_CELL_SIZE must divide evenly into both SCREEN_W and SCREEN_H

#define SCREEN_W 1280
#define SCREEN_H 720
#define GRID_CELL_SIZE 20
// #define GRID_CELL_SIZE 40

#define INPUT_CD 60 // Input cooldown (prevents multiple direction changes in a single tick)

#define FPS 60

// Colors 
#define LIGHT_BLUE 0x3E7C9E
#define GREEN 0x04C70E
#define RED 0xAD2300
#define GREY 0x474747

extern bool game_over;

// Returns true if the two SDL_Rect objects overlap
bool checkCollision(SDL_Rect a, SDL_Rect b); 

SDL_Color hexToColor(unsigned long hex_color);

typedef enum MoveDir {
	M_LEFT,
	M_DOWN, 
	M_RIGHT, 
	M_UP,
} MoveDir;

#endif // GLOBALS_H
