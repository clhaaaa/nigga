#pragma once
#include "..\Utils\Color.h"
#include "..\Utils\Utils.h"

enum EFontFlags
{
	FONTFLAG_NONE,
	FONTFLAG_ITALIC = 0x001,
	FONTFLAG_UNDERLINE = 0x002,
	FONTFLAG_STRIKEOUT = 0x004,
	FONTFLAG_SYMBOL = 0x008,
	FONTFLAG_ANTIALIAS = 0x010,
	FONTFLAG_GAUSSIANBLUR = 0x020,
	FONTFLAG_ROTARY = 0x040,
	FONTFLAG_DROPSHADOW = 0x080,
	FONTFLAG_ADDITIVE = 0x100,
	FONTFLAG_OUTLINE = 0x200,
	FONTFLAG_CUSTOM = 0x400,
	FONTFLAG_BITMAP = 0x800,
};


struct Vertex_t
{
	Vector2D	mPosition;
	Vector2D	m_TexCoord;

	Vertex_t() {}
	Vertex_t(const Vector2D &pos, const Vector2D &coord = Vector2D(0, 0))
	{
		mPosition = pos;
		m_TexCoord = coord;
	}
	void Init(const Vector2D &pos, const Vector2D &coord = Vector2D(0, 0))
	{
		mPosition = pos;
		m_TexCoord = coord;
	}
};

typedef  Vertex_t FontVertex_t;

class ISurface
{
public:
	void UnLockCursor()
	{
		typedef void(__thiscall* Fn)(void*);
		Utils::GetVFunc< Fn >(this, 66)(this);
	}

	void DrawSetColor(Color col)
	{
		typedef void(__thiscall* Fn)(PVOID, Color);
		return Utils::GetVFunc< Fn >(this, 14)(this, col);
	}

	void DrawSetColor(int r, int g, int b, int a)
	{
		typedef void(__thiscall* Fn)(PVOID, int, int, int, int);
		Utils::GetVFunc< Fn >(this, 15)(this, r, g, b, a);
	}

	void DrawFilledRect(int x0, int y0, int x1, int y1)
	{
		typedef void(__thiscall* Fn)(PVOID, int, int, int, int);
		Utils::GetVFunc< Fn >(this, 16)(this, x0, y0, x1, y1);
	}

	void DrawOutlinedRect(int x0, int y0, int x1, int y1)
	{
		typedef void(__thiscall* Fn)(PVOID, int, int, int, int);
		Utils::GetVFunc< Fn >(this, 18)(this, x0, y0, x1, y1);
	}

	void DrawLine(int x0, int y0, int x1, int y1)
	{
		typedef void(__thiscall* Fn)(PVOID, int, int, int, int);
		Utils::GetVFunc< Fn >(this, 19)(this, x0, y0, x1, y1);
	}

	void DrawPolyLine(int *x, int *y, int count)
	{
		typedef void(__thiscall* Fn)(PVOID, int *, int *, int);
		Utils::GetVFunc< Fn >(this, 20)(this, x, y, count);
	}

	void DrawSetTextFont(unsigned long font)
	{
		typedef void(__thiscall* Fn)(PVOID, unsigned long);
		Utils::GetVFunc< Fn >(this, 23)(this, font);
	}

	void DrawSetTextColor(int r, int g, int b, int a)
	{
		typedef void(__thiscall* Fn)(PVOID, int, int, int, int);
		Utils::GetVFunc< Fn >(this, 25)(this, r, g, b, a);
	}

	void DrawSetTextColor(Color col)
	{
		typedef void(__thiscall* Fn)(PVOID, Color);
		return Utils::GetVFunc< Fn >(this, 24)(this, col);
	}

	void DrawSetTextPos(int x, int y)
	{
		typedef void(__thiscall* Fn)(PVOID, int, int);
		Utils::GetVFunc< Fn >(this, 26)(this, x, y);
	}

	void DrawPrintText(const wchar_t* text, int textLen)
	{
		typedef void(__thiscall* Fn)(PVOID, const wchar_t*, int, int);
		Utils::GetVFunc< Fn >(this, 28)(this, text, textLen, 0);
	}

	void DrawSetTexture(int textureID)
	{
		typedef void(__thiscall* Fn)(PVOID, int);
		return Utils::GetVFunc< Fn >(this, 38)(this, textureID);
	}

	void DrawSetTextureRGBA(int textureID, unsigned char  const* colors, int w, int h)
	{
		typedef void(__thiscall* Fn)(PVOID, int, unsigned char  const*, int, int);
		return Utils::GetVFunc< Fn >(this, 37)(this, textureID, colors, w, h);
	}

	int CreateNewTextureID(bool procedural)
	{
		typedef int(__thiscall* Fn)(PVOID, bool);
		return Utils::GetVFunc< Fn >(this, 43)(this, procedural);
	}

	void DrawTexturedPolygon(int vtxCount, FontVertex_t *vtx, bool bClipVertices = true)
	{
		typedef void(__thiscall* Fn)(PVOID, int, FontVertex_t*, bool);
		return Utils::GetVFunc< Fn >(this, 106)(this, vtxCount, vtx, bClipVertices);
	}

	unsigned long FontCreate()
	{
		typedef unsigned int(__thiscall* Fn)(PVOID);
		return Utils::GetVFunc< Fn >(this, 71)(this);
	}

	void SetFontGlyphSet(unsigned long font, const char* windowsFontName, int tall, int weight, int blur, int scanlines, int flags)
	{
		typedef void(__thiscall* Fn)(PVOID, unsigned long, const char*, int, int, int, int, int, int, int);
		Utils::GetVFunc< Fn >(this, 72)(this, font, windowsFontName, tall, weight, blur, scanlines, flags, 0, 0);
	}

	void GetTextSize(DWORD font, const wchar_t *text, int &wide, int &tall)
	{
		typedef void(__thiscall* Fn)(void*, DWORD, const wchar_t*, int&, int&);
		return Utils::GetVFunc<Fn>(this, 79)(this, font, text, wide, tall);
	}

	void DrawOutlinedCircle(int x, int y, int r, int seg)
	{
		typedef void(__thiscall* Fn)(PVOID, int, int, int, int);
		return Utils::GetVFunc< Fn >(this, 103)(this, x, y, r, seg);
	}

	void DrawFilledRectFade(int x0, int y0, int x1, int y1, size_t alpha0, size_t alpha1, bool horizontal) 
	{
		typedef void(__thiscall* Fn)(PVOID, int, int, int, int, size_t, size_t, bool);
		Utils::GetVFunc< Fn >(this, 123)(this, x0, y0, x1, y1, alpha0, alpha1, horizontal);
	}

	void SurfaceGetCursorPos(int &x, int &y)
	{
		typedef void(__thiscall* Fn)(PVOID, int&, int&);
		return Utils::GetVFunc< Fn >(this, 66)(this, x, y);
	}
};

extern ISurface* g_pSurface;

class IRender // im nigger
{
public:

	void RoundedFilledRect(int x, int y, int width, int height, float radius, Color col) { // UC https://www.unknowncheats.me/forum/1498179-post4.html
		constexpr int quality = 24;

		static Vertex_t verts[quality];

		Vector2D add = { 0, 0 };

		for (int i = 0; i < quality; i++) {
			float angle = (static_cast <float> (i) / -quality) * 6.28f - (6.28f / 16.f);

			verts[i].mPosition.x = radius + x + add.x + (radius * sin(angle));
			verts[i].mPosition.y = height - radius + y + add.y + (radius * cos(angle));

			if (i == 4) {
				add.y = -height + (radius * 2);
			}
			else if (i == 10) {
				add.x = width - (radius * 2);
			}
			else if (i == 16) {
				add.y = 0;
			}
			else if (i == 22) {
				add.x = 0;
			}
		}
		g_pSurface->DrawSetColor(col.red, col.green, col.blue, col.alpha);
		g_pSurface->DrawTexturedPolygon(quality, verts);
	}

	void Polygon(int count, Vertex_t* vt, Color color)
	{
		g_pSurface->DrawSetColor(color.red, color.green, color.blue, color.alpha);
		g_pSurface->DrawTexturedPolygon(count, vt);
	}

	void FilledRect(int x, int y, int w, int h, Color color)
	{
		g_pSurface->DrawSetColor(color.red, color.green, color.blue, color.alpha);
		g_pSurface->DrawFilledRect(x, y, x + w, y + h);
	}

	void OutlinedRect(int x, int y, int w, int h, Color color)
	{
		g_pSurface->DrawSetColor(color.red, color.green, color.blue, color.alpha);
		g_pSurface->DrawOutlinedRect(x, y, x + w, y + h);
	}

	void GradientRect(int x, int y, int w, int h, Color topclr, Color btmclr) // dosnt wurk idky imm retarded
	{
		g_pSurface->DrawSetColor(topclr);
		g_pSurface->DrawFilledRectFade(x, y, x + w, y + h, 255, 255, false);
		g_pSurface->DrawSetColor(btmclr);
		g_pSurface->DrawFilledRectFade(x, y, x + w, y + h, 0, 255, false);
	}

	void AutoGradientRect(int x, int y, int w, int h, Color color) // i took this from sumwhere bec im 2 dum
	{
		Color Top = Color(color.red, color.green, color.blue, color.alpha);
		Color Bottom = Color(color.red - 17.f, color.green - 17.f, color.blue - 17.f, color.alpha);

		FilledRect(x, y, w, h, Top);

		for (int i = 0; i < h; i++)
		{
			float fi = i, fh = h;
			float a = fi / fh;
			DWORD ia = a * 255;
			FilledRect(x, y + i, w, 1, Color(Bottom.red, Bottom.green, Bottom.blue, ia));
		}
	}

	void OutlinedVecRect(int x, int y, int x2, int y2, Color color)
	{
		g_pSurface->DrawSetColor(color.red, color.green, color.blue, color.alpha);
		g_pSurface->DrawLine(x, y, x2, y);
		g_pSurface->DrawLine(x2, y, x2, y2);
		g_pSurface->DrawLine(x2, y2, x, y2);
		g_pSurface->DrawLine(x, y2, x, y);
	}

	void Line(int x, int y, int x2, int y2, Color color)
	{
		g_pSurface->DrawSetColor(color.red, color.green, color.blue, color.alpha);
		g_pSurface->DrawLine(x, y, x2, y2);
	}

	RECT GetTextSizeRect(DWORD font, const char* text)
	{
		size_t cSize = strlen(text) + 1;
		wchar_t* WideBuffer = new wchar_t[cSize];
		mbstowcs(WideBuffer, text, cSize);

		RECT rect; int x, y;
		g_pSurface->GetTextSize(font, WideBuffer, x, y);
		rect.left = x; rect.bottom = y;
		rect.right = x;
		delete[] WideBuffer; //maybe???
		return rect;
	}

	void DrawT(int X, int Y, Color Color, int Font, bool Center, const char* _Input, ...)
	{
		size_t cSize = strlen(_Input) + 1;
		wchar_t* WideBuffer = new wchar_t[cSize];
		mbstowcs(WideBuffer, _Input, cSize);

		int Width = 0, Height = 0;

		if (Center)
			g_pSurface->GetTextSize(Font, WideBuffer, Width, Height);

		g_pSurface->DrawSetTextColor(Color.red, Color.green, Color.blue, Color.alpha);
		g_pSurface->DrawSetTextFont(Font);
		g_pSurface->DrawSetTextPos(X - (Width / 2), Y);
		g_pSurface->DrawPrintText(WideBuffer, wcslen(WideBuffer));
		delete[] WideBuffer; // im retarded does this work????
	}

	Vector2D GetMousePosition() // bolbi ware
	{
		POINT mousePosition;
		GetCursorPos(&mousePosition);
		ScreenToClient(FindWindow(0, "Counter-Strike: Global Offensive"), &mousePosition);
		return { static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y) };
	}

	bool MouseInRegion(int x, int y, int x2, int y2) {
		if (GetMousePosition().x > x && GetMousePosition().y > y && GetMousePosition().x < x2 + x && GetMousePosition().y < y2 + y)
			return true;
		return false;
	}
};

extern IRender* g_pRender;