#include "ScreenCapture.h"
#include "Window.h"

#include <cstdio>

ScreenCapture::ScreenCapture(Window* window, HWND source) : window(window), hwndSource(source) {
	hdcSource = GetDC(source);
	hdcWindow = GetDC(window->getHWND());
}

ScreenCapture::~ScreenCapture() {
	ReleaseDC(hwndSource, hdcSource);
	ReleaseDC(window->getHWND(), hdcWindow);
}

bool ScreenCapture::drawScreencapture() {
	if (!BitBlt(hdcWindow, 0, 0, window->getWidth(), window->getHeight(), hdcSource, 1655, 814, SRCCOPY)) {
		printf("BitBlt has failed %#X", GetLastError());
		return false;
	}

	return true;
}
