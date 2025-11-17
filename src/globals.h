#ifndef GLOBALS_H
#define GLOBALS_H

#include <assert.h> // For rror checking

#include <SDL2/SDL.h> // SDL2 library
#include <SDL2/SDL_scancode.h> // For reading I/O device input
#include <SDL2/SDL_ttf.h> // For using fonts in SDL
#include <SDL2/SDL_mixer.h> // For sounds
#include <SDL2/SDL_image.h> // For blitting images

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

#include "sounds.h"
#include "save.h"

#define GAME_VERSION "v0.0.4"

// To ensure grid contains only full squares, GRID_CELL_SIZE must divide evenly into 
// both SCREEN_W and SCREEN_H
#define SCREEN_W 1280
#define SCREEN_H 720
#define GRID_CELL_SIZE 20

#define FPS 60

#define CD_LENGTH 3 // # of seconds that will elapse before game starts/resumes

// Colors 
#define BLACK 0x000000
#define WHITE 0xFFFFFF
#define LIGHT_BLUE 0x3E7C9E
#define SKY_BLUE 0x2F9CEB
#define GREEN 0x04C70E
#define RED 0xAD2300
#define GREY 0x474747

// Font stuff
#define FONT_PATH "assets/fonts/pixel-letters.ttf"

#define FONT_SIZE_SMALL 30
#define FONT_SIZE_MED 60
#define FONT_SIZE_LARGE 90

#define NUM_FONTS 3 // Number of different fonts used

// Enums used to identify and index fonts in gm->fonts list
typedef enum FontType {
	F_SMALL,
	F_MED,
	F_LARGE,
} FontType;

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

// Encapsulate important game data into GameMaster
struct GameMaster {
	GameState gstate;
	std::string buff_str; // string buffer for displaying high score in main menu
	int level;
	int option; // Determines the length of each frame. Lower option value == higher difficulty,
				// negative numbers are reserved for menu options.
	bool reset; // If true, game should resett all objects (snake & food) and global variables
	bool is_running; // Program should immediately exit if this variable is false
	bool game_over; // Player lost, game should return to main menu
	bool is_paused; // Game is currently running, but paused

	bool cd_started; // Indicate the cooldown is starting
	int cd_counter; // Keeps track of how many seconds remain before gameplay resumes/starts
	std::vector<TTF_Font*> fonts;
	bool explosion_sound_played; // Track if explosion sound has been played for current game over

	void resetGame(){ gstate = GS_MAINMENU; reset = game_over, is_paused = false; explosion_sound_played = false; }

	void startCD(){ // Should call this function when game starts or resumes from pause
		is_paused = false;
		cd_counter = CD_LENGTH;
		cd_started = true;
	}

	GameMaster(): gstate(GS_MAINMENU), buff_str(""), level(0), reset(false), is_running(true), game_over(false), 
	is_paused(false), cd_started(false), cd_counter(0), explosion_sound_played(false){}
};

extern std::unique_ptr<GameMaster> g_gamemaster; // Global game master
extern std::unique_ptr<SoundMaster> g_soundmaster; // Global sound master

void resetGame(GameMaster* gm); // Reset game master variables for new game

// Call this before doing anything with fonts or else
// Returns false if anything fails during initialization
bool initFonts();

// Returns true if the two SDL_Rect objects overlap
bool checkCollision(SDL_Rect a, SDL_Rect b); 

bool stringIsInt(std::string s); 

#endif // GLOBALS_H
