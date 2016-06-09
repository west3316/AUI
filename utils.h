#pragma once

#include <SDL/SDL_ttf.h>
#include <string>

class Window;

#include "define.h"

std::string & toU8(const std::string & src, std::string & dst);
std::string & u8ToGbk(const std::string & src, std::string & dst);
std::string & u8Pop(std::string & s);
size_t u8Len(const std::string & s);
bool testAscii(const std::string & s);

struct FontStyle
{
	const char	*name;
	int			size;
	int			style;
	int			outline;
	int			gap;
	SDL_Color	fg;
	SDL_Color	bg;
	FontStyle(const char *name = DEFAULT_FONT_NAME, int size = DEFAULT_FONT_SIZE, SDL_Color fg = { 0, 0, 0, 0 }, SDL_Color bg = { 0xff, 0xff, 0xff, 0xff }, int style = TTF_STYLE_NORMAL, int outline = 1) :
		name(name),
		size(size),
		style(style),
		fg(fg),
		outline(outline)
	{}
};

struct TextRenderInfo
{
	int				width;
	int				height;
	SDL_Texture*	texture;

	std::string & getHash() { return hash; }
	bool operator==(const TextRenderInfo& other);

	TextRenderInfo(const std::string &txt, const FontStyle &style);

private:
	//MD5_HASH hash;
	std::string hash;
};


bool init();

TextRenderInfo& allocTextRender(const std::string &txt, const FontStyle &style);
TextRenderInfo * findTextRender(const std::string &txt, const FontStyle &style);

bool getTextRenderInfo(SDL_Renderer* renderer, const std::string &txt, const FontStyle &style, TextRenderInfo &info);

void LayoutLeft(const SDL_Rect &rc1, SDL_Rect &rc2);
void LayoutCenter(const SDL_Rect &rc1, SDL_Rect &rc2);
void LayoutRight(const SDL_Rect &rc1, SDL_Rect &rc2);
void LayoutTop(const SDL_Rect &rc1, SDL_Rect &rc2);
void LayoutMiddle(const SDL_Rect &rc1, SDL_Rect &rc2);
void LayoutBottom(const SDL_Rect &rc1, SDL_Rect &rc2);

void layout(const SDL_Rect &rc1, SDL_Rect &rc2, LayoutFunc horizontal = LayoutLeft, LayoutFunc vertical = LayoutTop);

void BlurRect(SDL_Window *window, const SDL_Rect &rc, int blur_size);

SDL_Texture *stackBlur(SDL_Renderer *renderer, SDL_Surface *pOrg, unsigned radius, const SDL_Rect &rc);
SDL_Texture *createTextureFromSurface(SDL_Renderer *renderer, SDL_Surface *pOrg, const SDL_Rect *rcClip);

void round(SDL_Surface *sfWindow, const SDL_Rect &rcOrg, Uint32 color, int radius);