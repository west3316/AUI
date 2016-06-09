#pragma once

#include "define.h"
#include "label.h"

class Input :
	public Label
{
public:
	Input(Window & window);
	virtual bool event(SDL_Event &e);
	virtual bool draw(SDL_Renderer * renderer);
	virtual bool setViewPort(const TextRenderInfo &info, SDL_Rect &rcText, SDL_Rect &rcViewPort);

	void charLimit(size_t n);
	void asciiOnly(bool value);
	void passwdOnly(bool value, const std::string &sign = "", const std::string &mask = DEFAULT_PASSWORD_CHAR);
	bool isValidPasswd(const std::string &s);
	std::string password();
	void clear();
private:
	size_t		_charLimit;
	bool		_bGrating;
	bool		_asciiOnly;
	bool		_passwdOnly;
	std::string	_validPasswdSign;
	std::string	_passwdMaskChar;
	std::string _passwd;
};