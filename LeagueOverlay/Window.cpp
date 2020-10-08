#include <cstdio>
#include "Window.h"
#include "DirectX.h"

#pragma comment(lib, "dwmapi.lib")

int CaptureAnImage(HWND hWnd)
{
    HDC hdcScreen;
    HDC hdcWindow;
    HDC hdcMemDC = NULL;
    HBITMAP hbmScreen = NULL;
    BITMAP bmpScreen;

    // Retrieve the handle to a display device context for the client 
    // area of the window. 
    hdcScreen = GetDC(NULL);
    hdcWindow = GetDC(hWnd);

    // Create a compatible DC which is used in a BitBlt from the window DC
    hdcMemDC = CreateCompatibleDC(hdcWindow);

    if (!hdcMemDC)
    {
        MessageBox(hWnd, "CreateCompatibleDC has failed", "Failed", MB_OK);
        return 1;
    }

    // Get the client area for size calculation
    RECT rcClient;
    GetClientRect(hWnd, &rcClient);

    //This is the best stretch mode
    SetStretchBltMode(hdcWindow, HALFTONE);

    //The source DC is the entire screen and the destination DC is the current window (HWND)
    if (!StretchBlt(hdcWindow,
        0, 0,
        rcClient.right, rcClient.bottom,
        hdcScreen,
        0, 0,
        GetSystemMetrics(SM_CXSCREEN),
        GetSystemMetrics(SM_CYSCREEN),
        SRCCOPY))
    {
        MessageBox(hWnd, "StretchBlt has failed", "Failed", MB_OK);
        return 1;
    }

    // Create a compatible bitmap from the Window DC
    hbmScreen = CreateCompatibleBitmap(hdcWindow, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top);

    if (!hbmScreen)
    {
        MessageBox(hWnd, "CreateCompatibleBitmap Failed", "Failed", MB_OK);
        return 1;
    }

    // Select the compatible bitmap into the compatible memory DC.
    SelectObject(hdcMemDC, hbmScreen);

    // Bit block transfer into our compatible memory DC.
    if (!BitBlt(hdcMemDC,
        0, 0,
        rcClient.right - rcClient.left, rcClient.bottom - rcClient.top,
        hdcWindow,
        0, 0,
        SRCCOPY))
    {
        MessageBox(hWnd, "BitBlt has failed", "Failed", MB_OK);
        return 1;
    }

    // Get the BITMAP from the HBITMAP
    GetObject(hbmScreen, sizeof(BITMAP), &bmpScreen);

    BITMAPFILEHEADER   bmfHeader;
    BITMAPINFOHEADER   bi;

    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = bmpScreen.bmWidth;
    bi.biHeight = bmpScreen.bmHeight;
    bi.biPlanes = 1;
    bi.biBitCount = 32;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrUsed = 0;
    bi.biClrImportant = 0;

    DWORD dwBmpSize = ((bmpScreen.bmWidth * bi.biBitCount + 31) / 32) * 4 * bmpScreen.bmHeight;

    // Starting with 32-bit Windows, GlobalAlloc and LocalAlloc are implemented as wrapper functions that 
    // call HeapAlloc using a handle to the process's default heap. Therefore, GlobalAlloc and LocalAlloc 
    // have greater overhead than HeapAlloc.
    HANDLE hDIB = GlobalAlloc(GHND, dwBmpSize);
    char* lpbitmap = (char*)GlobalLock(hDIB);

    // Gets the "bits" from the bitmap and copies them into a buffer 
    // which is pointed to by lpbitmap.
    GetDIBits(hdcWindow, hbmScreen, 0,
        (UINT)bmpScreen.bmHeight,
        lpbitmap,
        (BITMAPINFO*)&bi, DIB_RGB_COLORS);

    // A file is created, this is where we will save the screen capture.
    HANDLE hFile = CreateFile("captureqwsx.bmp",
        GENERIC_WRITE,
        0,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL, NULL);

    // Add the size of the headers to the size of the bitmap to get the total file size
    DWORD dwSizeofDIB = dwBmpSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    //Offset to where the actual bitmap bits start.
    bmfHeader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER);

    //Size of the file
    bmfHeader.bfSize = dwSizeofDIB;

    //bfType must always be BM for Bitmaps
    bmfHeader.bfType = 0x4D42; //BM   

    DWORD dwBytesWritten = 0;
    WriteFile(hFile, (LPSTR)&bmfHeader, sizeof(BITMAPFILEHEADER), &dwBytesWritten, NULL);
    WriteFile(hFile, (LPSTR)&bi, sizeof(BITMAPINFOHEADER), &dwBytesWritten, NULL);
    WriteFile(hFile, (LPSTR)lpbitmap, dwBmpSize, &dwBytesWritten, NULL);

    //Unlock and Free the DIB from the heap
    GlobalUnlock(hDIB);
    GlobalFree(hDIB);

    //Close the handle for the file that was created
    CloseHandle(hFile);

    //Clean up

    DeleteObject(hbmScreen);
    DeleteObject(hdcMemDC);
    ReleaseDC(NULL, hdcScreen);
    ReleaseDC(hWnd, hdcWindow);

    return 0;
}


LRESULT CALLBACK WinProcedure(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch (Message)
	{
		case WM_PAINT:
			//DirectxFunctions::RenderDirectX();

			break;

		case WM_DESTROY:
			PostQuitMessage(1);
			break;

		default:
			return DefWindowProc(hWnd, Message, wParam, lParam);
			break;
	}
	return 0;
}

Window::Window(HINSTANCE hInstance, int nCmdShow) : m_hInstance(hInstance), m_nCmdShow(nCmdShow)
{
	findTargetWindow();
	createClass(WinProcedure, "overlay");
	createWindowOverlay();
}

void Window::createClass(WNDPROC winproc, const char* windowname)
{
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

void Window::findTargetWindow()
{
	m_targetWindow.m_hwnd = FindWindow(0, "League of Legends (TM) Client");

	if (m_targetWindow.m_hwnd)
	{
		GetWindowThreadProcessId(m_targetWindow.m_hwnd, &m_targetWindow.m_pid);
		m_targetWindow.m_pHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_targetWindow.m_pid);
		m_targetWindow.m_checked = true;
		GetWindowRect(m_targetWindow.m_hwnd, &m_targetWindow.m_size);
		m_width = m_targetWindow.m_size.right - m_targetWindow.m_size.left;
		m_height = m_targetWindow.m_size.bottom - m_targetWindow.m_size.top;
		m_targetWindow.m_style = GetWindowLong(m_targetWindow.m_hwnd, GWL_STYLE);

		if (m_targetWindow.m_style & WS_BORDER)
		{
			m_targetWindow.m_size.top += 23;
			m_height -= 23;
		}

		MoveWindow(m_targetWindow.m_hwnd, m_targetWindow.m_size.left, m_targetWindow.m_size.top, m_width, m_height, true);
		m_margin = { 0, 0, m_width, m_height };
	}
}

void Window::createWindowOverlay()
{
	m_hwnd = CreateWindowEx(WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT, m_name, m_name, WS_POPUP, 1, 1, m_width, m_height, 0, 0, m_hInstance, 0);
	SetLayeredWindowAttributes(m_hwnd, RGB(0, 0, 0), 255, LWA_COLORKEY | LWA_ALPHA);
	ShowWindow(m_hwnd, SW_SHOW);
	DwmExtendFrameIntoClientArea(m_hwnd, &m_margin);
}
