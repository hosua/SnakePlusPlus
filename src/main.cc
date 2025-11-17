#include "globals.h"
#include "graphics.h"
#include "snake.h"
#include <thread>
#include <time.h>

void handleMainMenuInputs(GFX* gfx, SDL_Event event);
void handleIngameInputs(GFX* gfx, Snake* snake, SDL_Event event);
void handlePauseInputs(GFX* gfx, SDL_Event event);
	
Menu* main_menu = nullptr;
Menu* pause_menu = nullptr;
Button* quit_btn = nullptr;

int main(int argc, char *argv[]){
	
	srand(time(NULL));
	
	unsigned long long tick = 0;

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)){ 
		fprintf(stderr, "Fatal error: Failed to initialize SDL: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	g_gamemaster = std::unique_ptr<GameMaster>(new GameMaster());
	if (!initIMG())
		exit(EXIT_FAILURE);
	if (!initFonts())
		exit(EXIT_FAILURE);
	#ifndef EMSCRIPTEN
	initSounds(); // If sounds fail to load, game should still be playable so no need to exit here.
	#endif
	
	// Save data
	if (!(g_savedata = saveLoad()))
		saveInit();
	// Sounds
	#ifndef EMSCRIPTEN
	g_soundmaster = std::unique_ptr<SoundMaster>(new SoundMaster());
	#else
	g_soundmaster = nullptr;
	#endif
	
	// Graphics
	std::unique_ptr<GFX> gfx = std::unique_ptr<GFX>(new GFX());
	
	// Game elements
	std::unique_ptr<Snake> snake = std::unique_ptr<Snake>(
			new Snake(
				SCREEN_W/2, SCREEN_H/2, 
				GRID_CELL_SIZE, LIGHT_BLUE
			)
	);
	std::unique_ptr<Food> food = std::unique_ptr<Food>(
			new Food(
				SCREEN_W/2+GRID_CELL_SIZE, SCREEN_H/2+GRID_CELL_SIZE, 
				GRID_CELL_SIZE, GREEN
			)
	);

	SDL_Rect prev; // Store position of head in the previous iteration
	
	while (g_gamemaster->is_running){
		if (g_gamemaster->reset){
			snake->reset();
			food->setRandPos();
			g_gamemaster->resetGame();
			g_gamemaster->reset = false;
		}

		switch(g_gamemaster->gstate){
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
					handleMainMenuInputs(gfx.get(), event);
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
				// Render quit button below main_menu
				gfx->renderButton(quit_btn);

				gfx->renderText("Made by: Hoswoo",
					(GRID_CELL_SIZE), (SCREEN_H-(GRID_CELL_SIZE*2)),
					WHITE, F_SMALL
				);
				// Display game version
				gfx->renderText(GAME_VERSION,
					(SCREEN_W - (GRID_CELL_SIZE*4)), (SCREEN_H-(GRID_CELL_SIZE*2)),
					WHITE, F_SMALL
				);
				
				// Render icon based on if sound is muted or unmuted
				#ifndef EMSCRIPTEN
				if (g_soundmaster && g_soundmaster->isMuted())
					gfx->blitImage(IMG_AUDIO_OFF, 
						SCREEN_W - (GRID_CELL_SIZE*2.5f), (GRID_CELL_SIZE/2),
						ICON_SIZE, ICON_SIZE
					);
				else if (g_soundmaster)
					gfx->blitImage(IMG_AUDIO_ON, 
						SCREEN_W - (GRID_CELL_SIZE*2.5f), (GRID_CELL_SIZE/2),
						ICON_SIZE, ICON_SIZE
					);
				#endif

				// Render high score
				if (!g_gamemaster->buff_str.empty())
					gfx->renderText(g_gamemaster->buff_str,
						((float)SCREEN_W/3) + ((float)GRID_CELL_SIZE*4.8f), SCREEN_H/2,
						WHITE, F_SMALL
					);

				gfx->renderPresent();
			}
			break; // End GS_MAINMENU

			case GS_INGAME:
			{
				gfx->renderClear();

				// Free up main_menu memory if game is being played
				if (main_menu){
					delete main_menu;
					delete quit_btn;
					main_menu = nullptr;
					quit_btn = nullptr;
				}

				SDL_Event event;
				SDL_Rect* coll = nullptr;

				if (g_gamemaster->is_paused){ // Game is paused, handle the pause menu
					while (SDL_PollEvent(&event)){
						pause_menu->handleEvents(&event);
						handlePauseInputs(gfx.get(), event);
					}

				} else { // Game is unpaused, handle gameplay
					if (pause_menu){ // If pause menu is allocated to memory, free it and set it to NULL
						delete pause_menu;	
						pause_menu = nullptr;
					}
					
					while (SDL_PollEvent(&event))
						handleIngameInputs(gfx.get(), snake.get(), event);

					// All events in this if statement are considered the "game tick"
					if (g_gamemaster->cd_counter < 0 && (tick % g_gamemaster->option == 0)){
						// If the snake ate the food
						if (checkCollision(*snake->getHead(), food->getPos())){
							snake->handleEatEvents(food.get());
							if (snake->length()-1 == 1) // English majors be like
								std::cout << "Snake has eaten 1 apple!\n";
							else
								std::cout << "Snake has eaten " << snake->length()-1 << " apples!\n";
							#ifndef EMSCRIPTEN
							if (g_soundmaster && !g_soundmaster->isMuted() && g_soundmaster->getSound(S_EAT))
								Mix_PlayChannel(-1, g_soundmaster->getSound(S_EAT), 0);
							#endif
						}

						snake->handleMovement();

						// If the snake collided with itself, then it's game over
						if ((coll = snake->checkSnakeCollision())){
							g_gamemaster->game_over = true;
							std::cout << "Snake committed sudoku\n";
						}

						if (g_gamemaster->game_over){
							#ifndef EMSCRIPTEN
							if (g_soundmaster && !g_soundmaster->isMuted() && g_soundmaster->getSound(S_EXPLOSION))
								Mix_PlayChannel(-1, g_soundmaster->getSound(S_EXPLOSION), 0);
							#endif
							
							if (snake->length() == 2) // English majors be like
								std::cout << "Game over! Your snake died after eating 1 apple.\n";
							else
								std::cout << "Game over! Your snake died after eating " << snake->length()-1 << " apples.\n";
							
							// If coll wasn't set, then the gameover location must be at the snake's head's previous location
							if (!coll)
								coll = &prev;

							g_gamemaster->gstate = GS_MAINMENU;
							g_gamemaster->game_over = g_gamemaster->is_paused = false;

							gfx->renderFood(*food);
							gfx->renderSnake(*snake);
							gfx->renderGameover(prev);

							// Render score
							gfx->renderText(std::string("SCORE: " + std::to_string(snake->length()-1)),
									(GRID_CELL_SIZE/2), (GRID_CELL_SIZE/2),
									WHITE, F_SMALL
							);

							gfx->renderPresent();
							// Update save file score
							saveUpdate(g_gamemaster->level, snake->length()-1);
							std::this_thread::sleep_for(std::chrono::seconds(2)); // Add some delay before game starts up again

							snake->reset();
							food->setRandPos();
							continue;	
						}
					}
					
					// Update direction only if we're in a game tick
					if (g_gamemaster->cd_counter < 0 && (tick % g_gamemaster->option == 0)){
						snake->updateDir();
					}

					// gfx->renderGrid(); // Render grey gridlines (might remove from final build)

					prev = *snake->getHead();
					tick++;
				} // End else
				  
				gfx->renderFood(*food);
				gfx->renderSnake(*snake);

				// Render score
				gfx->renderText(std::string("SCORE: " + std::to_string(snake->length()-1)),
						(GRID_CELL_SIZE/2), (GRID_CELL_SIZE/2),
						WHITE, F_SMALL
				);
				
				// These ifs are down here because we want them rendered on top of all the other game elements	
				if (!g_gamemaster->cd_started && g_gamemaster->cd_counter > -1){
					gfx->renderText(std::to_string(g_gamemaster->cd_counter).c_str(),
						SCREEN_W/2, SCREEN_H/2,
						WHITE, F_LARGE
					);
					std::this_thread::sleep_for(std::chrono::seconds(1));
					g_gamemaster->cd_counter--;
				}
				
				// This is for allowing the game to render the first frame before the cooldown starts
				if (g_gamemaster->cd_started){
					g_gamemaster->cd_started = false;
					gfx->renderText("Get ready...",
						(SCREEN_W/2)-(GRID_CELL_SIZE*6), ((SCREEN_H/2)-(GRID_CELL_SIZE*2)),
						WHITE, F_LARGE
					);
				}

				if (g_gamemaster->is_paused){
					gfx->renderMenu(pause_menu);
					gfx->renderText("Made by: Hoswoo",
						(GRID_CELL_SIZE), (SCREEN_H-(GRID_CELL_SIZE*2)),
						WHITE, F_SMALL
					);
					gfx->renderText("Paused",
						(SCREEN_W/2), (GRID_CELL_SIZE*10),
						WHITE, F_SMALL
					);
					// Display game version
					gfx->renderText(GAME_VERSION,
						(SCREEN_W - (GRID_CELL_SIZE*4)), (SCREEN_H-(GRID_CELL_SIZE*2)),
						WHITE, F_SMALL
					);

					// Render icon based on if sound is muted or unmuted
					#ifndef EMSCRIPTEN
					if (g_soundmaster && g_soundmaster->isMuted())
						gfx->blitImage(IMG_AUDIO_OFF, 
							SCREEN_W - (GRID_CELL_SIZE*2.5f), (GRID_CELL_SIZE/2),
							ICON_SIZE, ICON_SIZE
						);
					else if (g_soundmaster)
						gfx->blitImage(IMG_AUDIO_ON, 
							SCREEN_W - (GRID_CELL_SIZE*2.5f), (GRID_CELL_SIZE/2),
							ICON_SIZE, ICON_SIZE
						);
					#endif
				} // End if(g_gamemaster->is_paused())

				gfx->renderPresent();
				break; 
			} // End GS_INGAME
		} // End switch(g_game_state)
	} // End while(g_is_running)
	
	gfx->cleanQuit();
}

void handleIngameInputs(GFX* gfx, Snake* snake, SDL_Event event){
	if (g_gamemaster->cd_counter > 0) // Don't handle any input events if the cooldown is still running
		return;
	// Open pause menu if player presses ESC or P, or tries to exit manually
	if (event.type == SDL_QUIT || 
			(event.type == SDL_KEYDOWN && (event.key.keysym.sym == SDLK_ESCAPE || event.key.keysym.sym == SDLK_p))){
		pause_menu = initPauseMenu();
		g_gamemaster->is_paused = true;
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
				#ifndef EMSCRIPTEN
				case SDLK_m: // Mute/unmute sound
					if (g_soundmaster){
						g_soundmaster->toggleMuted();
						(g_soundmaster->isMuted()) ? std::cout << "Sound muted\n" : std::cout << "Sound unmuted\n";
					}
					break;
				#endif
			}
		break;
	}
}

void handlePauseInputs(GFX* gfx, SDL_Event event){
	// If user presses ESC or P while paused, resume the game
	if ((event.type == SDL_KEYDOWN && 
			(event.key.keysym.sym == SDLK_ESCAPE || event.key.keysym.sym == SDLK_p))){
		g_gamemaster->startCD();
	}
	switch(event.type){
		case SDL_KEYDOWN:
			#ifndef EMSCRIPTEN
			switch(event.key.keysym.sym){
				case SDLK_m: // Mute/unmute sound
					if (g_soundmaster){
						g_soundmaster->toggleMuted();
						(g_soundmaster->isMuted()) ? std::cout << "Sound muted\n" : std::cout << "Sound unmuted\n";
					}
					break;
			}
			#endif
		break;
	}
}

// Handles main menu keyboard input
void handleMainMenuInputs(GFX* gfx, SDL_Event event){
	// If user presses ESC while in the menu, quit the game
	// Disable ESC quit for Emscripten/web builds
	#ifndef EMSCRIPTEN
	if (event.type == SDL_QUIT ||
			(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE))
		gfx->cleanQuit();
	#else
	// In web builds, only handle window close, not ESC key
	if (event.type == SDL_QUIT)
		gfx->cleanQuit();
	#endif

	switch(event.type){ // Allow player to mute/unmute in main menu
		case SDL_KEYDOWN:
			#ifndef EMSCRIPTEN
			switch(event.key.keysym.sym){
				case SDLK_m: // Mute/unmute sound
					if (g_soundmaster){
						g_soundmaster->toggleMuted();
						(g_soundmaster->isMuted()) ? std::cout << "Sound muted\n" : std::cout << "Sound unmuted\n";
					}
					break;
			}
			#endif
		break;
	}
}
