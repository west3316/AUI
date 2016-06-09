#include "layer.h"
#include "widget.h"
#include "window.h"
#include "utils.h"

Layer::Layer():
_mouseOverWidgetId(-1)
{

}

Layer::~Layer()
{
	for (auto it : _data)
	{
		delete it;
	}
}

size_t Layer::add(Widget * w, Window *win)
{
	size_t id = _data.size();
	_data.push_back(w);
	w->id((int)id);
	w->window(win);
	return id;
}

void Layer::visable(size_t fid, bool flag)
{
	_data[fid]->visable(flag);
}

void Layer::enable(size_t fid, bool flag)
{
	_data[fid]->enable(flag);
}

bool Layer::preload()
{
	for (auto it : _data)
	{
		if (!it->preload())
			return false;
	}
	return true;
}

bool Layer::draw(SDL_Renderer *renderer)
{
	SDL_Texture *tx = NULL;
	bool ret = true;
	for (auto it : _data)
	{
		if (!it->visable())
			continue;

		if (!it->texture())
		{
			ret = it->draw(renderer);
			if (!ret)
				break;

			if (it->blurSize() > 0)
			{
				tx = stackBlur(renderer, SDL_GetWindowSurface(it->window()->toSDL()), it->blurSize(), it->rect());
				SDL_RenderCopy(renderer, tx, NULL, &it->rect());
				it->textureNoFree(tx);
			}
		}
		else
			SDL_RenderCopy(renderer, it->texture(), NULL, &it->rect());
	}

	return ret;
}

bool Layer::event(SDL_Event &e)
{
	if (e.type == SDL_MOUSEMOTION)
	{
		checkMouseInRect(e.motion.x, e.motion.y);
	}

	bool ret = true;
	for (auto it : _data)
	{
		if (!it->enable())
			continue;

		ret = it->event(e);
		if (!ret) 
		{
			break;
		}
	}

	return ret;
}


void Layer::checkMouseInRect(int x, int y)
{
	for (int i = (int)(_data.size() - 1); i >= 0; --i)
	{
		SDL_Rect rc;
		memcpy(&rc, &_data[i]->rect(), sizeof(rc));
		if (!_data[i]->mouseable())
			continue;

		if (x >= rc.x && x <= rc.x + rc.w
			&& y >= rc.y && y <= rc.y + rc.h)
		{
			_mouseOverWidgetId = _data[i]->id();
			//printf("_mouseOverWidgetId=%d\n", i);
			return;
		}
	}

	_mouseOverWidgetId = -1;
}

int Layer::mouseOverWidget()
{
	if (_mouseOverWidgetId  == -1)
		return NULL;

	return _data[_mouseOverWidgetId]->id();
}

void Layer::swap(size_t fid1, size_t fid2)
{
	Widget *tmp = _data[fid1];
	_data[fid1] = _data[fid2];
	_data[fid2] = tmp;
}

Widget * Layer::get(size_t fid)
{
	return _data[fid];
}

void Layer::owner(uint32_t wid)
{
	_windowId = wid;
}
