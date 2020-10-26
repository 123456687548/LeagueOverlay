#include <Windows.h>
#include <chrono>
#include <thread>
#include "Window.h"
#include "ScreenCapture.h"

Window* window;

#ifdef _DEBUG
#define SHOW_CONSOLE
#endif

#undef SHOW_CONSOLE

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
#ifdef SHOW_CONSOLE
	AllocConsole();
	FILE* f;
	freopen_s(&f, "CONOUT$", "w", stdout);
#endif
	window = new Window(hInstance, "Main window", 253, 253);
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
		if (GetAsyncKeyState(VK_INSERT) & 1) {
			if (window->m_transparent) {
				window->removeColor();
			} else {
				window->setTransparent();
			}
		}

		window->findTargetWindow();
		if (window->getHWND() && window->getTargetWindow().m_active) {
			window->showWindow();
			window->drawScreenCapture();
			window->coverEdge();
		} else {
			window->hideWindow();
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	}

	delete window;
#ifdef SHOW_CONSOLE
	system("pause");
	fclose(f);
	FreeConsole();
#endif
	return 0;
}