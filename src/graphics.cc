#include "graphics.h"

void GFX::init(){

	_window = SDL_CreateWindow("Snake++", 
			SDL_WINDOWPOS_CENTERED, 
			SDL_WINDOWPOS_CENTERED, 
			SCREEN_W, SCREEN_H, 0);

	if (!_window){
		fprintf(stderr, "Fatal Error: Window failed to initialize\n");
		fprintf(stderr, "SDL2 Error: %s\n", SDL_GetError());
		cleanQuit(false);
	}

	_renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED);

	_texture = NULL;

	_surface = SDL_GetWindowSurface(_window);

	if (!_surface){
		fprintf(stderr, "Failed to get _surface from _window.\n");
		fprintf(stderr, "SDL2 Error: %s\n", SDL_GetError());
		cleanQuit(false);
	}

	// Allow window to be resizable
	SDL_SetWindowResizable(_window, SDL_TRUE);
	// Allow resolution independence
	SDL_RenderSetLogicalSize(_renderer, SCREEN_W, SCREEN_H);
	// Allows for transparent rendering
	SDL_SetRenderDrawBlendMode(_renderer, SDL_BLENDMODE_BLEND);
}

void GFX::cleanQuit(bool success) const {
	printf("Quitting, goodbye!\n");
	SDL_DestroyRenderer(_renderer);
	SDL_DestroyWindow(_window);
	SDL_Quit();
	return (success) ? exit(EXIT_SUCCESS) : exit(EXIT_FAILURE);
}

void GFX::renderClear() const {
	SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255); // draw black screen
	SDL_RenderClear(_renderer);
}

void GFX::renderGrid() const {
	SDL_Color color = hexToColor(GREY);
	SDL_SetRenderDrawColor(_renderer, color.r, color.g, color.b, 255);
	for (int y = 0; y < SCREEN_H; y += GRID_CELL_SIZE)
		SDL_RenderDrawLine(_renderer, 0, y, SCREEN_W, y);
	for (int x = 0; x < SCREEN_W; x += GRID_CELL_SIZE)
		SDL_RenderDrawLine(_renderer, x, 0, x, SCREEN_H);
}

void GFX::renderFood(Food food) const {
	SDL_Rect rect = food.getPos();
	SDL_Color color = food.getColor();
	SDL_SetRenderDrawColor(_renderer, color.r, color.g, color.b, 255);
	SDL_RenderFillRect(_renderer, &rect);

}

void GFX::renderSnake(Snake snake) const {
	SDL_Rect* head = snake.getHead();
	SDL_Color color = snake.getColor();
	SDL_SetRenderDrawColor(_renderer, color.r, color.g, color.b, 255);
	SDL_RenderFillRect(_renderer, head);
	for (int i = 0; i < snake.length()-1; i++){
		SDL_Rect rect = snake.getBody().at(i);
		SDL_RenderFillRect(_renderer, &rect);
	}
}

// Renders a red square where the collision occurred
void GFX::renderGameover(SDL_Rect pos) const {
	SDL_Color color = hexToColor(RED);
	SDL_SetRenderDrawColor(_renderer, color.r, color.g, color.b, 255);
	SDL_RenderFillRect(_renderer, &pos);
	SDL_RenderPresent(_renderer);
}

// Also limits FPS
void GFX::renderPresent() const { 
	static double clock = 0;
    double new_clock = SDL_GetTicks();
    double delta = (1000.0/FPS)-(new_clock-clock);

    if (floor(delta) > 0)
        SDL_Delay(delta);

    clock = (delta < -30) ?  new_clock-30 : new_clock+delta;
	SDL_RenderPresent(_renderer);
}
