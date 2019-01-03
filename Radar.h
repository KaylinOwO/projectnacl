#pragma once

class Vector;
class QAngle;
class C_BaseEntity;

#include <Windows.h>


#include "SDK.h"

#include "CDrawManager.h"

#include "QAngle.h"


class CRadar
{

public:
	float m_flx = 101;
	float m_fly = 101;
	float m_flw = 100;

	void DrawRadarPoint(Vector vOriginX, Vector vOriginY, QAngle qAngle, CBaseEntity * pBaseEntity, Color dwTeamColor);
	void DrawRadarBack();
};
extern CRadar gRadar;