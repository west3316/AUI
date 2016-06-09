#pragma once

#include <string>

#include "widget.h"
#include "define.h"
#include "utils.h"



class Label :
	public Widget
{
public:
	Label(Window & window, const std::string &txt = "");

	virtual bool preload();
	virtual bool draw(SDL_Renderer * renderer);
	void text(const std::string &txt);
	std::string &text();

	
	void textColor(const SDL_Color& color);
	void font(const char *fontName);
	void fontSize(int size);
	void fontStyle(int style);
	void bgColor(const SDL_Color& color);
	void outline(int outline, const SDL_Color& color = COLOR_WHITE);
	void showBg(bool value);

	bool showBg();

	void layout(LayoutFunc horizontal, LayoutFunc vertical);

	virtual bool setViewPort(const TextRenderInfo &info, SDL_Rect &rcText, SDL_Rect &rcViewPort);
	const SDL_Rect & getTextRect();

	void shadow();
protected:
	int			_outline;
	int			_textSize;
	SDL_Color	_textColor;
	SDL_Color	_bgColor;
	SDL_Color	_outlineColor;
	std::string _text;
	const char	*_fontName;
	LayoutFunc	_layoutHorizontal;
	LayoutFunc	_layoutVertical;
	SDL_Rect	_textRect;
	int			_fontStyle;
	bool		_showBg;
};
