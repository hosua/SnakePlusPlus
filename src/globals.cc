#include "globals.h"

std::shared_ptr<GameMaster> g_master = nullptr; // Global game master

// Convert hexademical # to SDL_Color struct 
SDL_Color hexToColor(unsigned long hex_color){
	SDL_Color color;
	color.r = ((hex_color >> 16) & 0xFF); // Extract first two bytes
	color.g = ((hex_color >> 8) & 0xFF); // Extract middle two bytes
	color.b = ((hex_color) & 0xFF); // Extract last two bytes
	color.a = 255;
	return color;
}

// Returns true if both rect objects overlap
bool checkCollision(SDL_Rect a, SDL_Rect b){
	int a_left, b_left;
	int a_right, b_right;
	int a_bottom, b_bottom;
	int a_top, b_top;

	a_left = a.x; b_left = b.x;
	a_right = a.x+a.w; b_right = b.x+b.w;
	a_top = a.y; b_top = b.y;
	a_bottom = a.y+a.h; b_bottom = b.y+b.h;
	
	if (a_bottom <= b_top || 
			a_top >= b_bottom || 
			a_right <= b_left || 
			a_left >= b_right)
		return false;

	return true;

}

void initFonts(){
	TTF_Init(); // Must always be called before using SDL_ttf API
	
	g_master->fonts = std::vector<TTF_Font*>(NUM_FONTS, nullptr);

	g_master->fonts[0] = TTF_OpenFont(FONT_PATH, FONT_SIZE_SMALL);
	g_master->fonts[1] = TTF_OpenFont(FONT_PATH, FONT_SIZE_MED);
	g_master->fonts[2] = TTF_OpenFont(FONT_PATH, FONT_SIZE_LARGE);

	if (!g_master->fonts[0] || !g_master->fonts[1] || !g_master->fonts[2]){
		std::cerr << "Fatal Error: Font file \"" << FONT_PATH << "\" could not be found.\n";
		g_master->is_running = false;
	}
}
