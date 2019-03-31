#pragma once
#include "SDK.h"

class CUtil
{
public:

	void SilentMovementFix(CUserCmd *pUserCmd, Vector angles);

	bool IsVisible2(Vector& vecAbsStart, Vector& vecAbsEnd, CBaseEntity* pLocal, CBaseEntity* pEntity);

	float getVectorLength(Vector vector);
	
	float getVectorDistance(Vector vec1, Vector vec2);

	void VectorTransform(const Vector& vSome, const matrix3x4& vMatrix, Vector& vOut);

	Vector AngleVector(Vector meme);

	float DistPointToLine(Vector point, Vector origin, Vector direction);

	float flGetDistance(Vector vOrigin, Vector vLocalOrigin);

	bool IsVisible(void* pLocal, void* pEntity, Vector vStart, Vector vEnd);

	bool IsKeyPressed(int i);

	bool IsKeyPressedMisc(int i);

	bool IsHeadshotWeapon(CBaseEntity* pLocal, CBaseCombatWeapon* pWep);

	bool IsOtherSlot(CBaseEntity* pLocal, CBaseCombatWeapon* pWep);

	DWORD FindPattern(DWORD dwAddress, DWORD dwLength, const char* szPattern);

	DWORD FindSignature(const char* szModuleName, char* szPattern);

	Vector EstimateAbsVelocity(CBaseEntity *ent);

	float RandFloatRange(float min, float max);

	PVOID InitKeyValue();
};

extern CUtil* Util;

#define VectorMA(v, s, b, o)    ((o)[0] = (v)[0] + (b)[0] * (s), (o)[1] = (v)[1] + (b)[1] * (s), (o)[2] = (v)[2] + (b)[2] * (s))
