#include "SFreezing.h"
#include "Misc.h"

SequenceFreezing gSequenceFreezing;

void SequenceFreezing::Run(CBaseEntity* pLocal, CUserCmd* pCommand)
{
	if (!gMisc.sfreezing.value)
		return;

	if (Util->IsKeyPressedMisc(gMisc.sfreezing_key.value) //G
												  //  && pLocal->GetLifeState() == LIFE_ALIVE
		&& !gInts.Engine->Con_IsVisible()
		&& gInts.Engine->IsConnected())
		RemoveConds(pLocal, pCommand, (int)gMisc.sfreezing_value.value);
}
void SequenceFreezing::RemoveConds(CBaseEntity * local, CUserCmd * cmd, int value, bool disableattack)
{
	if (local == NULL) return;

	if (local->GetLifeState() != LIFE_ALIVE) return;

	INetChannel* ch = (INetChannel*)gInts.Engine->GetNetChannelInfo();
	int& m_nOutSequenceNr = *(int*)((unsigned)ch + 8);
	m_nOutSequenceNr += value;
}

void SequenceFreezing::RemoveConds(CBaseEntity *local, int value)
{
	RemoveConds(local, new CUserCmd(), value);
}
