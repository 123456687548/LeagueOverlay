#include <cstdio>
#include "Window.h"
#include "DirectX.h"
#include "resource.h"
#include "ScreenCapture.h"

#pragma comment(lib, "dwmapi.lib")

extern Window* window;


LRESULT CALLBACK WinProcedure(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	switch (Message) {
		case WM_PAINT:
			//DirectxFunctions::RenderDirectX();

			break;

		case WM_DESTROY:
			break;

		case MY_TRAY_ICON_MESSAGE:
			switch (lParam) {
				case WM_RBUTTONDOWN:
					window->stop();
			}
			break;
		default:
			return DefWindowProc(hWnd, Message, wParam, lParam);
			break;
	}
	return 0;
}

Window::Window(HINSTANCE hInstance) : m_hInstance(hInstance), m_running(true) {
	findTargetWindow();
	createClass(WinProcedure, "overlay");
	createWindowOverlay();
}

Window::Window(HINSTANCE hInstance, int width, int height) :m_hInstance(hInstance), m_running(true), m_width(width), m_height(height) {
	findTargetWindow();
	createClass(WinProcedure, "overlay");
	createWindowOverlay();
}

Window::~Window() {
	printf("~Window\n");
	cleanUp();
	m_running = false;
}

void Window::createClass(WNDPROC winproc, const char* windowname) {
	sprintf_s(m_name, "%s", windowname);
	m_windowClass.cbClsExtra = 0;
	m_windowClass.cbSize = sizeof(WNDCLASSEX);
	m_windowClass.cbWndExtra = 0;
	m_windowClass.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(0, 0, 0));
	m_windowClass.hCursor = LoadCursor(0, IDC_ARROW);
	m_windowClass.hIcon = LoadIcon(0, IDI_APPLICATION);
	m_windowClass.hIconSm = LoadIcon(0, IDI_APPLICATION);
	m_windowClass.hInstance = m_hInstance;
	m_windowClass.lpfnWndProc = winproc;
	m_windowClass.lpszClassName = m_name;
	m_windowClass.lpszMenuName = m_name;
	m_windowClass.style = CS_HREDRAW | CS_VREDRAW;

	if (!RegisterClassEx(&m_windowClass))
		exit(1);
}

void Window::findTargetWindow() {
	//m_targetWindow.m_hwnd = FindWindow(0, "D3D11 Dummy"); 
	m_targetWindow.m_hwnd = FindWindow(0, "League of Legends (TM) Client");
	if (m_targetWindow.m_hwnd) {
		//GetWindowThreadProcessId(m_targetWindow.m_hwnd, &m_targetWindow.m_pid);
		//m_targetWindow.m_pHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_targetWindow.m_pid);
		m_targetWindow.m_active = m_targetWindow.m_hwnd == GetForegroundWindow();
		//m_targetWindow.m_active = true;
		m_targetWindow.m_checked = true;
		GetWindowRect(m_targetWindow.m_hwnd, &m_targetWindow.m_size);
		//m_width = m_targetWindow.m_size.right - m_targetWindow.m_size.left;
		//m_height = m_targetWindow.m_size.bottom - m_targetWindow.m_size.top;
		m_targetWindow.m_style = GetWindowLong(m_targetWindow.m_hwnd, GWL_STYLE);

		if (m_targetWindow.m_style & WS_BORDER) {
			m_targetWindow.m_size.top += 23;
			m_height -= 23;
		}

		//MoveWindow(m_hwnd, m_targetWindow.m_size.left, m_targetWindow.m_size.top, m_width, m_height, true);
		m_margin = { 0, 0, m_width, m_height };
	}
}

void Window::hideWindow() {
	if (!m_visible) return;
	ShowWindow(m_hwnd, SW_HIDE);
	m_visible = false;
}

void Window::showWindow() {
	if (m_visible) return;
	ShowWindow(m_hwnd, SW_SHOW);
	m_visible = true;
}

void Window::createWindowOverlay() {
	m_hwnd = CreateWindowEx(WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT, m_name, m_name, WS_POPUP, 1, 1, m_width, m_height, 0, 0, m_hInstance, 0);
	SetLayeredWindowAttributes(m_hwnd, RGB(0, 0, 0), 255, LWA_COLORKEY | LWA_ALPHA);
	DwmExtendFrameIntoClientArea(m_hwnd, &m_margin);
	showWindow();
}

void Window::addTrayIcon() {
	ZeroMemory(&m_trayIconData, sizeof(NOTIFYICONDATA));

	m_trayIconData.cbSize = sizeof(NOTIFYICONDATA);

	m_trayIconData.uID = MY_TRAY_ICON_ID;

	m_trayIconData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;

	m_trayIconData.hIcon =
		(HICON)LoadImage(window->getHInstance(), MAKEINTRESOURCE(IDI_ICON1),
			IMAGE_ICON,
			GetSystemMetrics(SM_CXSMICON),
			GetSystemMetrics(SM_CYSMICON),
			LR_DEFAULTCOLOR);

	m_trayIconData.hWnd = window->getHWND();

	m_trayIconData.uCallbackMessage = MY_TRAY_ICON_MESSAGE;

	Shell_NotifyIcon(NIM_ADD, &m_trayIconData);
}

void Window::initScreenCapture() {
	screenCapture = new ScreenCapture(this, NULL);
}

bool Window::drawScreenCapture() { return screenCapture->drawScreencapture(); };

void Window::cleanUp() {
	delete screenCapture;
	Shell_NotifyIcon(NIM_DELETE, &m_trayIconData);
	printf("Cleanup Window\n");
	int result = DestroyWindow(m_hwnd);
	printf("DestroyWindow: %d\n", result);
	result = UnregisterClass(m_name, m_hInstance);
	printf("UnregisterClass: %d\n", result);
}
