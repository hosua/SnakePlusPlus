#ifndef GRAPHICS_H
#define GRAPHICS_H
#include "snake.h"
#include "globals.h"

class GFX {
public:
	GFX(): _window(nullptr), _surface(nullptr), _renderer(nullptr), 
	_texture(nullptr) { init(); }

	void init();
	void cleanQuit(bool flag=true) const;
	void renderClear() const;
	void renderGrid() const;
	void renderFood(Food food) const;
	void renderSnake(Snake snake) const;
	void renderPresent() const;
	void renderGameover(SDL_Rect pos) const; // Render red square where snake died

private:
	mutable SDL_Window* _window;
	mutable SDL_Surface* _surface;
	mutable SDL_Renderer* _renderer;
	mutable SDL_Texture* _texture;
};

#endif // GRAPHICS_H
