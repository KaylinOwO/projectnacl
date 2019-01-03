#include "SDK.h"

void CPlayerVariables::Initialize()
{
	gNetVars = new CNetVars();

	m_iHealth = gNetVars->get_offset("DT_BasePlayer", "m_iHealth");;
	m_fFlags = gNetVars->get_offset("DT_BasePlayer", "m_fFlags");
	m_iTeamNum = gNetVars->get_offset("DT_BaseEntity", "m_iTeamNum");
	m_Collision = gNetVars->get_offset("DT_BaseEntity", "m_Collision");
	m_lifeState = gNetVars->get_offset("DT_BasePlayer", "m_lifeState");
}