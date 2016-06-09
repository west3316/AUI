#pragma once

#include "widget.h"
#include <string>



class Image:
	public Widget
{
public:
	Image(Window & window, const std::string &file);
	bool load(const std::string &file);
	virtual bool draw(SDL_Renderer *renderer);
	void release();
};