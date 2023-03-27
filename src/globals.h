#ifndef GLOBALS_H
#define GLOBALS_H

#include <assert.h> // For rror checking

#include <SDL2/SDL.h> // SDL2 library
#include <SDL2/SDL_scancode.h> // For reading I/O device input
#include <SDL2/SDL_ttf.h> // For using fonts in SDL

#include <iostream> // For printing
 
#include <memory> // For smart pointers

// Collections
#include <utility> // For std::pair
#include <array>
#include <deque> // For snake body
#include <vector>
// #include <vector>
#include <chrono> // timing
#include <thread>


// Font stuff
#define FONT_PATH "assets/fonts/pixel-letters.ttf"

#define FONT_SIZE_SMALL 30
#define FONT_SIZE_MED 60
#define FONT_SIZE_LARGE 90

// Global array to store font structs
extern std::array<TTF_Font*, 3> g_fonts;

// Enum used to index font in g_fonts array
typedef enum FontType {
	F_SMALL,
	F_MED,
	F_LARGE,
} FontType;

// Call this before doing anything with fonts or else
void initFonts();

// For grid to align, GRID_CELL_SIZE must divide evenly into both SCREEN_W and SCREEN_H
#define SCREEN_W 1280
#define SCREEN_H 720
#define GRID_CELL_SIZE 20
// #define GRID_CELL_SIZE 40

#define INPUT_CD 60 // Input cooldown (prevents multiple direction changes in a single tick)

#define FPS 60

// Colors 
#define BLACK 0x000000
#define WHITE 0xFFFFFF
#define LIGHT_BLUE 0x3E7C9E
#define SKY_BLUE 0x2F9CEB
#define GREEN 0x04C70E
#define RED 0xAD2300
#define GREY 0x474747

extern bool g_master_reset; // If true, game should handle resetting all objects (snake and food)
extern bool g_is_running; // Program should immediately exit if this variable is false
extern bool g_game_over; // Player lost, game should return to main menu
extern bool g_is_paused; // Game is currently in progress, but paused
						 
void resetGlobals(); // Reset globals to original values (for resetting the game)

extern int g_diff; // Game difficulty (1-10) (Lower == harder)

// Returns true if the two SDL_Rect objects overlap
bool checkCollision(SDL_Rect a, SDL_Rect b); 

SDL_Color hexToColor(unsigned long hex_color);

typedef enum MoveDir {
	M_LEFT,
	M_DOWN, 
	M_RIGHT, 
	M_UP,
} MoveDir;

typedef enum GameState {
	GS_MAINMENU, // Main menu should render difficulty selection
	GS_INGAME, 
} GameState;

// Pause menu action enums
typedef enum PauseAction {
	PA_RESUME = -2,
	PA_MAINMENU = -1,
	PA_QUIT = 0,
} PauseAction;

extern GameState g_game_state;

#endif // GLOBALS_H
