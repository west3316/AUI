#include "image.h"
#include "window.h"

#include <SDL/SDL_image.h>
#include <iostream>

Image::Image(Window & window, const std::string &file):
	Widget(window)
{
	visable(false); 
	load(file);
}

void Image::release()
{
	if (!texture())
		return;

	SDL_DestroyTexture(texture());
	texture(NULL);
}

bool Image::load(const std::string &file)
{
	release();

	SDL_Surface *sf = IMG_Load(file.c_str());
	if (!sf)
		return false;

	_rc.h = sf->h;
	_rc.w = sf->w;
	
	texture(SDL_CreateTextureFromSurface(window()->renderer(), sf));
	SDL_FreeSurface(sf);

	visable(true);
	return true;
}

bool Image::draw(SDL_Renderer *renderer)
{
	//std::cout << texture() << std::endl;
	if (texture())
		SDL_RenderCopy(renderer, texture(), NULL, &_rc);

	return true;
}
