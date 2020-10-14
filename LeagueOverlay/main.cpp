#include <Windows.h>
#include <chrono>
#include <thread>
#include "Window.h"
#include "DirectX.h"

Window* window;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
#ifdef _DEBUG
	AllocConsole();
	FILE* f;
	freopen_s(&f, "CONOUT$", "w", stdout);
#endif
	window = new Window(hInstance, 253, 253);
	window->initScreenCapture();
	//DirectxFunctions::DirectXInit(window->getHWND());
	window->addTrayIcon();
	MoveWindow(window->getHWND(), 127, 109, window->getWidth(), window->getHeight(), true);
	while (window->isRunning()) {

		if (PeekMessage(&window->m_message, window->getHWND(), 0, 0, PM_REMOVE)) {
			DispatchMessage(&window->m_message);
			TranslateMessage(&window->m_message);
		}
		if (GetAsyncKeyState(VK_F12) & 1) {
			POINT p;
			if (GetCursorPos(&p)) {
				MoveWindow(window->getHWND(), p.x, p.y, window->getWidth(), window->getHeight(), true);
			}
		}

		if (GetAsyncKeyState(VK_DELETE)) {
			window->stop();
		}

		window->findTargetWindow();
		if (window->getHWND() && window->getTargetWindow().m_hwnd && window->getTargetWindow().m_active) {
			window->showWindow();
			window->drawScreenCapture();
		} else {
			window->hideWindow();
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	}

	delete window;
#ifdef _DEBUG
	system("pause");
	fclose(f);
	FreeConsole();
#endif
	return 0;
}