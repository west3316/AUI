#pragma once

#include "widget.h"
#include "mousecontrol.h"

namespace aui {

class Rectangle	: 
	public Widget,
	public MouseControl

{
public:
	Rectangle(Window& window);
	virtual bool draw(SDL_Renderer * renderer);
	void color(const SDL_Color& color);
	void onClick();
	virtual bool event(SDL_Event &e);
private:
	SDL_Color _color;
};

}