#include <string>


#include "round.h"
#include "utils.h"
#include "window.h"


#define DEFAULT_CORNER_VALUE	10

Round::Round(Window& window) :
Widget(window),
_color({ 0xff, 0xff, 0xff, 0xff }),
MouseControl(this),
_corner(DEFAULT_CORNER_VALUE)
{

}

bool Round::event(SDL_Event &e)
{
	mouseEvent(e);
	/*
	switch (e.type)
	{
	case APP_TIME:
	{
		if (_rc.h <= 0)
			break;

	//	_rc.h = std::max(0, int(_rc.h - _rc.h * 0.1));
	//	layout(window()->rect(), _rc, LayoutLeft, LayoutBottom);
		break;
	}
		
	}
	*/
	return true;
}

 
bool Round::draw(SDL_Renderer * renderer)
{
	if (_color.a < 0xff)
	{
		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	}

	SDL_SetRenderDrawColor(renderer, _color.r, _color.g, _color.b, _color.a);
	//SDL_RenderFillRect(renderer, &_rc);


	SDL_Surface *sf = SDL_GetWindowSurface(window()->toSDL());
	round(sf, _rc, SDL_MapRGBA(sf->format, _color.r, _color.g, _color.b, _color.a), _corner);

	return true;
}

void Round::color(const SDL_Color& color)
{
	memcpy(&_color, &color, sizeof(SDL_Color));
}


void Round::corner(int value)
{
	_corner = value;
}

void Round::onClick()
{
	printf("click Rectange\n");
}