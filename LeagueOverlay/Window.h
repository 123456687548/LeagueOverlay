#pragma once
#include <Windows.h>
#include <dwmapi.h>

struct target_t
{
	char m_name[256];
	HWND m_hwnd;
	RECT m_size;
	DWORD m_style;
	BOOL m_checked;
	HANDLE m_pHandle;
	DWORD m_pid;
	DWORD m_module;
};

class Window {
private:
	char m_name[256];
	HINSTANCE m_hInstance;
	HWND m_hwnd;
	WNDCLASSEX m_windowClass;
	int m_width = 255;
	int m_height = 255;
	MARGINS m_margin;
	int m_nCmdShow;
	target_t m_targetWindow;
public:
	MSG m_message;
private:
	void createClass(WNDPROC winproc, const char* windowname);
	void createWindowOverlay();
public:
	Window(HINSTANCE hInstance, int nCmdShow);
	void findTargetWindow();
	int getWidth() { return m_width; }
	int getHeight() { return m_height; }
	target_t getTargetWindow() { return m_targetWindow; }
	HWND getHWND() { return m_hwnd; }
};

