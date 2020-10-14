#pragma once
#include <Windows.h>
#include <dwmapi.h>

#define MY_TRAY_ICON_ID 1337
#define MY_TRAY_ICON_MESSAGE 1108

class ScreenCapture;

struct target_t {
	char m_name[256];
	HWND m_hwnd;
	RECT m_size;
	DWORD m_style;
	BOOL m_checked;
	HANDLE m_pHandle;
	DWORD m_pid;
	DWORD m_module;
	bool m_active;
};

class Window {
private:
	char m_name[256];
	HINSTANCE m_hInstance;
	HWND m_hwnd;
	WNDCLASSEX m_windowClass;
	int m_width = 253;
	int m_height = 253;
	MARGINS m_margin;
	target_t m_targetWindow;
	bool m_visible;
	bool m_running;
	NOTIFYICONDATA m_trayIconData;
	ScreenCapture* screenCapture;
public:
	MSG m_message;
private:
	void createClass(WNDPROC winproc, const char* windowname);
	void createWindowOverlay();
	void cleanUp();
public:
	Window(HINSTANCE hInstance);
	Window(HINSTANCE hInstance, int width, int height);
	~Window();
	void findTargetWindow();
	int getWidth() { return m_width; }
	int getHeight() { return m_height; }
	target_t getTargetWindow() { return m_targetWindow; }
	HWND getHWND() { return m_hwnd; }
	bool isRunning() { return m_running; }
	HINSTANCE getHInstance() { return m_hInstance; }
	void stop() { m_running = false; }
	void hideWindow();
	void showWindow();
	void addTrayIcon();
	void initScreenCapture();
	bool drawScreenCapture();
};

