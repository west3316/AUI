#include <Windows.h>
#include <map>



extern "C" {
#include "LibMd5.h"
}

#include "utils.h"
#include "window.h"




#include <iostream>
static __inline void error(const char* msg)
{
	std::cout << "ERROR: " << msg << std::endl;
}


static uint32_t	g_bufferUnicodeSize = 4096;
static uint32_t	g_bufferSize = 4096;
static wchar_t * g_bufferUnicode = new wchar_t[g_bufferUnicodeSize];
static char * g_buffer = new char[g_bufferSize];





using FontMap = std::map<std::string, TTF_Font*>;
struct TextRenderCompare {
	bool operator() (const std::string& lvalue, const std::string& rvalue) const
	{return memcmp(lvalue.c_str(), rvalue.c_str(), MD5_HASH_SIZE) < 0;}
};
typedef std::map<std::string, TextRenderInfo*, TextRenderCompare>		TextRenders;


static TextRenders					g_textRenders;
static FontMap						g_fontMap;
static char							g_sysPath[MAX_PATH];




bool initFont();
TTF_Font *getFont(const char *name, int size);
void fontStyleHash(const std::string & txt, const FontStyle & style, MD5_HASH& hash);



bool init()
{
	if (!initFont())
		return false;
	return true;
}


TextRenderInfo & allocTextRender(const std::string &txt, const FontStyle &style)
{
	TextRenderInfo *tri = new TextRenderInfo(txt, style);
	g_textRenders.insert(TextRenders::value_type(tri->getHash(), tri));
	return *tri;
}

TextRenderInfo * findTextRender(const std::string &txt, const FontStyle &style)
{
	std::string hash;
	MD5_HASH md5;
	fontStyleHash(txt, style, md5);
	for (int i = 0; i < MD5_HASH_SIZE; ++i)
		hash.push_back(md5.bytes[i]);

	auto it = g_textRenders.find(hash);
	if (it == g_textRenders.end())
		return NULL;

	return it->second;
}

void testRellocUnicode(uint32_t size)
{
	if (size > g_bufferUnicodeSize)
	{
		delete g_bufferUnicode;
		g_bufferUnicodeSize = size;
		g_bufferUnicode = new wchar_t[g_bufferUnicodeSize];
	}
}

void testRelloce(uint32_t size)
{
	if (size > g_bufferSize)
	{
		delete g_buffer;
		g_bufferSize = size;
		g_buffer = new char[g_bufferSize];
	}
}

std::string & toU8(const std::string & src, std::string & dst)
{
	int nRetLen;
	dst.clear();
	nRetLen = MultiByteToWideChar(CP_ACP, 0, src.c_str(), -1, NULL, 0);
	testRellocUnicode(nRetLen + 1);
	MultiByteToWideChar(CP_ACP, 0, src.c_str(), -1, g_bufferUnicode, nRetLen);
	nRetLen = WideCharToMultiByte(CP_UTF8, 0, g_bufferUnicode, -1, NULL, 0, NULL, NULL);
	if (!nRetLen)
		return dst;

	testRelloce(nRetLen + 1);
	nRetLen = WideCharToMultiByte(CP_UTF8, 0, g_bufferUnicode, -1, g_buffer, nRetLen, NULL, NULL);
	if (!nRetLen)
		return dst;

	dst = g_buffer;
	return dst;
}

std::string & u8ToGbk(const std::string & src, std::string & dst)
{
	int nRetLen;
	dst.clear();
	nRetLen = ::MultiByteToWideChar(CP_UTF8, 0, src.c_str(), -1, NULL, NULL);  //获取转换到Unicode编码后所需要的字符空间长度
	testRellocUnicode(nRetLen + 1);
	nRetLen = ::MultiByteToWideChar(CP_UTF8, 0, src.c_str(), -1, g_bufferUnicode, nRetLen);  //转换到Unicode编码
	if (!nRetLen)  //转换失败则出错退出
		return dst;

	
	nRetLen = ::WideCharToMultiByte(CP_ACP, 0, g_bufferUnicode, -1, NULL, NULL, NULL, NULL);  //获取转换到GBK编码后所需要的字符空间长度
	if (nRetLen > (int)g_bufferUnicodeSize)
		testRellocUnicode(nRetLen);

	nRetLen = ::WideCharToMultiByte(CP_ACP, 0, g_bufferUnicode, -1, g_buffer, nRetLen, NULL, NULL);  //转换到GBK编码
	if (!nRetLen)
		return dst;

	dst = g_buffer;
	return dst;
}

bool testAscii(const std::string & s)
{
	for (size_t i = 0; i < s.length();)
	{
		size_t cnt = 0;
		for (int n = 7; n > 1; --n)
		{
			if (!(s[i] & (1 << n)))
				break;
			++cnt;
		}
		if (!cnt)
			cnt = 1;

		if (cnt > 1)
			return false;

		if (cnt >= s.length())
			break;


		i += cnt;
	}
	return true;
}

size_t u8Len(const std::string & s)
{
	size_t len = 0;
	for (size_t i = 0; i < s.length();)
	{
		size_t cnt = 0;
		for (int n = 7; n > 1; --n)
		{
			if (!(s[i] & (1 << n)))
			{
				++len;
				break;
			}
			++cnt;
		}
		if (!cnt)
			cnt = 1;

		if (cnt >= s.length())
			break;

		i += cnt;
	}
	return len;
}

std::string & u8Pop(std::string & s)
{
	size_t prev = 0;
	for (size_t i = 0; i < s.length();)
	{
		size_t len = 0;
		for (int n = 7; n > 1; --n)
		{
			if (!(s[i] & (1 << n)))
				break;
			++len;
		}

		if (!len)
			len = 1;

		if (len >= s.length())
			break;


		prev = i;
		i += len;
	}

	s = s.substr(0, prev);
	return s;
}

bool initFont()
{
	UINT len = GetWindowsDirectoryA(g_sysPath, MAX_PATH);
	if (!len)
	{
		error("GetWindowsDirectoryA failed");
		return false;
	}
		

	g_sysPath[len] = 0;

	return !TTF_Init();
}

TTF_Font * getFont(const char * name, int size)
{
	// key = font name:font size
	std::string key = name;
	key += ":";
	key += std::to_string(size);

	auto it = g_fontMap.find(key);
	if (it != g_fontMap.cend())
	{
		return it->second;
	}

	std::string file = g_sysPath;
	file += "\\Fonts\\";
	file += name;

	std::cout << "load font: " << file << size << std::endl;
	TTF_Font *pFont = TTF_OpenFont(file.c_str(), size);
	if (!pFont)
	{
		std::cout << "TTF_OpenFont failed: " << file << std::endl;
		return NULL;
	}

	g_fontMap.insert(FontMap::value_type(key, pFont));

	return pFont;
}

bool getTextRenderInfo(SDL_Renderer *renderer, const std::string &txt, const FontStyle &style, TextRenderInfo &info)
{
	if (txt.empty())
		return false;

	TTF_Font * font = getFont(style.name, style.size);
	if (!font)
	{
		std::cout << "getFont: " << style.name << style.size << std::endl;
		return false;
	}
		
	if (TTF_GetFontStyle(font) != style.style)
		TTF_SetFontStyle(font, style.style);

	int w, h;
	TTF_SizeUTF8(font, txt.c_str(), &w, &h);
	SDL_Surface *sf = NULL;
	SDL_Surface *sfTemp = NULL;

	if (style.outline > 0)
	{
		//std::cout << "style.outline" << style.outline << std::endl;
		TTF_SetFontOutline(font, style.outline);
		sf = TTF_RenderUTF8_Blended(font, txt.c_str(), style.bg);
		if (!sf)
			return false;
		TTF_SetFontOutline(font, 0);
	}
	
	//std::cout << "TTF_RenderUTF8_Blended("<<txt.length() <<") :" << txt.c_str() << std::endl;
	sfTemp = TTF_RenderUTF8_Blended(font, txt.c_str(), style.fg);
	if (!sfTemp)
		return false;

	if (style.outline > 0)
	{
		SDL_BlitSurface(sfTemp, NULL, sf, NULL);
		//SDL_FreeSurface(sfTemp);		
	}
	else
	{
		sf = sfTemp;
	}
	
	TTF_SizeUTF8(font, txt.c_str(), &info.width, &info.height);
	info.texture = SDL_CreateTextureFromSurface(renderer, sf);
	TTF_SetFontStyle(font, TTF_STYLE_NORMAL);
	//SDL_FreeSurface(sf);
	sf = NULL;

	return true;
}

void LayoutLeft(const SDL_Rect &rc1, SDL_Rect &rc2)
{
	rc2.x = rc1.x;
}

void LayoutCenter(const SDL_Rect &rc1, SDL_Rect &rc2)
{
	rc2.x = rc1.x + rc1.w / 2 - rc2.w / 2;

}

void LayoutRight(const SDL_Rect &rc1, SDL_Rect &rc2)
{
	rc2.x = rc1.x + rc1.w - rc2.w;
}

void LayoutTop(const SDL_Rect &rc1, SDL_Rect &rc2)
{
	rc2.y = rc1.y;
}

void LayoutMiddle(const SDL_Rect &rc1, SDL_Rect &rc2)
{
	rc2.y = rc1.y + rc1.h / 2 - rc2.h / 2;
}

void LayoutBottom(const SDL_Rect &rc1, SDL_Rect &rc2)
{
	rc2.y = rc1.y + rc1.h - rc2.h;
}


Uint32 getpixel(SDL_Surface *surface, int x, int y)
{
	int bpp = surface->format->BytesPerPixel;
	/* Here p is the address to the pixel we want to retrieve */
	Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

	switch (bpp) {
	case 1:
		return *p;
		break;

	case 2:
		return *(Uint16 *)p;
		break;

	case 3:
		if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
			return p[0] << 16 | p[1] << 8 | p[2];
		else
			return p[0] | p[1] << 8 | p[2] << 16;
		break;

	case 4:
		return *(Uint32 *)p;
		break;

	default:
		return 0;       /* shouldn't happen, but avoids warnings */
	}
}

void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
	int bpp = surface->format->BytesPerPixel;
	/* Here p is the address to the pixel we want to set */
	Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

	switch (bpp) {
	case 1:
		*p = pixel;
		break;

	case 2:
		*(Uint16 *)p = pixel;
		break;

	case 3:
		if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
			p[0] = (pixel >> 16) & 0xff;
			p[1] = (pixel >> 8) & 0xff;
			p[2] = pixel & 0xff;
		}
		else {
			p[0] = pixel & 0xff;
			p[1] = (pixel >> 8) & 0xff;
			p[2] = (pixel >> 16) & 0xff;
		}
		break;

	case 4:
		*(Uint32 *)p = pixel;
		break;
	}
}

void BlurRect(SDL_Window * window, const SDL_Rect &rc, int blur_size)
{
	if (blur_size < 1)
		return;

	SDL_Surface *pOrg = SDL_GetWindowSurface(window);
	if (!pOrg)
		return;

	SDL_Renderer *renderer = SDL_GetRenderer(window);
	if (!renderer)
		return;

	SDL_LockSurface(pOrg);

	Uint32 p;
	Uint8 r, g, b, a;
	int x, y, px, py;
	int blur_sum_factor;
	double rd, gd, bd, ad;
	int x1, y1;
	const int width = rc.w + rc.x;
	const int height = rc.h + rc.y;
	SDL_Rect rcWindow = {0, 0, 0, 0};
	SDL_GetWindowPosition(window, &rcWindow.x, &rcWindow.y);
	SDL_GetWindowSize(window, &rcWindow.w, &rcWindow.h);
	

	for (y = rc.y; y < height; y++)
	{
		for (x = rc.x; x < width; x++)
		{
			rd = gd = bd = ad = 0.0;
			blur_sum_factor = 0;
			for (py = -blur_size; py <= blur_size; py++)
			{
				for (px = -blur_size; px <= blur_size; px++)
				{
					x1 = x + px;
					y1 = y + py;
					if (x1 < 0 || x1 >= width || y1 < 0 || y1 >= height)
						continue;

					p = getpixel(pOrg, x1, y1);
					SDL_GetRGBA(p, pOrg->format, &r, &g, &b, &a);
					rd += r;
					gd += g;
					bd += b;
					blur_sum_factor++;
					ad += a;
				}
			}
			rd /= blur_sum_factor;
			gd /= blur_sum_factor;
			bd /= blur_sum_factor;
			ad /= blur_sum_factor;
			r = (Uint8)rd;
			g = (Uint8)gd;
			b = (Uint8)bd;
			a = (Uint8)ad;
			p = SDL_MapRGBA(pOrg->format, r, g, b, a);
			putpixel(pOrg, x, y, p);
		}
	}

	SDL_UnlockSurface(pOrg);
	SDL_FreeSurface(pOrg);
}





static unsigned int stack_blur8_mul[] =
{
	512, 512, 456, 512, 328, 456, 335, 512, 405, 328, 271, 456, 388, 335, 292, 512,
	454, 405, 364, 328, 298, 271, 496, 456, 420, 388, 360, 335, 312, 292, 273, 512,
	482, 454, 428, 405, 383, 364, 345, 328, 312, 298, 284, 271, 259, 496, 475, 456,
	437, 420, 404, 388, 374, 360, 347, 335, 323, 312, 302, 292, 282, 273, 265, 512,
	497, 482, 468, 454, 441, 428, 417, 405, 394, 383, 373, 364, 354, 345, 337, 328,
	320, 312, 305, 298, 291, 284, 278, 271, 265, 259, 507, 496, 485, 475, 465, 456,
	446, 437, 428, 420, 412, 404, 396, 388, 381, 374, 367, 360, 354, 347, 341, 335,
	329, 323, 318, 312, 307, 302, 297, 292, 287, 282, 278, 273, 269, 265, 261, 512,
	505, 497, 489, 482, 475, 468, 461, 454, 447, 441, 435, 428, 422, 417, 411, 405,
	399, 394, 389, 383, 378, 373, 368, 364, 359, 354, 350, 345, 341, 337, 332, 328,
	324, 320, 316, 312, 309, 305, 301, 298, 294, 291, 287, 284, 281, 278, 274, 271,
	268, 265, 262, 259, 257, 507, 501, 496, 491, 485, 480, 475, 470, 465, 460, 456,
	451, 446, 442, 437, 433, 428, 424, 420, 416, 412, 408, 404, 400, 396, 392, 388,
	385, 381, 377, 374, 370, 367, 363, 360, 357, 354, 350, 347, 344, 341, 338, 335,
	332, 329, 326, 323, 320, 318, 315, 312, 310, 307, 304, 302, 299, 297, 294, 292,
	289, 287, 285, 282, 280, 278, 275, 273, 271, 269, 267, 265, 263, 261, 259
};

static unsigned int stack_blur8_shr[] =
{
	9, 11, 12, 13, 13, 14, 14, 15, 15, 15, 15, 16, 16, 16, 16, 17,
	17, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 18, 18, 18, 18, 19,
	19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 20, 20, 20,
	20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 21,
	21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
	21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 22, 22, 22, 22, 22, 22,
	22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
	22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 23,
	23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
	23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
	23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
	23, 23, 23, 23, 23, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24
};


enum enumColorOrder
{
	R,
	G,
	B,
	A
};


void rectCopy(unsigned long *dst, const SDL_Rect &rc, unsigned long *src, unsigned w)
{
	int x, X;
	for (int i = rc.y; i < (rc.y + rc.h); ++i)
	{
		X = i * w + rc.x;
		x = i * rc.w;
		memcpy(&dst[x], &src[X], rc.w * sizeof(unsigned long));
	}
}

void rectUpdate(unsigned long *dst, unsigned w, unsigned long *src, const SDL_Rect &rc)
{
	int x, X;
	for (int i = rc.y; i < (rc.y + rc.h); ++i)
	{
		X = i * w + rc.x;
		x = i * rc.w;
		memcpy(&dst[X], &src[x], rc.w * sizeof(unsigned long));
	}
}
 
void stack_blur_rgba32(unsigned long* pImage, unsigned w, unsigned h, unsigned r);
SDL_Texture * stackBlur(SDL_Renderer *renderer, SDL_Surface *pOrg, unsigned radius, const SDL_Rect &rc)
{
	SDL_Surface *sf = SDL_CreateRGBSurface(0, rc.w, rc.h, 32, 0, 0, 0, 0);
	SDL_Rect rcSf = {0, 0, rc.w, rc.h};
	SDL_BlitSurface(pOrg, &rc, sf, &rcSf);
	stack_blur_rgba32((unsigned long*)sf->pixels, sf->w, sf->h, radius);
	SDL_Texture * tx = SDL_CreateTextureFromSurface(renderer, sf);
	SDL_FreeSurface(sf);
	return tx;
}

SDL_Texture *createTextureFromSurface(SDL_Renderer *renderer, SDL_Surface *pOrg, const SDL_Rect *rc)
{
	SDL_Surface *sf = SDL_CreateRGBSurface(0, rc->w, rc->h, 32, 0, 0, 0, 0);
	SDL_Rect rcSf = { 0, 0, rc->w, rc->h };
	SDL_BlitSurface(pOrg, rc, sf, &rcSf);
	SDL_Texture * tx = SDL_CreateTextureFromSurface(renderer, sf);
	SDL_FreeSurface(sf);
	return tx;
}

void stack_blur_rgba32(unsigned long* pImage, unsigned w, unsigned h, unsigned r)
{
	unsigned x = 0, y = 0, xp = 0, yp = 0, i = 0, t = 0;
	unsigned stack_ptr = 0;
	unsigned stack_start = 0;

	unsigned char* src_pix_ptr = NULL;
	unsigned char* dst_pix_ptr = NULL;
	unsigned char* stack_pix_ptr = NULL;
	unsigned long* stack_data_ptr = NULL;
	unsigned long* lpStack = NULL;

	unsigned sum_r = 0;
	unsigned sum_g = 0;
	unsigned sum_b = 0;
	unsigned sum_a = 0;
	unsigned sum_in_r = 0;
	unsigned sum_in_g = 0;
	unsigned sum_in_b = 0;
	unsigned sum_in_a = 0;
	unsigned sum_out_r = 0;
	unsigned sum_out_g = 0;
	unsigned sum_out_b = 0;
	unsigned sum_out_a = 0;

	unsigned wm = (w - 1);
	unsigned hm = (h - 1);
	unsigned div = 0;
	unsigned mul_sum = 0;
	unsigned shr_sum = 0;
	unsigned row_addr = 0;
	unsigned stride = 0;

	if ((NULL == pImage) || (w < 1 || h < 1 || r < 1))
	{
		return;
	}
	else if (r > 254)
	{
		r = 254;
	}

	mul_sum = stack_blur8_mul[r];
	shr_sum = stack_blur8_shr[r];

	div = ((r + r) + 1);

	lpStack = new unsigned long[div];
	stack_data_ptr = lpStack;
	y = 0;

	do
	{
		sum_r =
			sum_g =
			sum_b =
			sum_a =
			sum_in_r =
			sum_in_g =
			sum_in_b =
			sum_in_a =
			sum_out_r =
			sum_out_g =
			sum_out_b =
			sum_out_a = 0;

		row_addr = (y * w);
		src_pix_ptr = (unsigned char*)(pImage + row_addr);
		i = 0;

		do
		{
			t = (i + 1);

			stack_pix_ptr = (unsigned char*)(stack_data_ptr + i);

			*(stack_pix_ptr + R) = *(src_pix_ptr + R);
			*(stack_pix_ptr + G) = *(src_pix_ptr + G);
			*(stack_pix_ptr + B) = *(src_pix_ptr + B);
			*(stack_pix_ptr + A) = *(src_pix_ptr + A);

			sum_r += (*(stack_pix_ptr + R) * t);
			sum_g += (*(stack_pix_ptr + G) * t);
			sum_b += (*(stack_pix_ptr + B) * t);
			sum_a += (*(stack_pix_ptr + A) * t);

			sum_out_r += *(stack_pix_ptr + R);
			sum_out_g += *(stack_pix_ptr + G);
			sum_out_b += *(stack_pix_ptr + B);
			sum_out_a += *(stack_pix_ptr + A);

			if (i > 0)
			{
				t = (r + 1 - i);

				if (i <= wm)
				{
					src_pix_ptr += 4;
				}

				stack_pix_ptr = (unsigned char*)(stack_data_ptr + (i + r));

				*(stack_pix_ptr + R) = *(src_pix_ptr + R);
				*(stack_pix_ptr + G) = *(src_pix_ptr + G);
				*(stack_pix_ptr + B) = *(src_pix_ptr + B);
				*(stack_pix_ptr + A) = *(src_pix_ptr + A);

				sum_r += (*(stack_pix_ptr + R) * t);
				sum_g += (*(stack_pix_ptr + G) * t);
				sum_b += (*(stack_pix_ptr + B) * t);
				sum_a += (*(stack_pix_ptr + A) * t);

				sum_in_r += *(stack_pix_ptr + R);
				sum_in_g += *(stack_pix_ptr + G);
				sum_in_b += *(stack_pix_ptr + B);
				sum_in_a += *(stack_pix_ptr + A);
			}
		} while (++i <= r);

		stack_ptr = r;
		xp = r;

		if (xp > wm)
		{
			xp = wm;
		}

		src_pix_ptr = (unsigned char*)(pImage + (xp + row_addr));
		dst_pix_ptr = (unsigned char*)(pImage + row_addr);
		x = 0;

		do
		{
			*(dst_pix_ptr + R) = ((sum_r * mul_sum) >> shr_sum);
			*(dst_pix_ptr + G) = ((sum_g * mul_sum) >> shr_sum);
			*(dst_pix_ptr + B) = ((sum_b * mul_sum) >> shr_sum);
			*(dst_pix_ptr + A) = ((sum_a * mul_sum) >> shr_sum);

			dst_pix_ptr += 4;

			sum_r -= sum_out_r;
			sum_g -= sum_out_g;
			sum_b -= sum_out_b;
			sum_a -= sum_out_a;

			stack_start = (stack_ptr + div - r);

			if (stack_start >= div)
			{
				stack_start -= div;
			}

			stack_pix_ptr = (unsigned char*)(stack_data_ptr + stack_start);

			sum_out_r -= *(stack_pix_ptr + R);
			sum_out_g -= *(stack_pix_ptr + G);
			sum_out_b -= *(stack_pix_ptr + B);
			sum_out_a -= *(stack_pix_ptr + A);

			if (xp < wm)
			{
				src_pix_ptr += 4;
				++xp;
			}

			*(stack_pix_ptr + R) = *(src_pix_ptr + R);
			*(stack_pix_ptr + G) = *(src_pix_ptr + G);
			*(stack_pix_ptr + B) = *(src_pix_ptr + B);
			*(stack_pix_ptr + A) = *(src_pix_ptr + A);

			sum_in_r += *(stack_pix_ptr + R);
			sum_in_g += *(stack_pix_ptr + G);
			sum_in_b += *(stack_pix_ptr + B);
			sum_in_a += *(stack_pix_ptr + A);

			sum_r += sum_in_r;
			sum_g += sum_in_g;
			sum_b += sum_in_b;
			sum_a += sum_in_a;

			if (++stack_ptr >= div)
			{
				stack_ptr = 0;
			}

			stack_pix_ptr = (unsigned char*)(stack_data_ptr + stack_ptr);

			sum_out_r += *(stack_pix_ptr + R);
			sum_out_g += *(stack_pix_ptr + G);
			sum_out_b += *(stack_pix_ptr + B);
			sum_out_a += *(stack_pix_ptr + A);

			sum_in_r -= *(stack_pix_ptr + R);
			sum_in_g -= *(stack_pix_ptr + G);
			sum_in_b -= *(stack_pix_ptr + B);
			sum_in_a -= *(stack_pix_ptr + A);
		} while (++x < w);
	} while (++y < h);

	stride = (w << 2);
	stack_data_ptr = lpStack;
	x = 0;

	do
	{
		sum_r =
			sum_g =
			sum_b =
			sum_a =
			sum_in_r =
			sum_in_g =
			sum_in_b =
			sum_in_a =
			sum_out_r =
			sum_out_g =
			sum_out_b =
			sum_out_a = 0;

		src_pix_ptr = (unsigned char*)(pImage + x);
		i = 0;

		do
		{
			t = (i + 1);

			stack_pix_ptr = (unsigned char*)(stack_data_ptr + i);

			*(stack_pix_ptr + R) = *(src_pix_ptr + R);
			*(stack_pix_ptr + G) = *(src_pix_ptr + G);
			*(stack_pix_ptr + B) = *(src_pix_ptr + B);
			*(stack_pix_ptr + A) = *(src_pix_ptr + A);

			sum_r += (*(stack_pix_ptr + R) * t);
			sum_g += (*(stack_pix_ptr + G) * t);
			sum_b += (*(stack_pix_ptr + B) * t);
			sum_a += (*(stack_pix_ptr + A) * t);

			sum_out_r += *(stack_pix_ptr + R);
			sum_out_g += *(stack_pix_ptr + G);
			sum_out_b += *(stack_pix_ptr + B);
			sum_out_a += *(stack_pix_ptr + A);

			if (i > 0)
			{
				t = (r + 1 - i);

				if (i <= hm)
				{
					src_pix_ptr += stride;
				}

				stack_pix_ptr = (unsigned char*)(stack_data_ptr + (i + r));

				*(stack_pix_ptr + R) = *(src_pix_ptr + R);
				*(stack_pix_ptr + G) = *(src_pix_ptr + G);
				*(stack_pix_ptr + B) = *(src_pix_ptr + B);
				*(stack_pix_ptr + A) = *(src_pix_ptr + A);

				sum_r += (*(stack_pix_ptr + R) * t);
				sum_g += (*(stack_pix_ptr + G) * t);
				sum_b += (*(stack_pix_ptr + B) * t);
				sum_a += (*(stack_pix_ptr + A) * t);

				sum_in_r += *(stack_pix_ptr + R);
				sum_in_g += *(stack_pix_ptr + G);
				sum_in_b += *(stack_pix_ptr + B);
				sum_in_a += *(stack_pix_ptr + A);
			}
		} while (++i <= r);

		stack_ptr = r;
		yp = r;

		if (yp > hm)
		{
			yp = hm;
		}

		src_pix_ptr = (unsigned char*)(pImage + (x + (yp * w)));
		dst_pix_ptr = (unsigned char*)(pImage + x);
		y = 0;

		do
		{
			*(dst_pix_ptr + R) = ((sum_r * mul_sum) >> shr_sum);
			*(dst_pix_ptr + G) = ((sum_g * mul_sum) >> shr_sum);
			*(dst_pix_ptr + B) = ((sum_b * mul_sum) >> shr_sum);
			*(dst_pix_ptr + A) = ((sum_a * mul_sum) >> shr_sum);

			dst_pix_ptr += stride;

			sum_r -= sum_out_r;
			sum_g -= sum_out_g;
			sum_b -= sum_out_b;
			sum_a -= sum_out_a;

			stack_start = (stack_ptr + div - r);
			if (stack_start >= div)
			{
				stack_start -= div;
			}

			stack_pix_ptr = (unsigned char*)(stack_data_ptr + stack_start);

			sum_out_r -= *(stack_pix_ptr + R);
			sum_out_g -= *(stack_pix_ptr + G);
			sum_out_b -= *(stack_pix_ptr + B);
			sum_out_a -= *(stack_pix_ptr + A);

			if (yp < hm)
			{
				src_pix_ptr += stride;
				++yp;
			}

			*(stack_pix_ptr + R) = *(src_pix_ptr + R);
			*(stack_pix_ptr + G) = *(src_pix_ptr + G);
			*(stack_pix_ptr + B) = *(src_pix_ptr + B);
			*(stack_pix_ptr + A) = *(src_pix_ptr + A);

			sum_in_r += *(stack_pix_ptr + R);
			sum_in_g += *(stack_pix_ptr + G);
			sum_in_b += *(stack_pix_ptr + B);
			sum_in_a += *(stack_pix_ptr + A);

			sum_r += sum_in_r;
			sum_g += sum_in_g;
			sum_b += sum_in_b;
			sum_a += sum_in_a;

			if (++stack_ptr >= div)
			{
				stack_ptr = 0;
			}

			stack_pix_ptr = (unsigned char*)(stack_data_ptr + stack_ptr);

			sum_out_r += *(stack_pix_ptr + R);
			sum_out_g += *(stack_pix_ptr + G);
			sum_out_b += *(stack_pix_ptr + B);
			sum_out_a += *(stack_pix_ptr + A);

			sum_in_r -= *(stack_pix_ptr + R);
			sum_in_g -= *(stack_pix_ptr + G);
			sum_in_b -= *(stack_pix_ptr + B);
			sum_in_a -= *(stack_pix_ptr + A);
		} while (++y < h);
	} while (++x < w);

	delete lpStack;
	lpStack = NULL;
}

void layout(const SDL_Rect &rc1, SDL_Rect &rc2, LayoutFunc horizontal, LayoutFunc vertical)
{
	horizontal(rc1, rc2);
	vertical(rc1, rc2);
}

void round(SDL_Surface *sfWindow, const SDL_Rect &rcOrg, Uint32 color, int radius)
{
	// 1.画圆
	const int R = radius * 2;
	SDL_Rect rcFix = { rcOrg.x, rcOrg.y, max(R, rcOrg.w), max(R, rcOrg.h) };
	SDL_Surface *sf = SDL_CreateRGBSurface(0, R, R, 32, 0, 0, 0, 0);

	// 拷贝4个角
	SDL_Rect rcCorner1 = {rcOrg.x, rcOrg.y, radius, radius};
	SDL_Rect rcDst1 = {0, 0, radius, radius};
	SDL_BlitSurface(sfWindow, &rcCorner1, sf, &rcDst1);

	SDL_Rect rcCorner2 = {rcOrg.x + rcOrg.w - radius, rcOrg.y, radius, radius};
	SDL_Rect rcDst2 = {radius, 0, radius, radius};
	SDL_BlitSurface(sfWindow, &rcCorner2, sf, &rcDst2);

	SDL_Rect rcCorner3 = {rcOrg.x, rcOrg.y + rcOrg.h - radius, radius, radius};
	SDL_Rect rcDst3 = {0, radius, radius, radius};
	SDL_BlitSurface(sfWindow, &rcCorner3, sf, &rcDst3);
	SDL_Rect rcCorner4 = {rcOrg.x + rcOrg.w - radius, rcOrg.y + rcOrg.h - radius, radius, radius};
	SDL_Rect rcDst4 = {radius, radius, radius, radius};
	SDL_BlitSurface(sfWindow, &rcCorner4, sf, &rcCorner4);
	
	SDL_LockSurface(sf);
	Uint32 *pix = (Uint32*)sf->pixels;
	for (int y = -radius; y <= radius; y++)
	{
		for (int x = -radius; x <= radius; x++)
		{
			if ((x * x) + (y * y) <= (radius * radius))
			{
				pix[(y+radius) * sf->w + (x+radius)] = color;
			}
		}
	}
	SDL_UnlockSurface(sf);

	// 2.切圆为4等份，拷贝4个圆角
	SDL_Rect rcCircle1 = {0, 0, radius, radius};
	SDL_Rect rcCDst1 = {rcFix.x, rcFix.y, radius, radius};
	SDL_BlitSurface(sf, &rcCircle1, sfWindow, &rcCDst1);

	SDL_Rect rcCircle2 = {radius, 0, radius, radius};
	SDL_Rect rcCDst2 = {rcFix.x + rcFix.w - radius, rcFix.y, radius, radius};
	SDL_BlitSurface(sf, &rcCircle2, sfWindow, &rcCDst2);
	
	
	SDL_Rect rcCircle3 = {0, radius, radius, radius};
	SDL_Rect rcCDst3 = {rcFix.x, rcFix.y + rcFix.h - radius, radius, radius};
	SDL_BlitSurface(sf, &rcCircle3, sfWindow, &rcCDst3);
	
	SDL_Rect rcCircle4 = {radius, radius, radius, radius};
	SDL_Rect rcCDst4 = {rcFix.x + rcFix.w - radius, rcFix.y + rcFix.h - radius, radius, radius};
	SDL_BlitSurface(sf, &rcCircle4, sfWindow, &rcCDst4);

	SDL_FreeSurface(sf);

	// 3.填充圆角以外的部分
	SDL_Rect rcFill = {rcFix.x + radius, rcFix.y, rcFix.w - R, rcFix.h};
	SDL_FillRect(sfWindow, &rcFill, color);
	rcFill = {rcFix.x, rcFix.y + radius, radius, rcFix.h - R};
	SDL_FillRect(sfWindow, &rcFill, color);
	rcFill = {rcFix.x + rcFix.w - radius, rcFix.y + radius, radius, rcFix.h - R};
	SDL_FillRect(sfWindow, &rcFill, color);
}


bool TextRenderInfo::operator==(const TextRenderInfo & other)
{
	if (&other == this)
		return true;

	return other.hash == hash;
}

void fontStyleHash(const std::string & txt, const FontStyle & style, MD5_HASH& hash)
{
	Md5Context cxt;
	Md5Initialise(&cxt);
	Md5Update(&cxt, (void*)txt.c_str(), txt.length());
	Md5Update(&cxt, (void*)style.name, strlen(style.name));
	Md5Update(&cxt, (void*)&style.size, sizeof(style.size));
	Md5Update(&cxt, (void*)&style.style, sizeof(style.style));
	Md5Update(&cxt, (void*)&style.outline, sizeof(style.outline));
	Md5Update(&cxt, (void*)&style.bg, sizeof(style.bg));
	Md5Update(&cxt, (void*)&style.fg, sizeof(style.fg));
	Md5Finalise(&cxt, &hash);
}

TextRenderInfo::TextRenderInfo(const std::string & txt, const FontStyle & style) :width(0), height(0), texture(nullptr)
{
	MD5_HASH md5;
	fontStyleHash(txt, style, md5);
	for (int i = 0; i < MD5_HASH_SIZE; ++i)
		hash.push_back(md5.bytes[i]);
}