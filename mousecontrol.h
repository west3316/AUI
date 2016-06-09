#pragma once

#include "widget.h"

class MouseControl
{
public:
	enum WidgetState
	{
		WS_DISABLE	= 0,
		WS_NORMAL	= 1,
		WS_OVER		= 2,
		WS_PRESS	= 4,


		WS_NUM
	};

	MouseControl(Widget *wdg);

	virtual bool mouseEvent(SDL_Event &e);
	virtual	void onButtonStateChange(WidgetState before, WidgetState after);
	virtual	void onClick();
	WidgetState	state();
protected:
	Widget			*_owner;
	WidgetState		_widgetState;
	void			changeState(WidgetState state);

};
