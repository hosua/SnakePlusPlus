#ifndef GRAPHICS_H
#define GRAPHICS_H
#include "snake.h"
#include "globals.h"

class GFX;
class Button;
class Menu;

class GFX {
public:
	GFX(): _window(nullptr), _surface(nullptr), _renderer(nullptr)
	{ init(); }

	void init();
	void cleanQuit(bool flag=true) const;
	void renderClear() const;
	void renderGrid() const;
	void renderFood(Food food) const;
	void renderSnake(Snake snake) const;
	void renderPresent() const; // Always call at the end of a frame
	void renderGameover(SDL_Rect pos) const; // Render red square where snake died
	
	void renderText(std::string text, int x, int y,
			unsigned long hex_font_color, FontType font_type) const;
	
	void renderMenu(Menu* menu) const; // Render menu consisting of multiple buttons
	void renderButton(Button* button) const; // Render a single button
private:
	mutable SDL_Window* _window;
	mutable SDL_Surface* _surface;
	mutable SDL_Renderer* _renderer;
};

// Menu-related code
class Button {
public:
	Button(int x, int y, int w, int h,
			unsigned long hex_bgcolor,
			std::string text, FontType font, unsigned long hex_fontcolor,
			float x_offset=0.1f, float y_offset=0.1f, // Offset to control position where text renders in button
			int level=0) 
			: _rect({.x=x, .y=y, .w=w, .h=h}),
			  _text(text.c_str()), 
			  _font_type(font), 
			  _hex_bgcolor(hex_bgcolor), _hex_fontcolor(hex_fontcolor), _hex_currcolor(hex_bgcolor), 
			  _x_offset(x_offset), _y_offset(y_offset),
			  _level(level){}
	
	~Button();

	void handleEvents(SDL_Event* e); // Handle button events
	
	/* Getters */
	SDL_Rect getRect(){ return _rect; } // Button's background rect
	unsigned long getBgColor(){ return _hex_bgcolor; }
	unsigned long getFontColor(){ return _hex_fontcolor; }
	unsigned long getCurrColor(){ return _hex_currcolor; }
	int getDiff(){ return _level; }

	std::pair<float, float> getOffset(){ return std::make_pair(_x_offset, _y_offset); }
	FontType getFontType(){ return _font_type; }
	std::string getText(){ return _text; }
	
private:
	SDL_Rect _rect;
	std::string _text;
	FontType _font_type;
	unsigned long _hex_bgcolor, _hex_fontcolor, _hex_currcolor;
	float _x_offset, _y_offset;
	int _level;
};

class Menu {
public:
	Menu(std::vector<std::string> text, std::vector<int> levels,
	 	 int x, int y, int w, int h,
		 int ncols,
		 int rgap=10, int cgap=10,
		 float x_offset=0.1f, float y_offset=0.1f,
		 FontType font_type=F_SMALL,
		 unsigned long hex_btncolor=WHITE, unsigned long hex_fontcolor=BLACK);
	
	~Menu();

	void handleEvents(SDL_Event* e);
	
	unsigned long getBgColor(){ return _hex_bgcolor; }
	std::vector<Button*> getButtons(){ return _buttons; }
	SDL_Rect* getBgRect(){ return _bgrect; }
		
	void setBackground(unsigned long hex_bgcolor, int border_sz=15);
	
private:
	SDL_Rect _rect;
	unsigned long _hex_bgcolor;
	int _nbtns;
	int _ncols, _rgap, _cgap;
	SDL_Rect* _bgrect;
	std::vector<Button*> _buttons;
};


/* Main menu stuff */
Menu* initMainMenu();
Button* initMainMenuQuitBtn();

/* Pause button stuff */
Menu* initPauseMenu();

#endif // GRAPHICS_H
