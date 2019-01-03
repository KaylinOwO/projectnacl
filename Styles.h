#pragma once

class WindowStyle
{
public:
	// Draws a framed control area for dialogs
	virtual void Dialog(int x, int y, int w, int h) = 0;
	// Draws a framed control area for small panels within another control area
	// - Returns the height of the namebar
	virtual int ControlPanel(int x, int y, int w, int h, const char* name = 0) = 0;
	// Draws a button made to open dialogs
	// - Returns the height of the box
	virtual int DialogButton(int x, int y, int w, const char* text = 0, bool mouseOver = false) = 0;

	// Draws a window top bar
	// - Returns the height of the bar
	virtual int TopBar(int x, int y, int w, const char* title = 0) = 0;

	static void WaterMark(const char* name, int x, int y, bool background = true);
	static void About(int x, int y, int w, int h);
};

class DefaultStyle : public WindowStyle
{
public:
	void Dialog(int x, int y, int w, int h);
	int ControlPanel(int x, int y, int w, int h, const char* name = 0);
	int DialogButton(int x, int y, int w, const char* text, bool mouseOver = false);

	int TopBar(int x, int y, int w, const char* title = 0);
};