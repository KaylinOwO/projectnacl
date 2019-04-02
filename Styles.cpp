#include "Styles.h"
#include "CDraw.h"
#include "CDrawManager.h"

int DefaultStyle::ControlPanel(int x, int y, int w, int h, const char* name)
{
	gDraw.OutlineRect(x, y, w, h, SColor(50, 50, 55));

	int txtMargin = 0;

	int textW = 0, textH = 15;
	if (name)
		gInts.Surface->GetTextSize(gFonts.calibri_light_small, ToWchar(name).c_str(), textW, textH);
	gDraw.DrawRect(x + 1, y + 1, w - 2, textH + txtMargin, SColor(25));
	if (name)
		gDraw.DrawString(x + ((w / 2) - (textW / 2)), y, SColor(150), name, gFonts.calibri_light_small);

	gDraw.DrawLine(x, y + textH + txtMargin, x + w, y + textH + txtMargin, SColor(50, 50, 55));
	x += 1, y += textH + txtMargin + 1, w -= 2, h -= textH + txtMargin + 2;

	gDraw.DrawRect(x, y, w, h, SColor(30, 30, 33));

	return textH + txtMargin + 1;
}

int DefaultStyle::DialogButton(int x, int y, int w, const char* text, bool mouseOver)
{
	gDraw.DrawRect(x, y, w, 15, SColor(42, 42, 48));
	gDraw.OutlineRect(x, y, w, 15, SColor(60));

	gDraw.DrawString(x + 3, y, mouseOver ? SColor(90, 150, 225) : SColor(125, 125, 130), text, gFonts.verdana_bold);

	return 15;
}

void DefaultStyle::Dialog(int x, int y, int w, int h)
{
	gDraw.OutlineRect(x, y, w, h, SColor(58, 58, 70));
	gDraw.DrawRect(x + 1, y + 1, w - 2, h - 2, SColor(36, 36, 40));
}

#define TOPBAR 25
int DefaultStyle::TopBar(int x, int y, int w, const char* title)
{
	// Dark topbar
	gDraw.DrawRect(x, y, w, TOPBAR, SColor(32, 34, 37));

	if (title)
		gDraw.DrawString(x + 10, y, SColor(120), title, gFonts.segoe);

	return TOPBAR;
}

void WindowStyle::WaterMark(const char* name, int x, int y, bool background)
{
	static float hue = 0;

	static SColor red(255, 105, 180);
	static SColor light(200);

	int tw = 0, th = 0;
	gInts.Surface->GetTextSize(gFonts.cambria, ToWchar(name).c_str(), tw, th);

	if (background)
	{
		static Vertex_t verts[4];
		verts[0] = Vertex_t(Vector2(x + 10, y - 10));
		verts[1] = Vertex_t(Vector2(x + tw + 10, y - 10));
		verts[2] = Vertex_t(Vector2(x + tw + 10, y + th + 20));
		verts[3] = Vertex_t(Vector2(x + 10, y + th + 20));

		for (int i = 0; i < 4; i++)
		{
			Vector2 newVec = verts[i].m_Position;
			newVec = rotate_vec2(newVec, Vector2(x + tw / 2, y + th / 2), DEG2RAD(5));

			verts[i].m_Position = newVec;
		}
		gDraw.DrawPolygon(4, verts, gTex.dark_gray);
	}

	gDraw.DrawString(x + 14, y + 4, light, name, gFonts.cambria);

	// Replaced colors.red with color
	gDraw.DrawRect(x, y, tw, 5, red);
	gDraw.DrawRect(x, y + 5, 5, th / 2, red);

	gDraw.DrawRect(x + 31, y + 14 + th, tw, 5, red);
	gDraw.DrawRect(x + 31 + tw - 5, y + 14 + th / 2, 5, th / 2, red);
}

void WindowStyle::About(int x, int y, int w, int h)
{
	int CenterX = x + (w / 2);

	int dcHeight, dcWidth, iHeight, iWidth;
	gInts.Surface->GetTextSize(gFonts.arial, L"NaCl is an open-source project meant to benefit the b1g pasters out there\n", iWidth, iHeight);
	gDraw.DrawString(CenterX - (iWidth / 2), y + 5, SColor(160), L"NaCl is an open-source TF2 project meant to benefit the b1g pasters out there\n");

	gInts.Surface->GetTextSize(gFonts.calibri_light, L"Project NaCl", dcWidth, dcHeight);
	gDrawManager.DrawString(CenterX - (dcWidth / 2), y + 110 + iHeight, Color::Rainbow(), L"Project NaCl");
}