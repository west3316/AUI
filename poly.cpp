#include <string>


#include "poly.h"

#include "window.h"



Poly::Poly(Window& window) :
	Widget(window),
	_color({ 0xff, 0xff, 0xff, 0xff }),
	MouseControl(this)
{

}

bool Poly::event(SDL_Event &e)
{
	mouseEvent(e);

	return true;
}

void Poly::points(const SDL_Point * pt, int count)
{
	for (int i = 0; i < count; ++i)
	{
		SDL_Point2 p = { (Uint16)pt[i].x, (Uint16)pt[i].y };
		_lsPoint.push_back(p);
	}
}

 
bool Poly::draw(SDL_Renderer * renderer)
{
	if (_lsPoint.empty())
		return true;

	if (_color.a < 0xff)
	{
		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	}

	SDL_SetRenderDrawColor(renderer, _color.r, _color.g, _color.b, _color.a);


	SDL_Surface *sf = SDL_GetWindowSurface(window()->toSDL());

	SDL_FillPolygon(sf, _lsPoint.data(), (int)_lsPoint.size(), SDL_MapRGBA(sf->format, _color.r, _color.g, _color.b, _color.a));

	return true;
}

void Poly::color(const SDL_Color& color)
{
	memcpy(&_color, &color, sizeof(SDL_Color));
}

 

void Poly::onClick()
{
	printf("click Rectange\n");
}