#include <Windows.h>
#include <chrono>
#include <thread>
#include "Window.h"
#include "DirectX.h"

Window* window;

int CaptureAnImage(HWND hwndSource, HWND hwndDestination)
{
    HDC hdcScreen;
    HDC hdcWindow;
    HDC hdcMemDC = NULL;

    // Retrieve the handle to a display device context for the client 
    // area of the window. 
    hdcScreen = GetDC(NULL);
    hdcWindow = GetDC(hwndDestination);

    // Create a compatible DC which is used in a BitBlt from the window DC
    hdcMemDC = CreateCompatibleDC(hdcWindow);

    if (!hdcMemDC)
    {
        MessageBox(hwndDestination, "CreateCompatibleDC has failed", "Failed", MB_OK);
        return 1;
    }

    // Get the client area for size calculation
    RECT rcClient;
    GetClientRect(hwndDestination, &rcClient);

    //This is the best stretch mode
    SetStretchBltMode(hdcWindow, HALFTONE);

    //The source DC is the entire screen and the destination DC is the current window (HWND)
    if (!BitBlt(hdcWindow,
        0, 0,
        253, 253,
        hdcScreen,
        1655, 814,
        SRCCOPY))
    {
        MessageBox(hwndDestination, "BitBlt has failed", "Failed", MB_OK);
        return 1;
    }

    DeleteObject(hdcMemDC);
    ReleaseDC(hwndSource, hdcScreen);
    ReleaseDC(hwndDestination, hdcWindow);

    return 0;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
#ifdef _DEBUG
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);
#endif
	window = new Window(hInstance, nCmdShow);
	//DirectxFunctions::DirectXInit(window->getHWND());
    window->addTrayIcon();
	MoveWindow(window->getHWND(), 127, 109, window->getWidth(), window->getHeight(), true);
	while (window->isRunning())
	{

		if (PeekMessage(&window->m_message, window->getHWND(), 0, 0, PM_REMOVE))
		{
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
            CaptureAnImage(window->getTargetWindow().m_hwnd, window->getHWND());
        }
        else {
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