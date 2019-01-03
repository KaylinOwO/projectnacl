#pragma once

enum class e_mb
{
	null,
	lclick,
	rclick,
	ldown,
	rdown
};

enum class e_mw
{
	null,
	up,
	down
};

enum class e_focus
{
	null,
	dialog
};

#include "Controls.h"
#include "Styles.h"
class Menu
{
public:
	bool enabled = false;

	bool mouseOver(int x, int y, int w, int h);
	POINT mouse{ 0, 0 }, pmouse{ 0, 0 };

	e_mb mb = e_mb::null;
	e_mw mw = e_mw::null;
	inline int GetFocus() { return focus; }

	void Draw();
	void CreateGUI();

	void OpenDialog(Dialog& dlg);
	void CloseDialog(size_t Index);
	inline bool IsDialogOpen() { return dialogs.size(); }

	void GetInput();
	void EndInput();

	WindowStyle* style = new DefaultStyle();

	WNDPROC windowProc;

	// - Store flags for all keys.
	// - The largest integers can't hold 256 flags, so we use an array
	byte keys[256];

	// - Virtual key of the newest index
	// - Also the index to find that key in keys[]
	byte last_key = NULL;

	// - Indicates a keypress for one frame
	byte key = NULL;

	inline const TabGroup* GetTabs()
	{
		return &Tabs;
	}
private:
	int focus = 0;
	vector<Dialog*> dialogs;

	TabGroup Tabs;
};
extern Menu gMenu;

LRESULT __stdcall Hooked_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);