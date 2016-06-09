#pragma once

#include	<SDL/SDL.h>

#include	"define.h"

class Window;

class Widget
{
public:
	explicit Widget(Window & window);
	~Widget();
	virtual bool draw(SDL_Renderer *renderer);
	virtual bool event(SDL_Event &e);
	virtual bool preload();
	virtual bool mouseable();

	void visable(bool value);
	void enable(bool value);
	bool visable();
	bool enable();

	void size(int w, int h);
	void position(int x, int y);

	void id(int id);					// internal call(for Layer)
	int id();

	void padding(int left, int right, int top, int bottom);
	void getPaddingRect(SDL_Rect & rc);

	SDL_Window *toSDL();
	Window *window();

	void blur(int size);


	SDL_Rect &rect();

	bool isMouseOver();


	// internal
	void window(Window *win);

	// internal
	int blurSize();

	SDL_Texture* texture();
	void texture(SDL_Texture *tx);
	void textureNoFree(SDL_Texture *tx); // internal
	//void setStatic(bool value);

	
	void mouseable(bool value);

	

	const int INVALID_WIDGET_ID = -1;
protected:
	SDL_Rect		_rc;
	Padding			_padding;

private:
	int				_id;
	Window			*_window;
	int				_blurSize;
	bool			_visable;
	bool			_enable;
	SDL_Texture		*_tx;
	bool			_mouseable;
};
