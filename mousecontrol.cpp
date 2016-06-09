#include "mousecontrol.h"
#include "widget.h"
#include "window.h"

#include <iostream>



bool MouseControl::mouseEvent(SDL_Event &e)
{
	if (!_owner->isMouseOver())
	{
		if (_owner->window()->mouseOverWidget() == 5)
			std::cout << "11111" << std::endl;
		changeState(WS_NORMAL);
		return true;
	}

	std::cout << _owner->id() << ", " << _widgetState << std::endl;
	
	if (_widgetState == WS_DISABLE)
		return false;

	switch (e.type)
	{
	case SDL_MOUSEBUTTONDOWN:
		if (e.button.button == SDL_BUTTON_LEFT)
			changeState(WS_PRESS);
		break;
	case SDL_MOUSEBUTTONUP:
	{
		std::cout << "SDL_MOUSEBUTTONUP: "<< _widgetState <<", rl: "<<(int)e.button.button<< std::endl;
		if (e.button.button == SDL_BUTTON_LEFT)
		{
			std::cout << "WS_PRESS: " << std::endl;
			changeState(WS_OVER);

			//if (e.button.clicks == 1)
			onClick();
		}
	}
		break;
	default:
		changeState(WS_OVER);
		break;
	}

	return true;
}

void MouseControl::onButtonStateChange(WidgetState before, WidgetState after)
{
}

void MouseControl::onClick()
{
}

MouseControl::WidgetState MouseControl::state()
{
	return _widgetState;
}

 

void MouseControl::changeState(WidgetState state)
{
	if (_widgetState == state)
		return;

	WidgetState old = _widgetState;
	
	_widgetState = state;
	onButtonStateChange(old, state);
}

MouseControl::MouseControl(Widget *wdg):
	_widgetState(WS_NORMAL),
	_owner(wdg)
{
	_owner->mouseable(true);
}
