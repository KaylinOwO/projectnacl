#include "Radar.h"


void CRadar::DrawRadarPoint(Vector vOriginX, Vector vOriginY, QAngle qAngle, CBaseEntity *pBaseEntity, Color dwTeamColor)
{
	CBaseEntity* pLocalEntity = GetBaseEntity(me);

	if (pLocalEntity == NULL)
		return;

	float flDeltaX = vOriginX.x - vOriginY.x;
	float flDeltaY = vOriginX.y - vOriginY.y;

	float flAngle = qAngle.y;

	float flYaw = (flAngle)* (3.14159265358979323846 / 180.0);
	float flMainViewAngles_CosYaw = cos(flYaw);
	float flMainViewAngles_SinYaw = sin(flYaw);

	// rotate
	float x = flDeltaY * (-flMainViewAngles_CosYaw) + flDeltaX * flMainViewAngles_SinYaw;
	float y = flDeltaX * (-flMainViewAngles_CosYaw) - flDeltaY * flMainViewAngles_SinYaw;

	float flRange = 2000;

	if (fabs(x) > flRange || fabs(y) > flRange)
	{
		// clipping
		if (y > x)
		{
			if (y > -x)
			{
				x = flRange * x / y;
				y = flRange;
			}
			else
			{
				y = -flRange * y / x;
				x = -flRange;
			}
		}
		else
		{
			if (y > -x)
			{
				y = flRange * y / x;
				x = flRange;
			}
			else
			{
				x = -flRange * x / y;
				y = -flRange;
			}
		}
	}

	int	iScreenX = gCvars.radar_posx + int(x / flRange * gCvars.radar_width);
	int iScreenY = gCvars.radar_posy + int(y / flRange * gCvars.radar_width);

	gDrawManager.DrawRect(iScreenX - 3, iScreenY - 3, 7, 7, Color(0, 0, 0, 255));
	gDrawManager.DrawRect(iScreenX - 2, iScreenY - 2, 5, 5, dwTeamColor);

}
//===================================================================================
void CRadar::DrawRadarBack()
{
	if (gCvars.radar_enabled && gCvars.esp_active)
	{
		int lmao1;
		int lmao2;
		//int iCenterX = m_flx;
		//int iCenterY = m_fly;
		int iCenterX = gCvars.radar_posx;
		int iCenterY = gCvars.radar_posy;
		int iSize = gCvars.radar_width;

		gDrawManager.DrawRect(iCenterX - iSize, iCenterY - iSize, 2 * iSize + 2, 2 * iSize + 2, Color(30, 30, 30, 128));
		iSize -= 10;

		gDrawManager.DrawRect(iCenterX, iCenterY - iSize, 1, 2 * iSize, Color(255, 120, 0, 255));
		gDrawManager.DrawRect(iCenterX - iSize, iCenterY, 2 * iSize, 1, Color(255, 120, 0, 255));
	}
}


CRadar gRadar;