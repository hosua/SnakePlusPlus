#include "globals.h"
#include "graphics.h"
#include "snake.h"
#include <time.h>

void handleMenuQuit(GFX* gfx, SDL_Event event);
void handleIngameInputs(GFX* gfx, Snake* snake, SDL_Event event);
void handlePauseInputs(GFX* gfx, SDL_Event event);
	
Menu* main_menu = nullptr;
Menu* pause_menu = nullptr;
Button* quit_btn = nullptr;

int main(int argc, char *argv[]){
	
	srand(time(NULL));
	
	unsigned long long tick = 0;

	if (SDL_Init(SDL_INIT_EVERYTHING)){ 
		fprintf(stderr, "Fatal error: Failed to initialize SDL: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	g_master = std::shared_ptr<GameMaster>(new GameMaster());

	initFonts();

	GFX* gfx = new GFX();

	Snake* snake = new Snake(SCREEN_W/2, SCREEN_H/2, 
		GRID_CELL_SIZE, LIGHT_BLUE
	);
	Food* food = new Food(SCREEN_W/2+GRID_CELL_SIZE, SCREEN_H/2+GRID_CELL_SIZE, 
		GRID_CELL_SIZE, GREEN
	);

	SDL_Rect prev; // Store position of head in the previous iteration
	
	while (g_master->is_running){
		if (g_master->reset){
			snake->reset();
			food->setRandPos();
			g_master->resetGame();
			g_master->reset = false;
		}

		switch(g_master->gstate){
			case GS_MAINMENU:
			{
				// Initialize the main_menu if it is NULL
				if (!main_menu){
					main_menu = initMainMenu();
					quit_btn = initMainMenuQuitBtn();
				}

				gfx->renderClear();
				SDL_Event event;

				// Handle menu input
				while (SDL_PollEvent(&event)){
					handleMenuQuit(gfx, event);
					main_menu->handleEvents(&event);
					quit_btn->handleEvents(&event);
				}

				gfx->renderText("Snake++", 
					((float)SCREEN_W/3) + ((float)GRID_CELL_SIZE*4.5f), GRID_CELL_SIZE, 
					WHITE, F_LARGE
				);
				gfx->renderMenu(main_menu);
				// Render text above menu frame
				gfx->renderText("Select Difficulty", 
					((float)SCREEN_W/3) + ((float)GRID_CELL_SIZE*6.5f), (GRID_CELL_SIZE*8),
					BLACK, F_SMALL
				);
				// Render quit button below menu
				gfx->renderButton(quit_btn);

				gfx->renderText("Made by: Hoswoo",
					(GRID_CELL_SIZE), (SCREEN_H-(GRID_CELL_SIZE*2)),
					WHITE, F_SMALL
				);
				gfx->renderPresent();
			}
			break; // End GS_MAINMENU

			case GS_INGAME:
			{
				// Free up main_menu memory if game is being played
				if (main_menu){
					delete main_menu;
					delete quit_btn;
					main_menu = nullptr;
				}

				gfx->renderClear();
				SDL_Event event;
				SDL_Rect* coll = nullptr;

				if (g_master->is_paused){ // Game is paused, handle the pause menu
					while (SDL_PollEvent(&event)){
						pause_menu->handleEvents(&event);
						handlePauseInputs(gfx, event);
					}

				} else { // Game is unpaused, handle gameplay
					if (pause_menu){ // If pause menu is allocated to memory, free it and set it to NULL
						delete pause_menu;	
						pause_menu = nullptr;
					}

					while (SDL_PollEvent(&event))
						handleIngameInputs(gfx, snake, event);

					// All events in this if statement are considered the "game tick"
					if (tick % g_master->diff == 0){
						// If the snake ate the food
						if (checkCollision(*snake->getHead(), food->getPos())){
							snake->handleEatEvents(food);
							std::cout << "Snake has eaten " << snake->length()-1 << " apples!\n";
						}

						snake->handleMovement();

						// If the snake collided with itself, then it's game over
						if ((coll = snake->checkSnakeCollision())){
							g_master->game_over = true;
							std::cout << "Snake ate itself\n";
						}


						if (g_master->game_over){
							if (snake->length() == 2)
								std::cout << "Game over! Your snake died after eating 1 apple.\n";
							else
								std::cout << "Game over! Your snake died after eating " << snake->length()-1 << " apples.\n";
							
							// If coll wasn't set, then the gameover location must be at the snake's head's previous location
							if (!coll)
								coll = &prev;

							g_master->gstate = GS_MAINMENU;
							g_master->game_over = g_master->is_paused = false;

							gfx->renderFood(*food);
							gfx->renderSnake(*snake);
							gfx->renderGameover(prev);
							gfx->renderPresent();
							std::this_thread::sleep_for(std::chrono::seconds(2)); // Add some delay before game starts up again

							snake->reset();
							food->setRandPos();
							continue;	
						}
						snake->updateDir();
					}

					// gfx->renderGrid(); // Render grey gridlines (might remove from final build)
					

					prev = *snake->getHead();
					tick++;
				} // End else
				  
				gfx->renderFood(*food);
				gfx->renderSnake(*snake);

				if (g_master->is_paused){
					gfx->renderMenu(pause_menu);
					gfx->renderText("Made by: Hoswoo",
						(GRID_CELL_SIZE), (SCREEN_H-(GRID_CELL_SIZE*2)),
						WHITE, F_SMALL
					);
					gfx->renderText("Paused",
						(SCREEN_W/2), (GRID_CELL_SIZE*10),
						WHITE, F_SMALL
					);
				}

				gfx->renderPresent();
				break; 
			} // End GS_INGAME
		} // End switch(g_game_state)
	} // End while(g_is_running)
	  
	gfx->cleanQuit();
}

void handleIngameInputs(GFX* gfx, Snake* snake, SDL_Event event){
	// Open pause menu if player presses ESC or P, or tries to exit manually
	if (event.type == SDL_QUIT || 
			(event.type == SDL_KEYDOWN && (event.key.keysym.sym == SDLK_ESCAPE || event.key.keysym.sym == SDLK_p))){
		pause_menu = initPauseMenu();
		g_master->is_paused = true;
	}

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
			}
		break;
	}
}

void handlePauseInputs(GFX* gfx, SDL_Event event){
	// If user presses ESC or P while paused, resume the game
	if ((event.type == SDL_KEYDOWN && 
			(event.key.keysym.sym == SDLK_ESCAPE || event.key.keysym.sym == SDLK_p)))
		g_master->is_paused = false;
}

// Checks if game should quit in the menu
void handleMenuQuit(GFX* gfx, SDL_Event event){
	// If user presses ESC while in the menu, quit the game
	if (event.type == SDL_QUIT ||
			(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE))
		gfx->cleanQuit();
}
