#include "Offsets.h"

COffsets gOffsets;
void COffsets::GetOffsets()
{
	// Add signatures here
	CHECKPTR(gOffsets.KeyValues = gSignatures.GetEngineSignature("FF 15 ? ? ? ? 83 C4 08 89 06 8B C6"));
	CHECKPTR(gOffsets.LoadFromBuffer = gSignatures.GetEngineSignature("55 8B EC 83 EC 38 53 8B 5D 0C"));

	gOffsets.KeyValues -= 0x42;
}
