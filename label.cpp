#include "label.h"
#include "utils.h"
#include "window.h"




#ifdef _DEBUG
#include <fstream>
static std::ofstream xlog("c:\\lable_raw.txt", std::ios::out | std::ios::app);
#endif

Label::Label(Window & window, const std::string & txt) :
Widget(window),
_textSize(DEFAULT_FONT_SIZE),
_showBg(false),
_fontStyle(TTF_STYLE_NORMAL),
_layoutHorizontal(NULL),
_layoutVertical(NULL),
_outline(0),
_textRect({ 0, 0, 0, 0 }),
_fontName(DEFAULT_FONT_NAME)
{
	_text = txt;
	memcpy(&_textColor, &DEFAULT_COLOR_TEXT, sizeof(_textColor));
	memcpy(&_bgColor, &DEFAULT_LABEL_BG_COLOR, sizeof(_bgColor));
	memcpy(&_outlineColor, &COLOR_WHITE, sizeof(_outlineColor));
	padding(0, 0, 0, 0);

}

bool Label::setViewPort(const TextRenderInfo &info, SDL_Rect &rcText, SDL_Rect &rcViewPort)
{
	return false;
}
 

bool Label::preload()
{
	//FontStyle style;
	//style.name = _fontName;
	//style.size = _textSize;
	//style.style = _fontStyle;
	//memcpy(&style.fg, &_textColor, sizeof(_textColor));
	//memcpy(&style.bg, &_outlineColor, sizeof(_outlineColor));
	//style.outline = _outline;
	//
	//TextRenderInfo info;
	//SDL_Rect rcPading;
	//getPaddingRect(rcPading);
	//getTextRenderInfo(window()->renderer(), _text, style, info);
	//if (info.texture)
	//	SDL_DestroyTexture(info.texture);
	return true;
}

bool Label::draw(SDL_Renderer * renderer)
{
	if (_showBg)
	{
		if (_bgColor.a < 0xff)
			SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
		SDL_SetRenderDrawColor(renderer, _bgColor.r, _bgColor.g, _bgColor.b, _bgColor.a);
		SDL_RenderFillRect(renderer, &_rc);
		if (_bgColor.a < 0xff)
			SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
	}

	if (_text.empty())
		return true;

	FontStyle style;
	style.name = _fontName;
	style.size = _textSize;
	style.style = _fontStyle;
	memcpy(&style.fg, &_textColor, sizeof(_textColor));
	memcpy(&style.bg, &_outlineColor, sizeof(_outlineColor));
	style.outline = _outline;

	SDL_Rect rcPading;
	getPaddingRect(rcPading);

	TextRenderInfo *info = info = findTextRender(_text, style);
	if (!info)
	{
		info = &allocTextRender(_text, style);
		getTextRenderInfo(renderer, _text, style, *info);
		static int a = 0;
		printf("label draw text: %d\n", ++a);
	}

	_textRect.x = rcPading.x;
	_textRect.y = rcPading.y;
	_textRect.w = info->width;
	_textRect.h = info->height;

	if (_layoutHorizontal)
	{
		_layoutHorizontal(rcPading, _textRect);
	}
	if (_layoutVertical)
	{
		_layoutVertical(rcPading, _textRect);
	}

	SDL_Rect rcViewPort;
	memcpy(&rcViewPort, &_textRect, sizeof(rcViewPort));
	SDL_Rect *rcViewPortPtr = setViewPort(*info, _textRect, rcViewPort) ? &rcViewPort : NULL;

	SDL_RenderCopy(renderer, info->texture, rcViewPortPtr, &_textRect);

	//if (info->texture)
	//	SDL_DestroyTexture(info->texture);
	//shadow();
	return true;
}

void Label::shadow()
{
	enum enumColorOrder
	{
		R,
		G,
		B,
		A
	};
	const int shadowSize = 5;
	const int AlphaIndex = 3;
	// base on _rc.x, _rc.y
	SDL_Rect rcHorizontal = { _rc.x, _rc.y + _rc.h, _rc.w, shadowSize };
	SDL_Rect rcVertical = { _rc.x - shadowSize, _rc.y, shadowSize, _rc.h };
	
	SDL_Surface *sf = SDL_GetWindowSurface(window()->toSDL());
	// horizontal alpha from 100% to 20%
	Uint8 *pal;
	Uint32 *pix = pix = (Uint32*)sf->pixels;

	double v = 1.0 / shadowSize;
	SDL_LockSurface(sf);

	for (int x = rcHorizontal.x; x < (rcHorizontal.x + rcHorizontal.w - shadowSize); ++x)
	{
		for (int y = rcHorizontal.y, i = 0; y < (rcHorizontal.y + rcHorizontal.h); ++y, ++i)
		{
			pal = (Uint8 *)(pix + y * sf->w + x);
			pal[R] = (Uint8)round(pal[R] * v * i);
			pal[G] = (Uint8)round(pal[G] * v * i);
			pal[B] = (Uint8)round(pal[B] * v * i);
		}
	}

	for (int y = (rcVertical.y + shadowSize); y < (rcVertical.y + rcVertical.h); ++y)
	{
		for (int x = (rcVertical.x + rcVertical.w), i = 0; x >= rcVertical.x; --x, ++i)
		{
				pal = (Uint8 *)(pix + y * sf->w + x);
				pal[R] = (Uint8)round(pal[R] * v * i);
				pal[G] = (Uint8)round(pal[G] * v * i);
				pal[B] = (Uint8)round(pal[B] * v * i);
			}
	}
	SDL_UnlockSurface(sf);
}

void Label::showBg(bool value)
{
	_showBg = value;
}

bool Label::showBg()
{
	return _showBg;
}

void Label::text(const std::string & txt)
{
	_text = txt;
}

std::string & Label::text()
{
	return _text;
}

void Label::textColor(const SDL_Color & color)
{
	memcpy(&_textColor, &color, sizeof(_textColor));
}

void Label::font(const char *fontName)
{
	_fontName = fontName;
}

void Label::fontSize(int size)
{
	_textSize = size;
}

void Label::fontStyle(int style)
{
	_fontStyle = style;
}

void Label::bgColor(const SDL_Color & color)
{
	memcpy(&_bgColor, &color, sizeof(_bgColor));
}

void Label::outline(int outline, const SDL_Color& color)
{
	_outline = outline;
	if (_outline > 0)
		memcpy(&_outlineColor, &color, sizeof(_outlineColor));
}
 

void Label::layout(LayoutFunc horizontal, LayoutFunc vertical)
{
	_layoutHorizontal = horizontal;
	_layoutVertical = vertical;
}

const SDL_Rect & Label::getTextRect()
{
	return _textRect;
}