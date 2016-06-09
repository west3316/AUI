#pragma once

#include <cstdint>
#include <vector>
#include <bitset>

class Window;
class Widget;
struct SDL_Renderer;
union SDL_Event;

class Layer
{
public:
	size_t add(Widget *w, Window *win);
	
	void visable(size_t fid, bool flag = true);
	void enable(size_t fid, bool flag = true);
	void swap(size_t fid1, size_t fid2);

	Widget *get(size_t fid);


	bool preload();
	bool draw(SDL_Renderer *renderer);
	bool event(SDL_Event &e);

	~Layer();
	Layer();

	void owner(uint32_t wid);
	int mouseOverWidget();

private:
	void checkMouseInRect(int x, int y);
private:
	std::vector<Widget *>	_data;
	uint32_t				_windowId;
	int						_mouseOverWidgetId;
};