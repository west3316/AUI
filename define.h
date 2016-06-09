#pragma once

#include <SDL/SDL.h>

const SDL_Color COLOR_WHITE = { 0xff, 0xff, 0xff, 0xff };
const SDL_Color COLOR_BLACK = { 0x00, 0x00, 0x00, 0xff };
const SDL_Color DEFAULT_BG_COLOR = { 0xcc, 0xcc, 0xcc, 0xff };
const SDL_Color DEFAULT_COLOR_INPUT_BG = { 0xff, 0xff, 0xff, 0xff };
const SDL_Color DEFAULT_COLOR_TEXT = { 0x00, 0x00, 0x00, 0xff };
const int		DEFAULT_FONT_SIZE = 12;
const int		DEFAULT_INPUT_WIDTH = 80;
const int		DEFAULT_INPUT_HEIGHT = 26;

const SDL_Color DEFAULT_LABEL_BG_COLOR = { 0xaa, 0xaa, 0xaa, 0xaa };
const SDL_Color DEFAULT_BUTTON_BG_COLOR = { 0xaa, 0xaa, 0xaa, 0xff };
const int		DEFAULT_BUTTON_WIDTH = 100;
const int		DEFAULT_BUTTON_HEIGHT = 30;

const int		APP_TIME = SDL_USEREVENT + 1;

#define DEFAULT_FONT_NAME		"msyh.ttf"
#define DEFAULT_PASSWORD_CHAR	"*"

enum VerticalAlign
{
	V_TOP,
	V_MIDDLE,
	V_BOTTOM,
};

enum HorizontalAlign
{
	H_LEFT,
	H_CENTER,
	H_RIGHT,
};

struct Padding 
{
	int left;
	int right;
	int top;
	int bottom;
	Padding(int left = 0, int right = 0, int top = 0, int bottom = 0){}
};


typedef void (*LayoutFunc)(const SDL_Rect &, SDL_Rect &);