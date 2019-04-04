#include "SDK.h"
#include "Util.h"

int CBaseEntity::GetHealth() 
{
	DYNVAR_RETURN(int, this, "DT_BasePlayer", "m_iHealth");
}

int CBaseEntity::GetTeamNum()
{
	DYNVAR_RETURN(int, this, "DT_BaseEntity", "m_iTeamNum");
}

int CBaseEntity::GetFlags()
{
	DYNVAR_RETURN(int, this, "DT_BasePlayer", "m_fFlags");
}

BYTE CBaseEntity::GetLifeState()
{
	DYNVAR_RETURN(BYTE, this, "DT_BasePlayer", "m_lifeState");
}

int CBaseEntity::GetClassNum()
{
	DYNVAR_RETURN(int, this, "DT_TFPlayer", "m_PlayerClass", "m_iClass");
}

char* CBaseEntity::szGetClass()
{
	DYNVAR(iClass, int, "DT_TFPlayer", "m_PlayerClass", "m_iClass");

	switch (iClass.GetValue(this))
	{
	case TF2_Scout:
		return "Scout";
	case TF2_Soldier:
		return "Soldier";
	case TF2_Pyro:
		return "Pyro";
	case TF2_Demoman:
		return "Demoman";
	case TF2_Heavy:
		return "Heavy";
	case TF2_Engineer:
		return "Engineer";
	case TF2_Medic:
		return "Medic";
	case TF2_Sniper:
		return "Sniper";
	case TF2_Spy:
		return "Spy";
	default:
		return "Unknown class";
	}

	return "Unknown class"; //Just in case
}

int CBaseEntity::GetCond()
{
	DYNVAR_RETURN(int, this, "DT_TFPlayer", "m_Shared", "m_nPlayerCond");
}

CBaseCombatWeapon* CBaseEntity::GetActiveWeapon()
{
	DYNVAR(pHandle, DWORD, "DT_BaseCombatCharacter", "m_hActiveWeapon");
	return (CBaseCombatWeapon *)gInts.EntList->GetClientEntityFromHandle(pHandle.GetValue(this));
}

CBaseCombatWeapon* CBaseEntity::GetActiveWeaponOther()
{
	DYNVAR(pHandle, HANDLE, "DT_BaseCombatCharacter", "m_hActiveWeapon");
	return (CBaseCombatWeapon *)gInts.EntList2->GetClientEntityFromHandle(pHandle.GetValue(this));
}


Vector CBaseEntity::GetCollideableMins()
{
	DYNVAR_RETURN(Vector, this, "DT_BaseEntity", "m_Collision", "m_vecMins");
}

Vector CBaseEntity::GetCollideableMaxs()
{
	DYNVAR_RETURN(Vector, this, "DT_BaseEntity", "m_Collision", "m_vecMaxs");
}

Vector CBaseEntity::GetEyePosition()
{
	DYNVAR_RETURN(Vector, this, "DT_BasePlayer", "localdata", "m_vecViewOffset[0]") + this->GetAbsOrigin();
}

Vector CBaseEntity::GetEyeAngles()
{
	DYNVAR_RETURN(Vector, this, "DT_TFPlayer", "tfnonlocaldata", "m_angEyeAngles[0]");
}

Vector CBaseEntity::GetAbsEyePosition()
{
	DYNVAR_RETURN(Vector, this, "DT_BasePlayer", "localdata", "m_vecViewOffset[0]");
}

Vector CBaseEntity::GetHitboxPosition(int iHitbox)
{
	DWORD* model = GetModel();
	if (!model)
		return Vector();

	studiohdr_t* hdr = gInts.ModelInfo->GetStudiomodel(model);
	if (!hdr)
		return Vector();

	matrix3x4 matrix[128];
	if (!SetupBones(matrix, 128, 0x100, gInts.globals->curtime))
		return Vector();

	mstudiohitboxset_t* set = hdr->GetHitboxSet(GetHitboxSet());
	if (!set)
		return Vector();

	mstudiobbox_t* box = set->pHitbox(iHitbox);
	if (!box)
		return Vector();

	Vector center = (box->bbmin + box->bbmax) * 0.5f;

	Vector vHitbox;

	Util->VectorTransform(center, matrix[box->bone], vHitbox);

	return vHitbox;
}