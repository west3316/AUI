#pragma once

#include <vector>
#include <SDL/SDL.h>

#include "layer.h"


class Widget;

class Window
{
public:
	Window(const std::string &title = "no title", int w = 800, int h = 600, uint32_t flag = 0);
	~Window();

	bool run();

	SDL_Rect &rect();
	SDL_Renderer *renderer();
	SDL_Window *toSDL();

	void lock();
	void unlock();

	size_t add(Widget *wdg);

	int mouseOverWidget();
	Layer			layer;
private:
	static Uint32 clock(Uint32, void*);
private:
	int				_timerId;
	SDL_Rect		_rc;
	SDL_Renderer	*_renderer;
	SDL_Window		*_window;
	bool			_lock;
};