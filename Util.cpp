#include "Util.h"

CUtil* Util;

float CUtil::flGetDistance(Vector vOrigin, Vector vLocalOrigin)
{
	Vector vDelta = vOrigin - vLocalOrigin;

	float m_fDistance = sqrt(vDelta.Length());

	if (m_fDistance < 1.0f)
		return 1.0f;

	return m_fDistance;
}

void CUtil::VectorTransform(const Vector& vSome, const matrix3x4& vMatrix, Vector& vOut)
{
	for (auto i = 0; i < 3; i++)
		vOut[i] = vSome.Dot((Vector&)vMatrix[i]) + vMatrix[i][3];
}

Vector CUtil::AngleVector(Vector meme) {
	auto sy = sin(meme.y / 180.f * static_cast<float>(PI));
	auto cy = cos(meme.y / 180.f * static_cast<float>(PI));
	auto sp = sin(meme.x / 180.f * static_cast<float>(PI));
	auto cp = cos(meme.x / 180.f* static_cast<float>(PI));
	return Vector(cp * cy, cp * sy, -sp);
}

float CUtil::DistPointToLine(Vector point, Vector origin, Vector direction) {
	auto point_direction = point - origin;

	auto temporary_offset = point_direction.dot(direction) / (direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);
	if (temporary_offset < 0.000001f) {
		return FLT_MAX;
	}

	auto perpendicular_point = origin + (direction * temporary_offset);

	return (point - perpendicular_point).length();
}

bool CUtil::IsVisible(void* pLocal, void* pEntity, Vector vStart, Vector vEnd)
{
	trace_t Trace;
	Ray_t Ray;			 // the future of variable naming
	CTraceFilter Filter;

	Filter.pSkip = pLocal;

	Ray.Init(vStart, vEnd);

	gInts.EngineTrace->TraceRay(Ray, MASK_SHOT, &Filter, &Trace);

	return (Trace.m_pEnt == pEntity || Trace.fraction == 1.0);
}

bool CUtil::IsKeyPressed(int i)
{
	switch (i)
	{
	case 0: //None
		return true;
	case 1: //Mouse 1
		return GetAsyncKeyState(VK_LBUTTON);
	case 2:
		return GetAsyncKeyState(VK_RBUTTON);
	case 3:										  //Mouses 1-5
		return GetAsyncKeyState(VK_MBUTTON);
	case 4:
		return GetAsyncKeyState(VK_XBUTTON1);
	case 5:
		return GetAsyncKeyState(VK_XBUTTON2);
	case 6: //Shift
		return GetAsyncKeyState(VK_SHIFT);
	case 7: //Alt
		return GetAsyncKeyState(VK_MENU);
	case 8: //F
		return GetAsyncKeyState(0x46);
	default:
		return false;
	}

	return false;
}

bool CUtil::IsKeyPressedMisc(int i)
{
	switch (i)
	{
	case 0: //None
		return true;
	case 1:  //Mouses 1-5
		return GetAsyncKeyState(VK_MBUTTON);
	case 2:
		return GetAsyncKeyState(VK_XBUTTON1);
	case 3:
		return GetAsyncKeyState(VK_XBUTTON2);
	case 4: //Shift
		return GetAsyncKeyState(VK_SHIFT);
	case 5: //Alt
		return GetAsyncKeyState(VK_MENU);
	case 6: //F
		return GetAsyncKeyState(0x46);
	case 7: //E
		return GetAsyncKeyState(0x45);
	default:
		return false;
	}

	return false;
}

void CUtil::SilentMovementFix(CUserCmd *pUserCmd, Vector angles)
{
	Vector vecSilent(pUserCmd->forwardmove, pUserCmd->sidemove, pUserCmd->upmove);
	float flSpeed = sqrt(vecSilent.x * vecSilent.x + vecSilent.y * vecSilent.y);
	Vector angMove;
	VectorAngles(vecSilent, angMove);
	float flYaw = DEG2RAD(angles.y - pUserCmd->viewangles.y + angMove.y);
	pUserCmd->forwardmove = cos(flYaw) * flSpeed;
	pUserCmd->sidemove = sin(flYaw) * flSpeed;
	pUserCmd->viewangles = angles;
}

bool CUtil::IsVisible2(Vector& vecAbsStart, Vector& vecAbsEnd, CBaseEntity* pLocal, CBaseEntity* pEntity)
{
	trace_t tr;
	Ray_t ray;
	CTraceFilter filter;

	ray.Init(vecAbsStart, vecAbsEnd);
	gInts.EngineTrace->TraceRay(ray, 0x200400B, &filter, &tr);

	if (tr.m_pEnt == NULL)
		if (tr.fraction == 1.0f)
			return true;

	return (tr.m_pEnt->GetIndex() == pEntity->GetIndex());
}


float CUtil::getVectorLength(Vector vector)
{
	return (float)sqrt(pow(vector.x, 2) + pow(vector.y, 2) + pow(vector.z, 2));
}
float CUtil::getVectorDistance(Vector vec1, Vector vec2)
{
	return getVectorLength(vec2 - vec1);
}

DWORD CUtil::FindSignature(const char* szModuleName, char* szPattern)
{
	HMODULE hModule = GetModuleHandleA(szModuleName);
	PIMAGE_DOS_HEADER pDOSHeader = (PIMAGE_DOS_HEADER)hModule;
	PIMAGE_NT_HEADERS pNTHeaders = (PIMAGE_NT_HEADERS)(((DWORD)hModule) + pDOSHeader->e_lfanew);

	return FindPattern(((DWORD)hModule) + pNTHeaders->OptionalHeader.BaseOfCode, ((DWORD)hModule) + pNTHeaders->OptionalHeader.SizeOfCode, szPattern);
}
#define IsInRange(x, a, b) (x >= a && x <= b)
#define GetBits(x) (IsInRange(x, '0', '9') ? (x - '0') : ((x&(~0x20)) - 'A' + 0xA))
#define GetByte(x) (GetBits(x[0]) << 4 | GetBits(x[1]))
DWORD CUtil::FindPattern(DWORD dwAddress, DWORD dwLength, const char* szPattern)
{
	if (!dwAddress || !dwLength || !szPattern)
		return 0;

	const char* pat = szPattern;
	DWORD firstMatch = NULL;

	for (DWORD pCur = dwAddress; pCur < dwLength; pCur++)
	{
		if (!*pat)
			return firstMatch;

		if (*(PBYTE)pat == '\?' || *(BYTE*)pCur == GetByte(pat))
		{
			if (!firstMatch)
				firstMatch = pCur;

			if (!pat[2])
				return firstMatch;

			if (*(PWORD)pat == '\?\?' || *(PBYTE)pat != '\?')
				pat += 3;

			else pat += 2;
		}
		else
		{
			pat = szPattern;
			firstMatch = 0;
		}
	}

	return 0;
}
Vector CUtil::EstimateAbsVelocity(CBaseEntity *ent)
{
	typedef void(__thiscall * EstimateAbsVelocityFn)(CBaseEntity *, Vector &);



	static DWORD dwFn = FindSignature("client.dll", "E8 ? ? ? ? F3 0F 10 4D ? 8D 85 ? ? ? ? F3 0F 10 45 ? F3 0F 59 C9 56 F3 0F 59 C0 F3 0F 58 C8 0F 2F 0D ? ? ? ? 76 07") + 0x1;

	static DWORD dwEstimate = ((*(PDWORD)(dwFn)) + dwFn + 4);

	EstimateAbsVelocityFn vel = (EstimateAbsVelocityFn)dwEstimate;

	Vector v;

	vel(ent, v);

	return v;
}



float CUtil::RandFloatRange(float min, float max)
{
	return (min + 1) + (((float)rand()) / (float)RAND_MAX) * (max - (min + 1));
}


bool CUtil::IsOtherSlot(CBaseEntity* pLocal, CBaseCombatWeapon* pWep)
{
	if (pWep->GetSlot() == 2)
		return true;

	if (pWep->GetSlot() == 3)
		return true;

	return false;
}

bool CUtil::IsHeadshotWeapon(CBaseEntity* pLocal, CBaseCombatWeapon* pWep)
{
	if (pWep->GetSlot() == 0 && pLocal->GetClassNum() == TF2_Sniper)
		if (pWep->GetItemDefinitionIndex() != WPN_Huntsman && pWep->GetItemDefinitionIndex() != WPN_CompoundBow)
			return true;

	if (pWep->GetItemDefinitionIndex() == WPN_Ambassador || pWep->GetItemDefinitionIndex() == WPN_FestiveAmbassador)
		return true;

	return false;
}

PVOID CUtil::InitKeyValue() //Credits f1ssion
{
	typedef PDWORD(__cdecl* Init_t)(int);
	static DWORD dwInitLocation = gSignatures.GetClientSignature("E8 ? ? ? ? 83 C4 14 85 C0 74 10 68") + 0x1;
	static DWORD dwInit = ((*(PDWORD)(dwInitLocation)) + dwInitLocation + 4);
	static Init_t InitKeyValues = (Init_t)dwInit;
	return InitKeyValues(32);
}
