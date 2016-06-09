#pragma once

#include "widget.h"
#include "mousecontrol.h"

#include <vector>

extern "C" {
	#include "SDL_prims.h"
}


class Poly	: 
	public Widget,
	public MouseControl

{
public:
	Poly(Window& window);
	virtual bool draw(SDL_Renderer * renderer);
	void color(const SDL_Color& color);
	void onClick();
	virtual bool event(SDL_Event &e);

	void points(const SDL_Point *pt, int count);
	typedef std::vector<SDL_Point2> Points;
private:

	SDL_Color	_color;
	Points		_lsPoint;
};