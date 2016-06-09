#include <string>
#include <algorithm>

#include "rectangle.h"
#include "utils.h"
#include "window.h"

namespace aui {

	Rectangle::Rectangle(Window& window) :
		Widget(window),
		_color({ 0xff, 0xff, 0xff, 0xff }),
		MouseControl(this)
	{

	}

	bool Rectangle::event(SDL_Event &e)
	{
		mouseEvent(e);
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
		return true;
	}
	bool Rectangle::draw(SDL_Renderer * renderer)
	{
		if (_color.a < 0xff)
		{
			SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
		}

		SDL_SetRenderDrawColor(renderer, _color.r, _color.g, _color.b, _color.a);
		SDL_RenderFillRect(renderer, &_rc);

		return true;
	}

	void Rectangle::color(const SDL_Color& color)
	{
		memcpy(&_color, &color, sizeof(SDL_Color));
	}



	void Rectangle::onClick()
	{
		printf("click Rectange\n");
	}

}