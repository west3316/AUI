#include <SDL/SDL.h>

#include "window.h"
#include "define.h"
#include "utils.h"

Window::Window(const std::string &title, int w, int h, uint32_t flag)
{
	_lock = false;
	_rc = {0, 0, w, h};
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
		return;

	_window = SDL_CreateWindow(title.c_str(),
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		w, h, SDL_WINDOW_OPENGL | flag);

	if (!_window)
		return;

	layer.owner(SDL_GetWindowID(_window));

	_renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_SOFTWARE);
	if (!_renderer)
		return;

	_timerId = SDL_AddTimer(500, Window::clock, this);
}

Uint32 Window::clock(Uint32 interval, void *param)
{
	Window &w = *((Window*)param);
	SDL_Event event;
	SDL_memset(&event, 0, sizeof(event));
	event.type = APP_TIME;
	event.user.data1 = param;
	event.user.data2 = (void*)interval;

	SDL_PushEvent(&event);

	return interval;
}

bool Window::run()
{
	if (!layer.preload())
		return false;

	SDL_Texture *tx = NULL;
	SDL_StartTextInput();
	bool bQuit = false;
	SDL_Event e;
	while (!bQuit)
	{
		SDL_WaitEvent(&e);

		switch (e.type)
		{
			case SDL_QUIT:
			{
				bQuit = true;
				break;
			}

			default:
			{
				if (!_lock)
					layer.event(e);
			}
		}

		SDL_RenderClear(_renderer);
		SDL_SetRenderDrawColor(_renderer, 0xcc, 0xcc, 0xcc, 0xff);
		SDL_RenderClear(_renderer);
	
		layer.draw(_renderer);

		SDL_RenderPresent(_renderer);
	}
	SDL_StopTextInput();
	return true;
}

SDL_Renderer *Window::renderer()
{
	return _renderer;
}

SDL_Rect &Window::rect()
{
	SDL_GetWindowPosition(_window, &_rc.x, &_rc.y);
	SDL_GetWindowSize(_window, &_rc.w, &_rc.h);
	return _rc;
}

Window::~Window()
{
	SDL_DestroyRenderer(_renderer);
	SDL_DestroyWindow(_window);
}

SDL_Window *Window::toSDL()
{
	return _window;
}

void Window::lock()
{
	_lock = true;
}

void Window::unlock()
{
	_lock = false;
}

int Window::mouseOverWidget()
{
	return layer.mouseOverWidget();
}

size_t Window::add(Widget *wdg)
{
	return layer.add(wdg, this);
}