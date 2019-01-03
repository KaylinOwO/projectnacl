#pragma once
#include "SDK.h"
#include "Vector2.h"

typedef unsigned long HFont;
typedef unsigned long Texture;

enum FontFlags
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

class CFonts
{
public:
	void Initialize();
	void Reload();

	HFont arial;
	HFont segoe;
	HFont verdana_bold;
	HFont verdana;
	HFont calibri_light;
	HFont calibri_light_small;
	HFont calibri;
	HFont cambria;

	HFont esp;
};
extern CFonts gFonts;

class CTextures
{
public:
	void Initialize();
	void Reload();

	Texture team_blue, team_red;
	Texture dark_gray;
};
extern CTextures gTex;


struct Vertex_t
{
	Vector2	m_Position;
	Vector2	m_TexCoord;

	Vertex_t() {}
	static inline Vertex_t FromVec3(const Vector &pos, const Vector2 &coord = Vector2(0, 0))
	{
		return Vertex_t(Vector2(pos.x, pos.y), coord);
	}
	Vertex_t(const Vector2 &pos, const Vector2 &coord = Vector2(0, 0))
	{
		m_Position = pos;
		m_TexCoord = coord;
	}
	void Init(const Vector2 &pos, const Vector2 &coord = Vector2(0, 0))
	{
		m_Position = pos;
		m_TexCoord = coord;
	}
};

typedef unsigned char byte;
class SColor
{
public:
	byte rgba[4];

	inline byte& operator[](int i)
	{
		return rgba[i];
	}
	inline bool operator==(SColor& other)
	{
		return *(size_t*)&rgba == *(size_t*)&other.rgba;
	}
	inline bool operator!=(SColor& other)
	{
		return *(size_t*)&rgba != *(size_t*)&other.rgba;
	}

	SColor(byte red, byte green, byte blue, byte alpha = 255)
	{
		rgba[0] = red, rgba[1] = green, rgba[2] = blue, rgba[3] = alpha;
	}
	SColor(byte bright, byte alpha = 255)
	{
		rgba[0] = bright, rgba[1] = bright, rgba[2] = bright, rgba[3] = alpha;
	}
	SColor() {}
};

SColor hsv2rgb(float hue, float saturation, float brightness, int alpha = 255);