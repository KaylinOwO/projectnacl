#pragma once
//===================================================================================
#include "SDK.h"
#include "Panels.h"

#define RED(COLORCODE)	((int) ( COLORCODE >> 24) )
#define BLUE(COLORCODE)	((int) ( COLORCODE >> 8 ) & 0xFF )
#define GREEN(COLORCODE)	((int) ( COLORCODE >> 16 ) & 0xFF )
#define ALPHA(COLORCODE)	((int) COLORCODE & 0xFF )
#define COLORCODE(r,g,b,a)((DWORD)((((r)&0xff)<<24)|(((g)&0xff)<<16)|(((b)&0xff)<<8)|((a)&0xff)))
//===================================================================================
class CDrawManager
{
public:
	void Initialize( );
	void Reload();
	void InitializeFonts();
	void ReloadFonts();
	void DrawString( int x, int y, Color clrColor, const wchar_t *pszText);
	void DrawString( int x, int y, Color clrColor, const char *pszText, ... );
	byte GetESPHeight( );
	void DrawLine(int x0, int y0, int x1, int y1, Color clrColor);
	void DrawLineEx(int x, int y, int w, int h, Color clrColor);
	void DrawBox( Vector vOrigin, int r, int g, int b, int alpha, int box_width, int radius );
	void DrawRect( int x, int y, int w, int h, Color clrColor);
	void OutlineRect( int x, int y, int w, int h, Color clrColor);
	bool WorldToScreen( Vector &vOrigin, Vector &vScreen );
	void DrawStringCenter(int x, int y, Color clrColor, const char* szText, ...);
	void DrawBoxOnEntity(CBaseEntity* entity, Color color);
	Color MenuColor(CBaseEntity* pPlayer);
	Color GetPlayerColor(CBaseEntity* pPlayer);
private:
	unsigned long m_Font;
};
//===================================================================================
extern CDrawManager gDrawManager;
//===================================================================================