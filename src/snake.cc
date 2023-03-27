#include "snake.h"

void Food::setRandPos(){
	// Generate random coordinates
	_pos.x = rand() % (SCREEN_W-GRID_CELL_SIZE)+1;
	_pos.y = rand() % (SCREEN_H-GRID_CELL_SIZE)+1;
	// Round the coordinates to the nearest multiple of GRID_CELL_SIZE
	_pos.x = ((_pos.x + GRID_CELL_SIZE/2) / GRID_CELL_SIZE) * GRID_CELL_SIZE;
	_pos.y = ((_pos.y + GRID_CELL_SIZE/2) / GRID_CELL_SIZE) * GRID_CELL_SIZE;
}

void Snake::reset(){
	_head.x = SCREEN_W/2; _head.y = SCREEN_H/2;
	_length = 1;
}

void Snake::handleMovement(){
	SDL_Rect prev = _head;

	switch (_dir){
		case M_LEFT:
			_head.x -= _dim;
			if (_head.x < 0){ 
				g_game_over = true;		
				setBuffDir(M_RIGHT);
			}
			break;
		case M_DOWN:
			_head.y += _dim;
			if (_head.y >= SCREEN_H){ 
				g_game_over = true;		
				setBuffDir(M_UP);
			}
			break;
		case M_RIGHT:
			_head.x += _dim;
			if (_head.x >= SCREEN_W){ 
				g_game_over = true;		
				setBuffDir(M_LEFT);
			}
			break;
		case M_UP:
			_head.y -= _dim;
			if (_head.y < 0){ 
				g_game_over = true;		
				setBuffDir(M_DOWN);
			}
			break;
	}

 	if (_length >= 2){
		// Using vector (less efficient, each rotation is O(N))
		// std::rotate(_body.begin(), _body.begin()+1, _body.end());
		// _body.back() = prev;	
	
		// Using deque (more efficient, each push/pop is O(1))
		_body.pop_back();
		_body.push_front(prev);
	} 

}

void Snake::printInfo(){
	std::cout << "(" << _head.x << "," << _head.y << ")\n";
	std::cout << "length: " << _length << "\n";
}

void Snake::handleEatEvents(Food* food){ // handle events that trigger after eating food 
	SDL_Rect new_seg = {.x=_head.x,.y=_head.y,.w=_dim,.h=_dim};
	_length++;
	_body.push_front(new_seg);
	
	// Move food to new, randomized location
	food->setRandPos();
	// Generate random locations until it doesn't spawn on top of the snake
	while (collidesWithFood(*food)){
		std::cout << "Food spawned on top of snake, moving it to a new position.\n";
		food->setRandPos();
	}
}

void Snake::setBuffDir(MoveDir new_dir){
	switch(new_dir){
		case M_LEFT:
			if (_dir != M_RIGHT)
				_buff_dir = M_LEFT;
			break;
		case M_DOWN:
			if (_dir != M_UP)
				_buff_dir = M_DOWN;
			break;
		case M_RIGHT:
			if (_dir != M_LEFT)
				_buff_dir = M_RIGHT;
			break;
		case M_UP:
			if (_dir != M_DOWN)
				_buff_dir = M_UP;
			break;

	}
}

SDL_Rect* Snake::checkSnakeCollision(){ // Returns NULL if no collision, check's collision of snake head and its body
	for (size_t i = 0; i < _body.size(); i++)
		if (checkCollision(_head, _body.at(i)))
			return &_body.at(i);
	return nullptr;
}

bool Snake::collidesWithFood(Food food){
	// Return true if food collides with snake's head
	if (checkCollision(_head, food.getPos()))
		return true;
	// Return true if food collides with any part of the body
	for (SDL_Rect rect : _body)
		if (checkCollision(rect, food.getPos()))
			return true;
	// Otherwise, no collision occurred
	return false;
}
