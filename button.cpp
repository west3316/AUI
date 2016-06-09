#include "define.h"
#include "button.h"

Button::Button(Window & window, const std::string &txt) :
Label(window, txt),
MouseControl(this)
{
	_bgColor = DEFAULT_BUTTON_BG_COLOR;
	_rc.w = DEFAULT_BUTTON_WIDTH;
	_rc.h = DEFAULT_BUTTON_HEIGHT;
	showBg(true);
}

bool Button::draw(SDL_Renderer *renderer)
{
	Label::draw(renderer);
	return true;
}

bool Button::event(SDL_Event &e)
{
	mouseEvent(e);

	return true;
}

void Button::onClick()
{
	printf("Button click!\n");
}

void Button::onDoubleClick()
{
	//printf("Button double click!\n");
}