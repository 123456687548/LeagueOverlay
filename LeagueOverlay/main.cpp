#include <Windows.h>
#include "Window.h"
#include "DirectX.h"

Window* window;
 
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
	window = new Window(hInstance, nCmdShow);
	//DirectxFunctions::DirectXInit(window->getHWND());

	MoveWindow(window->getHWND(), 127, 109, window->getWidth(), window->getHeight(), true);
	while (1)
	{
		if (PeekMessage(&window->m_message, window->getHWND(), 0, 0, PM_REMOVE))
		{
			DispatchMessage(&window->m_message);
			TranslateMessage(&window->m_message);
		}
		Sleep(1);
		if (GetAsyncKeyState(VK_F12) & 1) {
			POINT p;
			if (GetCursorPos(&p)) {
				MoveWindow(window->getHWND(), p.x, p.y, window->getWidth(), window->getHeight(), true);
			}
		}
		window->findTargetWindow();
	}

	return 0;
}