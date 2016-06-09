#pragma once

#include "mousecontrol.h"
#include "label.h"

class Button :
	public Label,
	public MouseControl
{
public:
	Button(Window & window, const std::string &txt);

	void onClick();
	void onDoubleClick();

	bool draw(SDL_Renderer *renderer);
	bool event(SDL_Event &e);

};
