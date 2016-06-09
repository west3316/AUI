#include "input.h"
#include "utils.h"
#include "define.h"
#include "window.h"


Input::Input(Window & window):
	Label(window, "")
{
	textColor(DEFAULT_COLOR_TEXT);
	bgColor(DEFAULT_COLOR_INPUT_BG);
	position(0, 0);
	size(DEFAULT_INPUT_WIDTH, DEFAULT_INPUT_HEIGHT);
	showBg(true);
	layout(LayoutLeft, LayoutMiddle);
	padding(1, 3, 2, 2);
	_charLimit = 256;
	_bGrating = false;
	_asciiOnly = false;
	_passwdOnly = false;
	_passwdMaskChar = DEFAULT_PASSWORD_CHAR;
	SDL_SetTextInputRect(&_rc);
}

void Input::passwdOnly(bool value, const std::string &sign, const std::string &mask)
{
	_passwdOnly = true;
	if (_passwdOnly)
	{
		_asciiOnly = true;
		_validPasswdSign = sign;
		_passwdMaskChar = mask;
	}
	else
	{
		_asciiOnly = false;
		_passwdMaskChar = DEFAULT_PASSWORD_CHAR;
		_validPasswdSign.clear();
	}
}

bool Input::isValidPasswd(const std::string &s)
{
	for (auto c : s)
	{
		auto it = _validPasswdSign.find(c);
		if (!isalnum(c) && it == std::string::npos)
		{
			return false;
		}
	}

	return true;
}

std::string Input::password()
{
	return _passwd;
}

void Input::clear()
{
	_text.clear();
	_passwd.clear();
}

bool Input::event(SDL_Event & e)
{
	switch (e.type)
	{
	case SDL_TEXTINPUT:
	{
		if (_asciiOnly && !testAscii(e.text.text))
			break;

		size_t before = _text.size();
		if (!_passwdOnly)
		{
			_text += e.text.text;
		}
		else
		{ 
			if (!isValidPasswd(e.text.text))
				break;

			_passwd += e.text.text;
			while (_passwd.size() > _charLimit)
				u8Pop(_passwd);

			_text += _passwdMaskChar;
		}			

		while (_text.size() > _charLimit)
			u8Pop(_text);

		//std::string input;
		//u8ToGbk(!_passwdOnly ? _text : _passwd, input);
		//printf("_text=%s\n", input.c_str());
		//printf("u8Len=%zu\n", u8Len(_text));
		break;
	}
	case APP_TIME:
	{
		_bGrating = !_bGrating;
		break;
	}

	case SDL_KEYUP:
	{
		if (e.key.state == SDL_RELEASED && e.key.keysym.sym ==  SDLK_BACKSPACE && !_text.empty())
		{
			size_t before = _text.size();
			u8Pop(_text);

			if (_passwdOnly)
				u8Pop(_passwd);
		}
	}
	default:
		break;
	}
	return true;  
}

bool Input::setViewPort(const TextRenderInfo &info, SDL_Rect &rcText, SDL_Rect &rcViewPort)
{
	if (_text.empty())
		return false;

	memcpy(&rcViewPort, &rcText, sizeof(SDL_Rect));
	rcViewPort.x = 0;
	rcViewPort.y = 0;

	SDL_Rect rcPading;
	getPaddingRect(rcPading);
	if (rcText.w <= rcPading.w)
		return true;
		

	rcViewPort.w = rcText.w; 
	rcViewPort.h = rcText.h;

	rcViewPort.x = rcText.w - rcPading.w;
	rcViewPort.w = rcPading.w;

	rcText.w = rcPading.w;

	return true;
}


bool Input::draw(SDL_Renderer * renderer)
{
	//static clock_t clock = std::clock();

	Label::draw(renderer);
	SDL_Rect rcText = { 0, 0, 0, 0 };
	if (!_text.empty()) 
	{
		memcpy(&rcText, &getTextRect(), sizeof(rcText));
	}

	SDL_Rect rcPading;
	getPaddingRect(rcPading);
	SDL_Rect dstText = { rcPading.x, rcPading.y, rcText.w, rcText.h };
	if (_bGrating)
	{
		int x = rcPading.x + dstText.w + 1;				// +1 for grating
		SDL_SetRenderDrawColor(renderer, 0xcc, 0xcc, 0xcc, 0xff);
		SDL_RenderDrawLine(renderer, x, rcPading.y, x, rcPading.y + rcPading.h);
	}

	return true;
}

void Input::charLimit(size_t n)
{
	_charLimit = n;
}

void Input::asciiOnly(bool value)
{
	_asciiOnly = value;
}


