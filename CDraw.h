#pragma once
//===================================================================================
#include "SDK.h"
#include "Draw utils.h"

class TextureHolder
{
public:
	TextureHolder() {}
	TextureHolder(const byte* pRawRGBAData, int W, int H)
	{
		m_iTexture = gInts.Surface->CreateNewTextureID(true);
		if (!m_iTexture)
			return;
		gInts.Surface->DrawSetTextureRGBA(m_iTexture, pRawRGBAData, W, H);
		rawData = pRawRGBAData;
		m_iW = W, m_iH = H;
		m_bValid = true;
	}

	bool IsValid() const
	{
		return m_bValid;
	}

	int GetTextureId() const
	{
		return m_iTexture;
	}

	int GetWidth() const
	{
		return m_iW;
	}

	int GetHeight() const
	{
		return m_iH;
	}

	const byte* GetRGBA() const
	{
		return rawData;
	}

	bool Draw(int x, int y, SColor clr = SColor(255), float scale = 1.0)
	{
		if (!gInts.Surface->IsTextureIDValid(m_iTexture))
			return false;
		gInts.Surface->DrawSetColor(clr[0], clr[1], clr[2], clr[3]);
		gInts.Surface->DrawSetTexture(m_iTexture);
		gInts.Surface->DrawTexturedRect(x, y, x + (m_iW * scale), y + (m_iH * scale));
		return true;
	};

protected:
	int m_iTexture;
	int m_iW, m_iH;
	const byte* rawData;
	bool m_bValid;
};

class CBounds
{
public:
	int x = 0, y = 0, w = 0, h = 0;

	CBounds operator+(const CBounds& other)
	{
		CBounds newbounds = *this;
		newbounds.x += other.x, newbounds.y += other.y;
		return newbounds;
	}
	CBounds operator-(const CBounds& other)
	{
		CBounds newbounds = *this;
		newbounds.x -= other.x, newbounds.y -= other.y;
		return newbounds;
	}

	void operator+=(const CBounds& other)
	{
		*this = *this + other;
	}
	void operator-=(const CBounds& other)
	{
		*this = *this - other;
	}

	CBounds(int X, int Y, int W, int H)
	{
		x = X, y = Y, w = W, h = H;
	}
	CBounds() {}
};

class CDraw
{
public:
	void Init();
	void Reload();

	void DrawString(int x, int y, SColor color, string text, HFont font = gFonts.arial);
	void DrawString(int x, int y, SColor color, wstring text, HFont font = gFonts.arial);
	void DrawString(int x, int y, SColor color, const char *pszText, HFont font = gFonts.arial);
	void DrawString(int x, int y, SColor color, const wchar_t *wcText, HFont font = gFonts.arial);

	void DrawLine(int x, int y, int x1, int y1, SColor color);

	byte GetESPHeight();
	void DrawBox(Vector vOrigin, SColor color, int box_width, int radius);
	void DrawRect(int x, int y, int w, int h, SColor color);
	void OutlineRect(int x, int y, int w, int h, SColor color);
	void DrawPolygon(int count, Vertex_t* verts, int texture);

	bool WorldToScreen(Vector &vOrigin, Vector &vScreen);
	SColor TeamColor(int iIndex)
	{
		static SColor colors[] =
		{
			SColor(0), //Dummy
			SColor(160), // 1 Teamone (UNUSED)
			SColor(186, 52, 53), // 2 Red
			SColor(0, 128, 255), // 3 Blu
			SColor(0) // 4 Teamfour (UNUSED) 
		};
		return colors[iIndex];
	}
	SColor LightTeamColor(int iIndex)
	{
		static SColor colors[] =
		{
			SColor(0), //Dummy
			SColor(160), // 1 Teamone (UNUSED)
			SColor(240, 100, 90), // 2 Red
			SColor(100, 150, 240), // 3 Blu
			SColor(0) // 4 Teamfour (UNUSED) 
		};
		return colors[iIndex];
	}
};
extern CDraw gDraw;