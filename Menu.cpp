#include "Menu.h"
#include "CDraw.h"
#include "Aimbot.h"
#include "Triggerbot.h"
#include "ESP.h"
#include "Misc.h"
#include "HvH.h"

Menu gMenu;
CKey gKey;

// - Feel free to use whatever default width you want
#define GROUP_WIDTH 180
#define TAB_WIDTH 150
#define MENU_TOPBAR 30

POINT pos = { 100, 100 }, scale = { 640, 480 };

void Menu::CreateGUI()
{
	/*What you're about to see may cause severe eye bleeding*/
	/*YOU HAVE BEEN WARNED*/

	if (GAME_TF2)
	{
		Tabs.AddTab(
			new Tab("Aimbot", {
				new Groupbox("General",{
					&gAim.enabled,
					&gAim.key,
					&gAim.hitbox,
					&gAim.fov,
					&gAim.smooth,
					&gAim.silent,
					&gAim.Autoshoot,
					&gAim.waitforcharge,
					&gAim.zoomedonly,
					&gAim.aimatteammates,
				}, GROUP_WIDTH),

				new Groupbox("Ignore List",{
					&gAim.ignorecloaked,
					&gAim.ignoredisguised,
				}, GROUP_WIDTH),
			})
			);

		Tabs.AddTab(
			new Tab("Triggerbot", {
				new Groupbox("Hitscan",{
					&gTrigger.enabled,
					&gTrigger.key,
					&gTrigger.headonly,
					&gTrigger.delay,
					&gTrigger.cloaked,
				}, GROUP_WIDTH),

				new Groupbox("Misc",{
					&gTrigger.backstab,
					&gTrigger.stickydetonate,
					&gTrigger.airblast,
					&gTrigger.airblastrage,
				}, GROUP_WIDTH),
			})
			);

		Tabs.AddTab(
			new Tab("Visuals", {
				new Groupbox("ESP",{
					&gESP.enabled,
					&gESP.enemyonly,
					&gESP.box,
					&gESP.name,
					&gESP.tfclass,
					&gESP.health,
					&gESP.bones,
					&gESP.viewlines,
					&gESP.player_enabled,
					&gESP.chamsmat,
					&gESP.player_mat,
					&gESP.ignorecloaked,
				}, GROUP_WIDTH),
				new Groupbox("Misc",{
					&gESP.nozoom,
					&gESP.noscope,
					&gESP.thirdperson,
					&gESP.visualize_backtrack,
					&gESP.hands_enabled,
					&gESP.hands_chams,
					&gESP.hand_mat_color,
				}, GROUP_WIDTH),

				new Groupbox("World Modulation",{
					&gESP.world_enabled,
					&gESP.world_clr,
					&gESP.sky_clr,
				}, GROUP_WIDTH)
			})
			);

		Tabs.AddTab(
			new Tab("Misc", {
				new Groupbox("General",
				{
					&gMisc.bhop,
					&gMisc.astrafer,
					&gMisc.nspam,
					&gMisc.tauntslide,
					&gMisc.speedcrouch,
					&gMisc.nopush,
					&gMisc.purebypass,
					&gMisc.cscreenshots,
				}, GROUP_WIDTH),

				new Groupbox("Backtrack",
				{
					&gMisc.backtrack,
					&gMisc.backtrack_ticks,
				}, GROUP_WIDTH),

				new Groupbox("Warp",
				{
					&gMisc.warp,
					&gMisc.doubletap,
					&gMisc.warp_key,
					&gMisc.warp_charge_key,
					&gMisc.warp_value,
				}, GROUP_WIDTH),

				new Groupbox("Fakelag",
				{
					&gMisc.flag,
					&gMisc.flagamount,
				}, GROUP_WIDTH),

			})
			);

		Tabs.AddTab(
			new Tab("HvH", {
				new Groupbox("General",
				{
					&gHvH.resolver,
					&gHvH.resolverkey,
					&gHvH.antiaim,
					&gHvH.pitch,
					&gHvH.yaw,
				}, GROUP_WIDTH),

			})
			);

		Tabs.AddTab(
			new Tab("About", {
				new DrawPanel("About Section", gMenu.style->About, scale.x - TAB_WIDTH - 32, scale.y - MENU_TOPBAR)
			})
			);
	}
	else
	{
		if (GAME_CSS)
		{
			Tabs.AddTab(
				new Tab("Aimbot", {
					new Groupbox("General",{
						&gAim.enabled,
						&gAim.key,
						&gAim.hitbox,
						&gAim.fov,
						&gAim.smooth,
						&gAim.silent,
						&gAim.Autoshoot,
						&gAim.aimatteammates,
					}, GROUP_WIDTH),

					new Groupbox("Accuracy",{
						&gAim.antirecoil,
						&gAim.autopistol,
					}, GROUP_WIDTH),
				})
				);
		}
		else
		{
			Tabs.AddTab(
				new Tab("Aimbot", {
					new Groupbox("General",{
						&gAim.enabled,
						&gAim.key,
						&gAim.hitbox,
						&gAim.fov,
						&gAim.smooth,
						&gAim.silent,
						&gAim.Autoshoot,
						&gAim.aimatteammates,
					}, GROUP_WIDTH),

					new Groupbox("Accuracy",{
						&gAim.autopistol,
					}, GROUP_WIDTH),
				})
				);
		}

		Tabs.AddTab(
			new Tab("Triggerbot", {
				new Groupbox("Hitscan",{
					&gTrigger.enabled,
					&gTrigger.key,
					&gTrigger.headonly,
					&gTrigger.delay,
				}, GROUP_WIDTH),
			})
			);

		Tabs.AddTab(
			new Tab("Visuals", {
				new Groupbox("ESP",{
					&gESP.enabled,
					&gESP.enemyonly,
					&gESP.box,
					&gESP.name,
					&gESP.health,
					&gESP.viewlines,
					&gESP.player_enabled,
					&gESP.chamsmat,
					&gESP.player_mat,
				}, GROUP_WIDTH),
				new Groupbox("Misc",{
					&gESP.nozoom,
					&gESP.noscope,
					&gESP.thirdperson,
					&gESP.visualize_backtrack,
				}, GROUP_WIDTH),

				new Groupbox("World Modulation",{
					&gESP.world_enabled,
					&gESP.world_clr,
					&gESP.sky_clr,
				}, GROUP_WIDTH)
			})
			);

		Tabs.AddTab(
			new Tab("Misc", {
				new Groupbox("General",
				{
					&gMisc.bhop,
					&gMisc.astrafer,
					&gMisc.cscreenshots,
				}, GROUP_WIDTH),

				new Groupbox("Backtrack",
				{
					&gMisc.backtrack,
					&gMisc.backtrack_ticks,
				}, GROUP_WIDTH),

				new Groupbox("Airstuck",
				{
					&gMisc.airstuck,
					&gMisc.airstuck_key,
				}, GROUP_WIDTH),

				new Groupbox("Time Shift",
				{
					&gMisc.sfreezing,
					&gMisc.sfreezing_key,
					&gMisc.sfreezing_value,
				}, GROUP_WIDTH),

				new Groupbox("Server Lagger",
				{
					&gMisc.serverlag,
					&gMisc.serverlag_key,
					&gMisc.serverlag_value,
				}, GROUP_WIDTH),

				new Groupbox("Fakelag",
				{
					&gMisc.flag,
					&gMisc.flagamount,
				}, GROUP_WIDTH),

			})
			);

		Tabs.AddTab(
			new Tab("HvH", {
				new Groupbox("General",
				{
					&gHvH.resolver,
					&gHvH.resolverkey,
					&gHvH.antiaim,
					&gHvH.pitch,
					&gHvH.yaw,
				}, GROUP_WIDTH),

			})
			);

		Tabs.AddTab(
			new Tab("About", {
				new DrawPanel("About Section", gMenu.style->About, scale.x - TAB_WIDTH - 32, scale.y - MENU_TOPBAR)
			})
			);
	}
}

#define TAB_WIDTH 150
#define MENU_TOPBAR 30

#include "Panels.h"
void Menu::Draw()
{
	if (key == VK_INSERT || key == VK_F11)
		enabled = !enabled;

	if (!enabled)
		return;

	style->WaterMark("Project NaCl", 20, 20);

#pragma region Handle input
	static bool dragging = false;

	if (mb == e_mb::lclick && mouseOver(pos.x, pos.y, scale.x, MENU_TOPBAR))
		dragging = true;
	else if (mb != e_mb::ldown)
		dragging = false;

	if (dragging && focus == 0)
	{
		pos.x += mouse.x - pmouse.x;
		pos.y += mouse.y - pmouse.y;
	}

#pragma endregion

#pragma region Main window
	// Topbar + outline
	int topbar = style->TopBar(pos.x, pos.y, scale.x, "Project NaCl");
	gDraw.OutlineRect(pos.x - 1, pos.y - 1, scale.x + 2, scale.y + topbar + 2, SColor(0));

	if (pos.x < 0)
		pos.x = 0;
	else if (pos.x + scale.x > gScreenSize.iScreenWidth)
		pos.x = gScreenSize.iScreenWidth - scale.x;
	if (pos.y < 0)
		pos.y = 0;
	else if (pos.y + topbar > gScreenSize.iScreenHeight)
		pos.y = gScreenSize.iScreenHeight - topbar;

	// Re-adjust pos to draw below the topbar
	POINT _pos = { pos.x, pos.y + topbar };

	// Tab region
	gDraw.DrawRect(_pos.x, _pos.y, TAB_WIDTH, scale.y, SColor(22, 23, 24));
	gDraw.DrawLine(_pos.x + TAB_WIDTH - 1, _pos.y, _pos.x + TAB_WIDTH - 1, _pos.y + scale.y, SColor(0));
	//gDraw.DrawString(_pos.x + 10, _pos.y + scale.y - 32, SColor(120, 120), "- 0.1 beta - ", gFonts.verdana_bold);
	gDraw.DrawString(_pos.x + 10, _pos.y + scale.y - 18, SColor(120, 120), "Build: " __DATE__, gFonts.verdana_bold);

	Tabs.SetPos(_pos.x, _pos.y + topbar);
	Tabs.SetWidth(TAB_WIDTH);
	Tabs.HandleInput();
	Tabs.Draw(false);

	// Control region
	gDraw.DrawRect(_pos.x + TAB_WIDTH, _pos.y, scale.x - TAB_WIDTH, scale.y, SColor(36, 36, 42));
	// Dividing line
	gDraw.DrawLine(_pos.x, _pos.y, _pos.x + scale.x, _pos.y, SColor(0));

	// Re-adjusting pos and scale again
	_pos.x += TAB_WIDTH + 3;
	POINT _scale = { scale.x - (_pos.x - pos.x), scale.y };

#pragma endregion

#pragma region Controls
	if (Tabs.active)
	{
		int cx = _pos.x + 13, cy = _pos.y + 12;
		int maxWidth = 0;
		vector<BaseControl*> controls = Tabs.active->GetChildren();
		for (size_t i = 0; i < controls.size(); i++)
		{
			if (controls[i]->flags & nodraw)
				continue;

			if (cy + controls[i]->GetHeight() > scale.y + _pos.y - 12)
				cy = _pos.y + 12, cx += 13 + maxWidth + 10, maxWidth = 0;

			if (controls[i]->GetWidth() > maxWidth)
				maxWidth = controls[i]->GetWidth();
			controls[i]->SetPos(cx, cy);

			bool over = mouseOver(cx, cy, controls[i]->GetWidth(), controls[i]->GetHeight());
			bool getInput = !(controls[i]->flags & noinput) && over && !IsDialogOpen();
			if (getInput)
				controls[i]->HandleInput();

			controls[i]->Draw(getInput);

			cy += controls[i]->GetHeight() + SPACING;
		}
	}
#pragma endregion

#pragma region Dialogs
	size_t last = dialogs.size() - 1;
	if (dialogs.size() > 1)
	{
		e_mb new_mb = mb;
		e_mw new_mw = mw;
		POINT new_mouse = mouse, new_pmouse = pmouse;

		// Enforce focus so that only the last dialog gets to use these variables
		mb = e_mb::null, mw = e_mw::null, mouse = pmouse = { 0, 0 };

		for (size_t i = 0; i < last; i++)
		{
			if (dialogs[i] == nullptr)
				continue;

			dialogs[i]->Draw(dialogs[i]->data, i + 1);
		}
		mb = new_mb, mw = new_mw, mouse = new_mouse, pmouse = new_pmouse;
		dialogs[last]->Draw(dialogs[last]->data, last + 1);
	}
	else if (!last)
		dialogs[last]->Draw(dialogs[last]->data, last + 1);

	if (key == VK_ESCAPE && dialogs.size())
		dialogs.pop_back();
#pragma endregion
}

bool Menu::mouseOver(int x, int y, int w, int h)
{
	return mouse.x >= x && mouse.x <= x + w && mouse.y >= y && mouse.y <= y + h;
}

void Menu::GetInput()
{
	int mx = 0, my = 0;
	gInts.Surface->GetCursorPosition(mx, my);

	pmouse = mouse;
	mouse = { mx, my };

	if (GetAsyncKeyState(VK_LBUTTON))
	{
		if (mb == e_mb::lclick || mb == e_mb::ldown)
			mb = e_mb::ldown;
		else
			mb = e_mb::lclick;
	}
	else if (GetAsyncKeyState(VK_RBUTTON))
	{
		if (mb == e_mb::rclick || mb == e_mb::rdown)
			mb = e_mb::rdown;
		else
			mb = e_mb::rclick;
	}
	else
		mb = e_mb::null;
}

void Menu::EndInput()
{
	// Reseting Window message variables so they won't stick
	mw = e_mw::null;
	key = NULL;
}

LRESULT __stdcall Hooked_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_MOUSEWHEEL:
		if ((int)wParam < 0)
			gMenu.mw = e_mw::up;
		else
			gMenu.mw = e_mw::down;
		break;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN: // Allows input to work with alt key
		if (wParam > 255)
			break;
		gMenu.keys[wParam] = true, gMenu.last_key = wParam = gMenu.key = wParam;
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		// Avoid Mouse 3/4/5 here
		if (wParam > 255 && wParam < VK_MBUTTON && wParam > VK_XBUTTON2)
			break;
		gMenu.keys[wParam] = false;
		if (gMenu.last_key == wParam)
			gMenu.last_key = NULL;
		break;
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
		gMenu.keys[VK_MBUTTON] = uMsg == WM_MBUTTONDOWN;
		if (uMsg == WM_MBUTTONDOWN)
			gMenu.last_key = gMenu.key = VK_MBUTTON;
		else if (gMenu.last_key == VK_MBUTTON)
			gMenu.last_key = NULL;
		break;
	}

	if (gMenu.enabled && gMenu.keys[VK_ESCAPE])
		return 0; // Don't let escape key affect the game while using our UI

	return CallWindowProc(gMenu.windowProc, hWnd, uMsg, wParam, lParam);
}

void Menu::OpenDialog(Dialog& dlg)
{
	dialogs.push_back(&dlg);
	focus = dialogs.size();
}
void Menu::CloseDialog(size_t Index)
{
	if (Index == 0)
		return;

	Index--;
	if (Index >= dialogs.size())
		return;

	dialogs.erase(dialogs.begin() + Index);
	focus = dialogs.size();
}