#pragma once
#include "Draw utils.h"
#include <Windows.h>
#include <vector>
using namespace std;

#define CONTROL_WIDTH 100
#define SPACING 10

void InitTextures();

class Dialog
{
public:
	int x = 0, y = 0, w = 200, h = 200;
	void(*Draw)(void* data, size_t Index);
	void* data = nullptr;

	Dialog() {}
	Dialog(void(*DrawFunction)(void* data, size_t Index), int W = 200, int H = 200)
	{
		Draw = DrawFunction;
		w = W, h = H;
	}
};

enum e_cflags
{
	null,
	nodraw = (1 << 0),
	noinput = (1 << 1),
	scale_width = (1 << 2),
	scale_height = (1 << 3)
};

enum class e_control
{
	null,
	menu,
	tabgroup,
	tab,
	groupbox,
	checkbox,
	slider,
	listbox,
	colorpicker,
	keybind,
	custompanel,
	space
};

class BaseControl
{
protected:
	int w = 0, h = 0;
	vector<BaseControl*> children;

public:
	const char* name = 0;
	e_control type = e_control::null;
	int flags = null;
	int x = 0, y = 0;
	bool* visible = nullptr;

	void SetX(int X) { x = X; }
	void SetY(int Y) { y = Y; }
	void SetPos(int X, int Y) { x = X, y = Y; }
	void SetPos(POINT pos) { x = pos.x, y = pos.y; }
	POINT GetPos() { return{ x, y }; }

	virtual int GetWidth() { return w; }
	virtual int GetHeight() { return h; }

	inline void SetWidth(int W)
	{
		if (flags & scale_width)
			w = W;
	}
	inline void SetHeight(int H)
	{
		if (flags & scale_height)
			h = H;
	}
	inline void SetScale(int W, int H)
	{
		if (flags & scale_width)
			w = W;
		if (flags & scale_height)
			h = H;
	}

	inline void SetChildren(const vector<BaseControl*>& Children) { children = Children; }
	inline vector<BaseControl*> GetChildren() { return children; }

	virtual int Draw(bool mouseOver = false) { return NULL; }
	virtual void HandleInput() {}

	// - Automatically handles input and drawing
	void RunControl(int Index = 0);
};

class Space : public BaseControl
{
public:
	// - Amount: Extra spacing, zero is fine.
	Space(int Amount = 0)
	{
		type = e_control::space;
		h = Amount;
		flags = scale_width | scale_height | noinput;
	}
};

#define GetTabHeight 30

class Tab : public BaseControl
{
public:
	bool enabled = false;

	int Draw(bool mouseOver);
	void HandleInput() {}

	Tab(const char* Name, const vector<BaseControl*>& Children, int W = NULL)
	{
		type = e_control::tab;
		name = Name, children = Children, w = W, h = GetTabHeight;
		flags = scale_width;
	}
};

class TabGroup : public BaseControl
{
	vector<Tab*> tabs;
public:
	int spacing = 5;
	Tab* active = nullptr;

	void AddTab(Tab* tab)
	{
		if (!tabs.size())
			active = tab;
		tabs.push_back(tab);
	}

	int Count()
	{
		return tabs.size();
	}

	Tab* operator[](int i) const
	{
		return tabs[i];
	}

	int GetHeight()
	{
		return tabs.size() > 0 ? (tabs.size() * GetTabHeight) + (spacing * (tabs.size() - 1)) : 0;
	}

	int Draw(bool mouseOver);
	void HandleInput();

	TabGroup(const vector<Tab*>& Tabs, int W = 200, int Spacing = 5)
	{
		type = e_control::tabgroup;
		tabs = Tabs, w = W, h = tabs.size() * GetTabHeight + Spacing * (tabs.size() - 1), spacing = Spacing;
		active = tabs[0];
		flags = scale_width;
	}

	TabGroup() { w = 200, flags = scale_width; }
};

class Groupbox : public BaseControl
{

public:
	int Draw(bool mouseOver);
	void HandleInput();

	int GetHeight();

	// Leaving the default height parameter will auto-fit all children controls
	Groupbox(const char* Name, const vector<BaseControl*>& Children, int W, int H = NULL)
	{
		type = e_control::groupbox;
		name = Name, children = Children, w = W, h = H;
		flags = scale_width;
	}
};

class Checkbox : public BaseControl
{
public:
	bool value;

	int Draw(bool mouseOver = false);
	void HandleInput();

	bool QuickReturn(bool Value, int X, int Y, int W = -1);

	Checkbox() {}
	Checkbox(const char* Name, bool Value = false, int W = 100, int X = 0, int Y = 0)
	{
		type = e_control::slider;
		name = Name, value = Value, x = X, y = Y;
		flags = scale_width;
		w = W, h = 13;
	}
};

class DrawPanel : public BaseControl
{
public:
	void(*draw)(int x, int y, int w, int h);

	int Draw(bool mouseOver = false);
	void HandleInput() {};

	DrawPanel(const char* Name, void DrawFunction(int x, int y, int w, int h), int W = 200, int H = 200, int X = 0, int Y = 0)
	{
		type = e_control::custompanel, flags = scale_width | scale_height | noinput;
		name = Name, draw = DrawFunction, x = X, y = Y;
		w = W, h = H;
	}
};

class Slider : public BaseControl
{
public:
	int value = 0, min = 0, max = 0, step = 0;

	int Draw(bool mouseOver = false);
	void HandleInput();

	Slider(const char* Name, int Value, int Min, int Max, int Step, int W = 100, int X = 0, int Y = 0)
	{
		type = e_control::slider, flags = scale_width;
		name = Name, value = Value, x = X, y = Y;
		min = Min, max = Max, step = Step;
		h = 19, w = W;
	}
};

class Listbox : public BaseControl
{
public:
	size_t value = 0;
	vector<string> list = {};

	int Draw(bool mouseOver = false);
	void HandleInput();

	Listbox(const char* Name, vector<string> List, int Value = 0, int W = 100, int X = 0, int Y = 0)
	{
		type = e_control::listbox, flags = scale_width;
		name = Name, value = Value, list = List, x = X, y = Y;
		h = 15 + 15, w = W;
	}
};

class ColorPicker : public BaseControl
{
public:
	SColor color;
	SColor cDef;
	bool bDef = true;

	int Draw(bool mouseOver = false);
	void HandleInput();

	ColorPicker(const char* Name, SColor defaultColor, int W = 100, int X = 0, int Y = 0)
	{
		type = e_control::colorpicker, flags = scale_width;
		name = Name, color = cDef = defaultColor, x = X, y = Y;
		h = 20, w = W;
	}
};

#include "KeyEnum.h"
enum class e_kbmode : byte
{
	disabled,
	always,
	hold,
	toggle,
};
class KeyBind : public BaseControl
{
	bool keyHeld = false;
	bool toggled = false;
public:
	e_kbmode mode = e_kbmode::always;

	byte key = NULL;
	bool UpdateKeys();
	bool KeyDown();

	int Draw(bool mouseOver = false);
	void HandleInput();

	KeyBind(const char* Name, byte VirtualKey, e_kbmode Mode = e_kbmode::disabled, int W = 100, int X = 0, int Y = 0)
	{
		type = e_control::keybind, flags = scale_width;
		name = Name, key = VirtualKey, mode = Mode, x = X, y = Y;
		h = 15 + 15, w = W;
	}
};