#include "widget.h"
#include "window.h"
#include "utils.h"

Widget::Widget(Window & window) :
	_window(&window),
	_id(INVALID_WIDGET_ID),
	_visable(true),
	_enable(true),
	_tx(NULL),
	_mouseable(false),
	_blurSize(-1)
{
	_window->add(this);
	_rc = { 0, 0, 0, 0 };
}

Widget::~Widget()
{
	//SDL_DestroyTexture(_tx);
}

//void Widget::setStatic(bool value)
//{
//	if (value)
//	{
//		texture(createTextureFromSurface(_window->renderer(), SDL_GetWindowSurface(_window->toSDL()), &_rc));
//	}
//	else
//	{
//		texture(NULL);
//	}
//}

bool Widget::mouseable()
{
	return _mouseable;
}

void Widget::mouseable(bool value)
{
	_mouseable = value;
}

bool Widget::preload()
{
	return true;
}

bool Widget::draw(SDL_Renderer * renderer)
{
	return true;
}

SDL_Texture* Widget::texture()
{
	return _tx;
}

void Widget::texture(SDL_Texture *tx)
{
	if (tx == _tx)
		return;

	if (_tx)
	{
		SDL_DestroyTexture(_tx);
		_tx = NULL;
	}

	_tx = tx;
}

void Widget::textureNoFree(SDL_Texture * tx)
{
	if (tx == _tx)
		return;

	_tx = tx;
}

bool Widget::event(SDL_Event & e)
{
	return true;
}

bool Widget::isMouseOver()
{
	return window()->mouseOverWidget() == _id;
}


void Widget::visable(bool value)
{
	_visable = value;
}

void Widget::enable(bool value)
{
	_enable = value;
}

bool Widget::visable()
{
	return _visable;
}

bool Widget::enable()
{
	return _enable;
}

void Widget::size(int w, int h)
{
	_rc.w = w;
	_rc.h = h;
}

void Widget::position(int x, int y)
{
	_rc.x = x;
	_rc.y = y;
}

void Widget::id(int id)
{
	_id = id;
}

int Widget::id()
{
	return _id;
}

void Widget::getPaddingRect(SDL_Rect & rc)
{
	rc.x = _rc.x + _padding.left;
	rc.w = _rc.w - _padding.right - _padding.left;
	rc.y = _rc.y + _padding.top;
	rc.h = _rc.h - _padding.bottom - _padding.top;
}

void Widget::padding(int left, int right, int top, int bottom)
{
	_padding.left = left;
	_padding.right = right;
	_padding.top = top;
	_padding.bottom = bottom;
}

void Widget::window(Window *win)
{
	_window = win;
}
SDL_Window *Widget::toSDL()
{
	return _window->toSDL();
}

Window *Widget::window()
{
	return _window;
}

void Widget::blur(int size)
{
	_blurSize = size;
}

int Widget::blurSize()
{
	return _blurSize;
}

SDL_Rect &Widget::rect()
{
	return _rc;
}