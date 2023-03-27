#ifndef SNAKE_H
#define SNAKE_H

#include "globals.h"

class Food;

class Snake {
public:
	Snake(int x, int y, int dim, unsigned long color): 
		_length(1), _dim(dim), _buff_dir(M_RIGHT), _dir(M_RIGHT), 
		_head({.x=x,.y=y,.w=dim,.h=dim}), _color(hexToColor(color)){}

	void handleMovement();

	// The buffer to store snake direction between game ticks
	void setBuffDir(MoveDir new_dir);
	// Snake's real dir should only update during game tick
	void updateDir(){ _dir = _buff_dir; }
	void reset();
	void handleEatEvents(Food* food); // handle events that trigger after eating food 
	
	SDL_Rect* checkSnakeCollision(); // Returns nullptr if no collision occurred
	void printInfo();

	bool collidesWithFood(Food food);

	std::deque<SDL_Rect> getBody(){ return _body; }
	size_t length(){ return _length; } // Get length of snake
	SDL_Rect* getHead(){ return &_head; } // Get snake's head rect
	SDL_Color getColor(){ return _color; } // Get color of snake
	
private:

	int _length; // length of snake
	int _dim; // dimensions of snake 
	MoveDir _buff_dir; // Buffer direction (To store snake's direction in between frames)
	MoveDir _dir; // Actual direction (The direction that the snake will actually travel too during game tick
	SDL_Rect _head; // position of snake head 
	std::deque<SDL_Rect> _body; // positions of the rest of the snake
	SDL_Color _color;
};


class Food {
public:
	Food(int x, int y, int dim, unsigned long color): 
		_pos({.w=dim,.h=dim}), _color(hexToColor(color)){ setRandPos(); }

	SDL_Color getColor(){ return _color; }
	SDL_Rect getPos(){ return _pos; }
	
	void setRandPos(); // Generates food at random position
	void setPos(int x, int y){ _pos.x=x, _pos.y=y; } // For debugging, should not be used in final game

private:
	SDL_Rect _pos;
	SDL_Color _color;
};

#endif // SNAKE_H
