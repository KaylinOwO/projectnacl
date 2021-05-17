#include "HvH.h"
#include "Util.h"
#include "Client.h"
#include <chrono>
#include <sstream>

CHvH gHvH;

//I keep this in a seperate file so I don't clutter misc, don't sperg about it, thanks.

void movementfix(CUserCmd* pCmd, Vector vOldAngles, float fOldSidemove, float fOldForward)
{
	int e = rand() % 3;

	Vector curAngs = pCmd->viewangles;
	float fRot = 90;
	//Vector nwANG = Vector(-89, curAngs.y + fRot, 0);
	//pCmd->viewangles = nwANG;

	float deltaView = pCmd->viewangles.y - vOldAngles.y;
	float f1;
	float f2;

	if (vOldAngles.y < 0.f)
		f1 = 360.0f + vOldAngles.y;
	else
		f1 = vOldAngles.y;

	if (pCmd->viewangles.y < 0.0f)
		f2 = 360.0f + pCmd->viewangles.y;
	else
		f2 = pCmd->viewangles.y;

	if (f2 < f1)
		deltaView = abs(f2 - f1);
	else
		deltaView = 360.0f - abs(f1 - f2);
	deltaView = 360.0f - deltaView;

	pCmd->forwardmove = cos(DEG2RAD(deltaView)) * fOldForward + cos(DEG2RAD(deltaView + 90.f)) * fOldSidemove;
	pCmd->sidemove = sin(DEG2RAD(deltaView)) * fOldForward + sin(DEG2RAD(deltaView + 90.f)) * fOldSidemove;
}

float cur_yaw = 0.0f;
float edgeDistance(float edgeRayYaw)
{
	//Main ray tracing area
	trace_t trace;
	Ray_t ray;
	Vector forward;
	float sp, sy, cp, cy;
	sy = sinf(DEG2RAD(edgeRayYaw)); // yaw
	cy = cosf(DEG2RAD(edgeRayYaw));
	sp = sinf(DEG2RAD(0)); // pitch
	cp = cosf(DEG2RAD(0));
	forward.x = cp * cy;
	forward.y = cp * sy;
	forward.z = -sp;
	forward = forward * 300.0f + GetBaseEntity(me)->GetEyePosition();
	ray.Init(GetBaseEntity(me)->GetEyePosition(), forward);
	//trace::g_pFilterNoPlayer to only focus on the enviroment
	gInts.EngineTrace->TraceRay(ray, 0x4200400B, NULL, &trace);
	//Pythagorean theorem to calculate distance
	float edgeDistance = (sqrt(pow(trace.startpos.x - trace.endpos.x, 2) + pow(trace.startpos.y - trace.endpos.y, 2)));
	return edgeDistance;
}
float edgeYaw = 0;
float edgeToEdgeOn = 0;
bool findEdge(float edgeOrigYaw, int pitch_mode)
{
	//distance two vectors and report their combined distances
	int edgeLeftDist = edgeDistance(edgeOrigYaw - 21);
	edgeLeftDist = edgeLeftDist + edgeDistance(edgeOrigYaw - 27);
	int edgeRightDist = edgeDistance(edgeOrigYaw + 21);
	edgeRightDist = edgeRightDist + edgeDistance(edgeOrigYaw + 27);

	//If the distance is too far, then set the distance to max so the angle isnt used
	if (edgeLeftDist >= 260) edgeLeftDist = 999999999;
	if (edgeRightDist >= 260) edgeRightDist = 999999999;

	//If none of the vectors found a wall, then dont edge
	if (edgeLeftDist == edgeRightDist) return false;

	//Depending on the edge, choose a direction to face
	if (edgeRightDist < edgeLeftDist)
	{
		edgeToEdgeOn = 1;
		//Correction for pitches to keep the head behind walls
		if (((int)pitch_mode == 1) || ((int)pitch_mode == 3)) edgeToEdgeOn = 2;
		return true;
	}
	else
	{
		edgeToEdgeOn = 2;
		//Same as above
		if (((int)pitch_mode == 1) | ((int)pitch_mode == 3)) edgeToEdgeOn = 1;
		return true;
	}
}
float useEdge(float edgeViewAngle)
{
	//Var to be disabled when a angle is choosen to prevent the others from conflicting
	bool edgeTest = true;
	if (((edgeViewAngle < -135) || (edgeViewAngle > 135)) && edgeTest == true)
	{
		if (edgeToEdgeOn == 1) edgeYaw = (float)-90;
		if (edgeToEdgeOn == 2) edgeYaw = (float)90;
		edgeTest = false;
	}
	if ((edgeViewAngle >= -135) && (edgeViewAngle < -45) && edgeTest == true)
	{
		if (edgeToEdgeOn == 1) edgeYaw = (float)0;
		if (edgeToEdgeOn == 2) edgeYaw = (float)179;
		edgeTest = false;
	}
	if ((edgeViewAngle >= -45) && (edgeViewAngle < 45) && edgeTest == true)
	{
		if (edgeToEdgeOn == 1) edgeYaw = (float)90;
		if (edgeToEdgeOn == 2) edgeYaw = (float)-90;
		edgeTest = false;
	}
	if ((edgeViewAngle <= 135) && (edgeViewAngle >= 45) && edgeTest == true)
	{
		if (edgeToEdgeOn == 1) edgeYaw = (float)179;
		if (edgeToEdgeOn == 2) edgeYaw = (float)0;
		edgeTest = false;
	}
	//return with the angle choosen
	return edgeYaw;
}


void CHvH::Run(CBaseEntity* pLocal, CUserCmd* pCommand)
{
	CUserCmd* pCmd = pCommand;
	Vector vOldAngles = pCommand->viewangles;
	float fOldForward = pCommand->forwardmove;
	float fOldSidemove = pCommand->sidemove;
	Vector/*&*/ angles = pCommand->viewangles;
	static bool flip = false;
	bool clamp = false;

	//Yea yea, use case instead, I don't care enough to actually change it right now, I'm only doing this update cuz I'm a bored.

	if (!antiaim.value)
		return;

	if (pCommand->buttons & IN_ATTACK)
		return;

	//Pitch Anti-Aims
	if (pitch.value > 0)
	{
		if (pitch.value == 1)//Fake Up
			angles.x = -271.0;
		if (pitch.value == 2)//Up
			angles.x = -89.0f;
		if (pitch.value == 3)//Fake Down
			angles.x = 271.0;
		if (pitch.value == 4)//Down
			angles.x = 89.0f;
	}

	//Yaw Anti-Aims
	if (yaw.value)
	{
		if (yaw.value == 1)//Right
			angles.y -= 90.0f;


		if (yaw.value == 2)//Left
			angles.y += 90.0f;

		if (yaw.value == 3)//Back
			angles.y -= 180;

		if (yaw.value == 4)//Emotion
			angles.y = (float)-89.99985719438652715;

		if (yaw.value == 5)//Random
			angles.y = Util->RandFloatRange(-180.0f, 180.0f);

		if (yaw.value == 6) //Fake Sideways Right
		{
			if (*g.sendpacket)
				angles.y += 90.0f;
			else
				angles.y += -90.0f;
		}

		if (yaw.value == 7) //Fake Sideways Left
		{
			if (*g.sendpacket)
				angles.y += -90.0f;
			else
				angles.y += 90.0f;
		}

		if (yaw.value == 8) //Fake Right
		{
			if (*g.sendpacket) angles.y += -90.0f;
			else angles.y += 0.0f;
		}

		if (yaw.value == 9)//Half Backwards Left
		{
			if (*g.sendpacket)
				angles.y += 135.0f;
			else
				angles.y += -135.0f;
		}

		if (yaw.value == 10)//Half Backwards Right
		{
			if (*g.sendpacket)
				angles.y += -135.0f;
			else
				angles.y += 135.0f;
		}

		if (yaw.value == 11) //Fake Left
		{
			if (*g.sendpacket)
				angles.y += 90.0f;
			else
				angles.y += 0.0f;
		}

		if (yaw.value == 12) //Fake Manual
		{
			{
				static bool right = false, left = false;
				if (GetAsyncKeyState(VK_LEFT) & 1) {
					right = false, left = true;
				}
				else if (GetAsyncKeyState(VK_RIGHT) & 1) {
					right = true, left = false;
				}

				if (left && !right) {
					if (*g.sendpacket) angles.y += -90.0f;
					else angles.y += 90.0f;
				}
				else if (right && !left) {
					if (*g.sendpacket) angles.y += 90.0f;
					else angles.y += -90.0f;
				}
			}
		}
	}

	//	if (clamp) fClampAngle(angles);

	//Util->SilentMovementFix(pCommand, angles);
	pCommand->viewangles = angles;
	movementfix(pCmd, vOldAngles, fOldSidemove, fOldForward);
}