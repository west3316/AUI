#pragma once

#include "widget.h"
#include "mousecontrol.h"

class Round	: 
	public Widget,
	public MouseControl

{
public:
	Round(Window& window);
	virtual bool draw(SDL_Renderer * renderer);
	void color(const SDL_Color& color);
	void onClick();
	virtual bool event(SDL_Event &e);
	void corner(int value);
private:
	int			_corner;
	SDL_Color	_color;
};