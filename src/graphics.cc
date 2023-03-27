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
	_surface = SDL_GetWindowSurface(_window);

	if (!_surface){
		fprintf(stderr, "Failed to get _surface from _window.\n");
		fprintf(stderr, "SDL2 Error: %s\n", SDL_GetError());
		cleanQuit(false);
	}

	// Allow window to be resizable
	SDL_SetWindowResizable(_window, SDL_TRUE);
	// Allow resolution independence (Disabled because buttons don't work properly when resolution is scaled)
	// SDL_RenderSetLogicalSize(_renderer, SCREEN_W, SCREEN_H);
	// Allows for transparent rendering
	SDL_SetRenderDrawBlendMode(_renderer, SDL_BLENDMODE_BLEND);
}


void GFX::cleanQuit(bool success) const {
	printf("Quitting, goodbye!\n");
	
	// Clean up fonts
	for (TTF_Font* font : g_master->fonts)
		TTF_CloseFont(font);
	TTF_Quit();
	
	// Clean up SDL
	SDL_DestroyRenderer(_renderer);
	SDL_DestroyWindow(_window);
	SDL_Quit();
	return (success) ? exit(EXIT_SUCCESS) : exit(EXIT_FAILURE);
}

void GFX::renderClear() const {
	SDL_Color color = hexToColor(BLACK);
	SDL_SetRenderDrawColor(_renderer, color.r, color.g, color.b, 255); // draw black screen
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
	for (size_t i = 0; i < snake.length()-1; i++){
		SDL_Rect rect = snake.getBody().at(i);
		SDL_RenderFillRect(_renderer, &rect);
	}
}

// Renders a red square where the collision occurred and a game over message
void GFX::renderGameover(SDL_Rect pos) const {
	SDL_Color color = hexToColor(RED);
	SDL_SetRenderDrawColor(_renderer, color.r, color.g, color.b, 255);
	SDL_RenderFillRect(_renderer, &pos);

	renderText("Game over!",
		(SCREEN_W/3) + (GRID_CELL_SIZE*5), (GRID_CELL_SIZE*10),
		WHITE, F_MED
	);
	// SDL_RenderPresent(_renderer);
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

// Renders the text in a line (Does not handle wrapping)
void GFX::renderText(std::string text, int x, int y,
		unsigned long hex_font_color, FontType font_type) const {

	SDL_Color color = hexToColor(hex_font_color);
	SDL_SetRenderDrawColor(_renderer, color.r, color.g, color.b, 255);
	SDL_Surface* surface = TTF_RenderText_Solid(g_master->fonts[font_type], text.c_str(), color);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(_renderer, surface);

	SDL_Rect rect = {.x=x, .y=y, .w=surface->w, .h=surface->h};
	SDL_RenderCopy(_renderer, texture, NULL, &rect);

	SDL_FreeSurface(surface);
	SDL_DestroyTexture(texture);
}

void GFX::renderMenu(Menu* menu) const {
	std::vector<Button*> btns = menu->getButtons();
	SDL_Rect* bg_rect = menu->getBgRect();
	if (bg_rect){
		SDL_Color bgc = hexToColor(menu->getBgColor());	
		SDL_SetRenderDrawColor(_renderer, bgc.r, bgc.g, bgc.b, 255);
		SDL_RenderFillRect(_renderer, bg_rect);
	}

	for (Button* btn : btns)
		renderButton(btn);
}

void GFX::renderButton(Button* btn) const {
	SDL_Rect rect = btn->getRect();
	std::string text = btn->getText();
	FontType font_type = btn->getFontType();
	SDL_Color btnc = hexToColor(btn->getCurrColor());
	// Render button background
	SDL_SetRenderDrawColor(_renderer, btnc.r, btnc.g, btnc.b, 255);
	SDL_RenderFillRect(_renderer, &rect);
	
	// Be sure to compile with C++17 extensions via -Wc++17-extensions
	// If C++17 is unavailable, then just change this to std::pair<float,flaot> 
	// and unpack x_offset and y_offset manually
	auto [x_off, y_off] = btn->getOffset(); 

	// Render button text
	renderText(text, rect.x+(rect.w*x_off), rect.y+(rect.h*y_off), btn->getFontColor(), font_type);
}

void Button::handleEvents(SDL_Event* e){
	SDL_Rect rect = {
		.x = _rect.x, .y = _rect.y,
		.w = _rect.w, .h = _rect.h,
	};

	SDL_bool inside = SDL_FALSE;
	if (e->type == SDL_MOUSEMOTION || e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEBUTTONUP){
		int mx, my;
		SDL_GetMouseState(&mx, &my);
		// printf("mouse: (%i,%i)\n", mx, my);

		if (mx < rect.x) // mouse left of button
			inside = SDL_FALSE;
		else if (mx > rect.x + rect.w) // mouse right of button
			inside = SDL_FALSE;
		else if (my < rect.y) // mouse above button
			inside = SDL_FALSE;
		else if (my > rect.y + rect.h) // mouse below button
			inside = SDL_FALSE;
		else 
			inside = SDL_TRUE;
	}

	if (inside){
		_hex_currcolor = GREEN;
		if (e->type == SDL_MOUSEBUTTONDOWN){
			int level = getDiff();
			// negative numbers are reserved for pause menu actions
			if (level < 0){	
				// Cast int to PauseAction enum to make compiler happy
				PauseAction pause_action = (PauseAction) level;
				switch(pause_action){
					case PA_RESUME:
						g_master->is_paused = false;
						break;
					case PA_MAINMENU:
						g_master->reset = true;
						g_master->gstate = GS_MAINMENU;
						break;
					case PA_QUIT:
						g_master->is_running = false;
						break;
				}
			// level 0 is reserved for quit event,
			} else if (level == 0){
				// If level is 0, then handle quit event
				std::cout << "Pressed quit button\n";
				g_master->is_running = false;
			} else {
				// Otherwise, set game level to button that was pressed and start the game
				std::cout << "Started game on difficulty: " << getText() << "\n";
				g_master->diff = getDiff();
				g_master->gstate = GS_INGAME;
			}
		}
	} else {
		_hex_currcolor = _hex_bgcolor;
	}
}

void Menu::handleEvents(SDL_Event* e){
	for (Button* btn : _buttons)
		btn->handleEvents(e);
}

Menu::Menu(std::vector<std::string> text, std::vector<int> levels,
		 int x, int y, int w, int h,
		 int ncols,
		 int rgap, int cgap,
		 float x_offset, float y_offset,
		 FontType font_type,
		 unsigned long hex_btncolor, unsigned long hex_fontcolor)
	  : _rect({.x=x,.y=y,.w=w,.h=h}), 
		_hex_bgcolor(0x0),
		_nbtns(text.size()), 
		_ncols(ncols), _rgap(rgap), _cgap(cgap),
		_bgrect(nullptr) {

	assert(text.size() == levels.size() && "Error: text vector size does not match levels vector size.");	
	int c = 0;
	int x_start = x;
	for (int i = 0; i < _nbtns; i++){
		Button* button = new Button(x, y, w, h, 
			hex_btncolor,
			text[i], font_type, BLACK,
			x_offset, y_offset,
			levels[i]);

		_buttons.push_back(button);
		if (++c == ncols){
			c = 0;
			y += h + rgap;
			x = x_start;
		} else {
			x += w + cgap;
		}
	}
}

Button::~Button(){ /* Nothing here needs to explicitly be deleted */ }

Menu::~Menu(){
	for (Button* btn : _buttons)
		delete(btn);
}

void Menu::setBackground(unsigned long hex_bgcolor, int border_sz){
	_hex_bgcolor = hex_bgcolor;
	_bgrect = new SDL_Rect();
	*_bgrect = {.x=_rect.x-border_sz, 
				.y=_rect.y-border_sz,
				.w=(border_sz*2)+((_rect.w+_cgap)*_ncols)-_cgap,
				.h=(border_sz*2)+((_rect.h+_rgap)*(_nbtns/_ncols))-_rgap};
	// printf("x=%i\ty=%i\tw=%i\th=%i\n", _bgrect->x, _bgrect->y, _bgrect->w, _bgrect->h);
}

/* Main Menu stuff */
// Text displayed on main menu buttons
std::vector<std::string> btn_txt = { "1", "2", "3", "4", "5", "6", "7", "8", "9", "10" };
// Lower level value indicates a harder difficulty
std::vector<int> levels = { 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 };
Menu* initMainMenu(){
	Menu* main_menu = new Menu(btn_txt, levels,
		((float)SCREEN_W/3) + ((float)GRID_CELL_SIZE*4.5f), (GRID_CELL_SIZE*10), // x, y
		(GRID_CELL_SIZE*2), (GRID_CELL_SIZE*2), // w, h (of buttons)
		5, // ncols
		10, 10, // rgap, cgap (gap between buttons)
		0.1f, 0.1f, // x_offset, y_offset (text offset within button)
		F_SMALL, // Font size
		WHITE, BLACK // btn_color, font_color
	);

	// Add colored frame to menu bg
	main_menu->setBackground(SKY_BLUE, GRID_CELL_SIZE*2.5f);
	return main_menu;
}

Button* initMainMenuQuitBtn(){
	return new Button(((float)SCREEN_W/2) - ((float)GRID_CELL_SIZE*2.75f),  SCREEN_H/2 - (GRID_CELL_SIZE*3),  // x, y
		(GRID_CELL_SIZE*5), (GRID_CELL_SIZE*1.5f), // w, h
		WHITE, // bg_color
		"Quit", F_SMALL, BLACK, // text, font_size, font_color
		0.1f, 0.1f // x_offset, y_offset (text within button)
	); 
}

/* Pause Menu stuff */
// Text displayed on pause menu buttons
std::vector<std::string> pause_menu_txt = { "Resume", "Main Menu", "Quit" };
// Handle pause actions by their PA ID's
std::vector<int> pause_actions = { PA_RESUME, PA_MAINMENU, PA_QUIT };

Menu* initPauseMenu(){
	Menu* pause_menu = new Menu(pause_menu_txt, pause_actions,
		((float)SCREEN_W/3) + ((float)GRID_CELL_SIZE*4.5f), (GRID_CELL_SIZE*12), // x, y
		(GRID_CELL_SIZE*8), (GRID_CELL_SIZE*2), // w, h (of buttons)
		2, // ncols	
		10, 10, // rgap, cgap (gap between buttons)
		0.1f, 0.1f, // x_offset, y_offset (text offset within button)
		F_SMALL, // Font size
		WHITE, BLACK // btn_color, font_color
	);
	// pause_menu->setBackground(SKY_BLUE, GRID_CELL_SIZE*4);
	return pause_menu;
}
