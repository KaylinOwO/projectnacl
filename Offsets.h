#pragma once
#include <Windows.h>
#include "SDK.h"
#include "Controls.h"

#define CHECKPTR(Pointer) (gOffsets.SafetyCheck(Pointer))
class COffsets
{
public:
	int iKeyEventOffset = 20, iCreateMoveOffset = 21, iPaintTraverseOffset = 41;

	void GetOffsets();

	DWORD LoadFromBuffer;
	DWORD KeyValues;

	void SafetyCheck(void* Pointer)
	{
		if (Pointer)
			return;

		exit(EXIT_FAILURE);
	}
	inline void SafetyCheck(DWORD Pointer)
	{
		return SafetyCheck((void*)Pointer);
	}
};
extern COffsets gOffsets;