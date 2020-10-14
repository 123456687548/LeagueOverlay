#pragma once
#include <Windows.h>

class Window;

class ScreenCapture {
private:
	Window* window;
	HDC hdcSource;
	HDC hdcWindow;

	HWND hwndSource;

public:

private:

public:
	ScreenCapture(Window* window, HWND source);
	~ScreenCapture();
	bool drawScreencapture();
};
