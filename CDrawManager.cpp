#include "CDrawManager.h"
//===================================================================================
CDrawManager gDrawManager;

void CDrawManager::Initialize() // Create font IDs here
{
	if (gInts.Surface == NULL)
		return;

	InitializeFonts();

	Reload();
}
void CDrawManager::Reload() // Create (or reload) your font styles
{
	if (gInts.Surface == NULL)
		return;

	gInts.Engine->GetScreenSize(gScreenSize.iScreenWidth, gScreenSize.iScreenHeight);

	ReloadFonts();
}

void CDrawManager::InitializeFonts()
{
	m_Font = gInts.Surface->CreateFont();
}

void CDrawManager::ReloadFonts()
{
	gInts.Surface->SetFontGlyphSet(m_Font, "Verdana", 12, 500, 0, 0, 0x200);
}

void CDrawManager::DrawStringCenter(int x, int y, Color clrColor, const char* szText, ...)
{
	if (!szText)
		return;

	va_list va_alist = nullptr;
	int Wide = 0, Tall = 0;
	char szBuffer[256] = { '\0' };
	wchar_t szString[128] = { '\0' };

	va_start(va_alist, szText);
	vsprintf_s(szBuffer, szText, va_alist);
	va_end(va_alist);

	MultiByteToWideChar(CP_UTF8, 0, szBuffer, -1, szString, 128);


	gInts.Surface->GetTextSize(m_Font, szString, Wide, Tall);
	x -= Wide / 2;


	gInts.Surface->DrawSetTextPos(x, y);
	gInts.Surface->DrawSetTextFont(m_Font);
	gInts.Surface->DrawSetTextColor(clrColor.r(), clrColor.g(), clrColor.b(), clrColor.a());
	gInts.Surface->DrawPrintText(szString, strlen(szBuffer));
}

void CDrawManager::DrawBoxOnEntity(CBaseEntity* entity, Color color)
{
	const matrix3x4& vMatrix = entity->GetRgflCoordinateFrame();

	Vector vMin = entity->GetCollideableMins();
	Vector vMax = entity->GetCollideableMaxs();

	Vector vPointList[] = {
		Vector(vMin.x, vMin.y, vMin.z),
		Vector(vMin.x, vMax.y, vMin.z),
		Vector(vMax.x, vMax.y, vMin.z),
		Vector(vMax.x, vMin.y, vMin.z),
		Vector(vMax.x, vMax.y, vMax.z),
		Vector(vMin.x, vMax.y, vMax.z),
		Vector(vMin.x, vMin.y, vMax.z),
		Vector(vMax.x, vMin.y, vMax.z)
	};
	Vector vTransformed[8];

	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 3; j++)
			vTransformed[i][j] = vPointList[i].Dot((Vector&)vMatrix[j]) + vMatrix[j][3];

	Vector flb, brt, blb, frt, frb, brb, blt, flt;

	if (!gDrawManager.WorldToScreen(vTransformed[3], flb) ||
		!gDrawManager.WorldToScreen(vTransformed[0], blb) ||
		!gDrawManager.WorldToScreen(vTransformed[2], frb) ||
		!gDrawManager.WorldToScreen(vTransformed[6], blt) ||
		!gDrawManager.WorldToScreen(vTransformed[5], brt) ||
		!gDrawManager.WorldToScreen(vTransformed[4], frt) ||
		!gDrawManager.WorldToScreen(vTransformed[1], brb) ||
		!gDrawManager.WorldToScreen(vTransformed[7], flt))
		return;

	Vector arr[] = { flb, brt, blb, frt, frb, brb, blt, flt };

	float left = flb.x;
	float top = flb.y;
	float right = flb.x;
	float bottom = flb.y;

	for (int i = 0; i < 8; i++)
	{
		if (left > arr[i].x)
			left = arr[i].x;
		if (top < arr[i].y)
			top = arr[i].y;
		if (right < arr[i].x)
			right = arr[i].x;
		if (bottom > arr[i].y)
			bottom = arr[i].y;
	}

	float x = left;
	float y = bottom;


	float w = right - left;
	float h = top - bottom;

	x += ((right - left) / 8); //pseudo fix for those THICC boxes
	w -= ((right - left) / 8) * 2;

	gDrawManager.OutlineRect(x, y, w, h, color);
	gDrawManager.OutlineRect(x + 1, y + 1, w - 2, h - 2, color);
}

Color CDrawManager::MenuColor(CBaseEntity* pPlayer)
{
	if (gCvars.menu_color == 0)
		return GetPlayerColor(pPlayer);

	if (gCvars.menu_color == 1)
		return Color(0, 153, 255, 255);

	if (gCvars.menu_color == 2)
		return Color(255, 20, 20, 255);

	if (gCvars.menu_color == 3)
		return Color(255, 255, 0, 255);

    if (gCvars.menu_color == 4)
		return Color(0, 255, 0, 255);

	return Color(0, 0, 0, 0); //no reason for this to be here, i just wanna look smart
}

Color CDrawManager::GetPlayerColor(CBaseEntity* pPlayer)
{
	if (gCvars.esp_aimbothighlights && pPlayer->GetIndex() == gCvars.iAimbotIndex)
		return Color::Green();
	else if (!gCvars.PlayerMode[pPlayer->GetIndex()])
		return Color(0, 255, 255, 255);
	else if (gCvars.PlayerMode[pPlayer->GetIndex()] == 2)
		return Color::Yellow();
	else
	{

		switch (pPlayer->GetTeamNum())
		{
		case 2: //RED
			return Color(255, 20, 20, 255);
		case 3: //BLU
			return Color(0, 153, 255, 255);
		default:
			return Color(0, 153, 255, 255);
		}
	}

	return Color(0, 0, 0, 0); //no reason for this to be here, i just wanna look smart
}
//===================================================================================
void CDrawManager::DrawString( int x, int y, Color clrColor, const wchar_t *pszText)
{
	if( pszText == NULL )
		return;

	gInts.Surface->DrawSetTextPos( x, y );
	gInts.Surface->DrawSetTextFont( m_Font );
	gInts.Surface->DrawSetTextColor( clrColor.r(), clrColor.g(), clrColor.b(), clrColor.a() );
	gInts.Surface->DrawPrintText( pszText, wcslen( pszText ) );
}
//===================================================================================
void CDrawManager::DrawString( int x, int y, Color clrColor, const char *pszText, ... )
{
	if( pszText == NULL )
		return;

	va_list va_alist;
	char szBuffer[1024] = { '\0' };
	wchar_t szString[1024] = { '\0' };

	va_start( va_alist, pszText );
	vsprintf_s( szBuffer, pszText, va_alist );
	va_end( va_alist );

	wsprintfW( szString, L"%S", szBuffer );

	gInts.Surface->DrawSetTextPos( x, y );
	gInts.Surface->DrawSetTextFont( m_Font );
	gInts.Surface->DrawSetTextColor( clrColor.r(), clrColor.g(), clrColor.b(), clrColor.a() );
	gInts.Surface->DrawPrintText( szString, wcslen( szString ) );
}
//===================================================================================
byte CDrawManager::GetESPHeight( )
{
	return 12;
}
//===================================================================================
void CDrawManager::DrawLine(int x, int y, int x1, int y1, Color clrColor)
{
	gInts.Surface->DrawSetColor(clrColor.r(), clrColor.g(), clrColor.b(), clrColor.a());
	gInts.Surface->DrawLine(x, y, x1, y1);
}
//===================================================================================
void CDrawManager::DrawLineEx(int x, int y, int w, int h, Color clrColor)
{
	gInts.Surface->DrawSetColor(clrColor.r(), clrColor.g(), clrColor.b(), clrColor.a());
	gInts.Surface->DrawLine(x, y, x + w, y + h);
}
//===================================================================================
void CDrawManager::DrawRect( int x, int y, int w, int h, Color clrColor)
{
	gInts.Surface->DrawSetColor( clrColor.r(), clrColor.g(), clrColor.b(), clrColor.a() );
	gInts.Surface->DrawFilledRect( x, y, x + w, y + h );
}
//===================================================================================
void CDrawManager::OutlineRect( int x, int y, int w, int h, Color clrColor)
{
	gInts.Surface->DrawSetColor( clrColor.r(), clrColor.g(), clrColor.b(), clrColor.a() );
	gInts.Surface->DrawOutlinedRect( x, y, x + w, y + h );
}
//===================================================================================
void CDrawManager::DrawBox( Vector vOrigin, int r, int g, int b, int alpha, int box_width, int radius )
{
	Vector vScreen;

	if( !WorldToScreen( vOrigin, vScreen ) )
		return;

	int radius2 = radius<<1;

	OutlineRect( vScreen.x - radius + box_width, vScreen.y - radius + box_width, radius2 - box_width, radius2 - box_width, Color::Black() );
	OutlineRect( vScreen.x - radius - 1, vScreen.y - radius - 1, radius2 + ( box_width + 2 ), radius2 + ( box_width + 2 ), Color::Black() );
	DrawRect( vScreen.x - radius + box_width, vScreen.y - radius, radius2 - box_width, box_width, Color( r, g, b, alpha ));
	DrawRect( vScreen.x - radius, vScreen.y + radius, radius2, box_width, Color( r, g, b, alpha ));
	DrawRect( vScreen.x - radius, vScreen.y - radius, box_width, radius2, Color( r, g, b, alpha ));
	DrawRect( vScreen.x + radius, vScreen.y - radius, box_width, radius2 + box_width, Color( r, g, b, alpha ) );
}
//===================================================================================
bool CDrawManager::WorldToScreen( Vector &vOrigin, Vector &vScreen )
{
	const matrix3x4& worldToScreen = gInts.Engine->WorldToScreenMatrix(); //Grab the world to screen matrix from CEngineClient::WorldToScreenMatrix

	float w = worldToScreen[3][0] * vOrigin[0] + worldToScreen[3][1] * vOrigin[1] + worldToScreen[3][2] * vOrigin[2] + worldToScreen[3][3]; //Calculate the angle in compareson to the player's camera.
	vScreen.z = 0; //Screen doesn't have a 3rd dimension.

	if( w > 0.001 ) //If the object is within view.
	{
		float fl1DBw = 1 / w; //Divide 1 by the angle.
		vScreen.x = (gScreenSize.iScreenWidth / 2) + ( 0.5 * ((worldToScreen[0][0] * vOrigin[0] + worldToScreen[0][1] * vOrigin[1] + worldToScreen[0][2] * vOrigin[2] + worldToScreen[0][3]) * fl1DBw) * gScreenSize.iScreenWidth + 0.5); //Get the X dimension and push it in to the Vector.
		vScreen.y = (gScreenSize.iScreenHeight / 2) - ( 0.5 * ((worldToScreen[1][0] * vOrigin[0] + worldToScreen[1][1] * vOrigin[1] + worldToScreen[1][2] * vOrigin[2] + worldToScreen[1][3]) * fl1DBw) * gScreenSize.iScreenHeight + 0.5); //Get the Y dimension and push it in to the Vector.
		return true;
	}

	return false;
}