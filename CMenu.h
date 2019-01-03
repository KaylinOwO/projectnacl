#pragma once
#include <Windows.h>

// credits: gir and kalvin for this and source file

struct CMenuItems_t
{
	char szTitle[30];
	float* value;
	float flMin;
	float flMax;
	float flStep;
	bool isClassSwitch;
};

class CCheatMenu
{
public:
	void Render(void);
	void DrawMenu(void);
	int	 AddItem(int nIndex, char szTitle[30], float* value, float flMin, float flMax, float flStep, bool isClassHead);

	//private:
	bool bMenuActive;
	int iMenuIndex;
	int iMenuItems;
	CMenuItems_t pMenu[120];
};

extern CCheatMenu gCheatMenu;