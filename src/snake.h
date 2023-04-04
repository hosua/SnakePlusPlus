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
	// Snake's real direction, should only update during game tick
	void updateDir(){ _dir = _buff_dir; }
	void reset();
	void handleEatEvents(Food* food); // handle events that trigger after eating food 
	
	// Checks if snake's head collided with any parts of its body. 
	// Returns nullptr upon no collision. Otherwise, returns a rect containing the position of the collision
	SDL_Rect* checkSnakeCollision(); 

	void printInfo(); // For debugging purposes

	// Returns true if any part of the snake's head/body collides with food.
	// Used to respawn food again if the food happens to spawn on top of the snake
	bool collidesWithFood(Food food) const; 

	std::deque<SDL_Rect> getBody() const { return _body; }
	size_t length() const { return _length; } // Get length of snake
	size_t size() const { return _length; } // Same as length()
	SDL_Rect* getHead(){ return &_head; } // Get snake's head rect
	SDL_Color getColor() const { return _color; } // Get color of snake
	
private:

	int _length; // length of snake
	int _dim; // dimensions of snake (cell is square, so only one parameter for width/height is needed)
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
