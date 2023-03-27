#include "globals.h"
#include "graphics.h"
#include "snake.h"
#include <time.h>

void handleInputs(GFX* gfx, Snake* snake, SDL_Event event);

int main(int argc, char *argv[]){
	
	srand(time(NULL));
	
	unsigned long long tick = 0;
	int game_tick_len = 5; // Game tick will only occur when modulo game_tick_len == 0

	if (SDL_Init(SDL_INIT_EVERYTHING)){ 
		fprintf(stderr, "Fatal error: Failed to initialize SDL: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	GFX* gfx = new GFX();
	Snake* snake = new Snake(SCREEN_W/2, SCREEN_H/2, 
			GRID_CELL_SIZE, LIGHT_BLUE);
	Food* food = new Food(SCREEN_W/2+GRID_CELL_SIZE, SCREEN_H/2+GRID_CELL_SIZE, 
			GRID_CELL_SIZE, GREEN);
				   
	while (1){

		SDL_Rect* coll = nullptr;
		SDL_Event event;
		SDL_Rect prev; // Store position of head in the previous iteration

		while (SDL_PollEvent(&event))
			handleInputs(gfx, snake, event);


		// All events in this if statement are considered the "game tick"
		if (tick % game_tick_len == 0){
			// Snake ate food
			if (checkCollision(*snake->getHead(), food->getPos())){
				snake->handleEatEvents(food);
				std::cout << "Snake has eaten " << snake->length()-1 << " apples!\n";
			}

			snake->handleMovement();
			if ((coll = snake->checkSnakeCollision())){
				game_over = true;
				std::cout << "Snake ate itself\n";
			}


			if (game_over){
				std::cout << "Game over! Your snake died after eating " << snake->length()-1 << " apples.\n";
				// If coll wasn't set, then the gameover location must be at the snake's head's previous location
				if (!coll)
					coll = &prev;
				gfx->renderGameover(*coll);
				snake->reset();

				food->setRandPos();
				// Generate new food position until it doesn't collide with the snake
				while (snake->collidesWithFood(*food)){
					std::cout << "Previous food position collided with snake, generating a new position\n";
					food->setRandPos();
				}
				game_over = false;
				gfx->renderPresent();
				std::this_thread::sleep_for(std::chrono::seconds(2));
			}
			snake->updateDir();
		}

		gfx->renderClear();
		gfx->renderGrid();
		gfx->renderFood(*food);
		gfx->renderSnake(*snake);

		gfx->renderPresent();
		prev = *snake->getHead();
		tick++;
	}
}

void handleInputs(GFX* gfx, Snake* snake, SDL_Event event){
	// Quit if ESC
	if (event.type == SDL_QUIT || 
			(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE))
		gfx->cleanQuit();

	switch(event.type){
		case SDL_KEYDOWN:
			switch(event.key.keysym.sym){
				case SDLK_w: case SDLK_UP:
					snake->setBuffDir(M_UP);
					break;
				case SDLK_s: case SDLK_DOWN:
					snake->setBuffDir(M_DOWN);
					break;
				case SDLK_a: case SDLK_LEFT:
					snake->setBuffDir(M_LEFT);
					break;
				case SDLK_d: case SDLK_RIGHT:
					snake->setBuffDir(M_RIGHT);
					break;
				case SDLK_ESCAPE:
					gfx->cleanQuit();
					break;
			}
		break;
	}
}
