#pragma once
#include <Windows.h>

class Window;

class ScreenCapture {
private:
	Window* window;
	HDC hdcSource;

	HWND hwndSource;

public:
	HDC hdcWindow;
private:

public:
	ScreenCapture(Window* window, HWND source);
	~ScreenCapture();
	bool drawScreencapture();
};
