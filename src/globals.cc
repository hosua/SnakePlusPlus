#include "globals.h"
bool game_over = false;

// Convert hexademical # to SDL_Color struct 
SDL_Color hexToColor(unsigned long hex_color){
	SDL_Color color;
	color.r = ((hex_color >> 16) & 0xFF); // Extract first two bytes
	color.g = ((hex_color >> 8) & 0xFF); // Extract middle two bytes
	color.b = ((hex_color) & 0xFF); // Extract last two bytes
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


