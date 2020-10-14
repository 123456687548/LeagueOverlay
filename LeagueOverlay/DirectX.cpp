#include "DirectX.h"
#include <Psapi.h>
//#include <cwchar>

#include "Window.h"

extern Window* window;

directx_t DirectX;
ID3DXFont* pFont;
LPD3DXFONT font_interface = NULL;
IDirect3D9Ex* p_Object = 0;
IDirect3DDevice9Ex* p_Device = 0;
D3DPRESENT_PARAMETERS p_Params;
RECT rc;

bool bInitialized = false;

bool bKeys[255];
bool bKeyPrev[255];

bool bMenuEnabled = true;


#define MENUITEMS 3
bool bMenuItems[MENUITEMS];
int iSelectedItem = 0;

void DrawString(char* String, int x, int y, int a, int r, int g, int b, ID3DXFont* font) {
	RECT FontPos;
	FontPos.left = x;
	FontPos.top = y;
	font->DrawTextA(0, String, strlen(String), &FontPos, DT_NOCLIP, D3DCOLOR_ARGB(a, r, g, b));
}

void DirectxFunctions::DirectXInit(HWND hwnd) {
	if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &DirectX.Object)))
		exit(1);

	ZeroMemory(&DirectX.Param, sizeof(DirectX.Param));
	DirectX.Param.Windowed = true;
	DirectX.Param.BackBufferFormat = D3DFMT_A8R8G8B8;
	DirectX.Param.BackBufferHeight = window->getHeight();
	DirectX.Param.BackBufferWidth = window->getWidth();
	DirectX.Param.EnableAutoDepthStencil = true;
	DirectX.Param.AutoDepthStencilFormat = D3DFMT_D16;
	DirectX.Param.MultiSampleQuality = D3DMULTISAMPLE_NONE;
	DirectX.Param.SwapEffect = D3DSWAPEFFECT_DISCARD;


	if (FAILED(DirectX.Object->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &DirectX.Param, 0, &DirectX.Device)))
		exit(1);


	D3DXCreateFont(DirectX.Device, 20, 0, FW_BOLD, 1, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial", &DirectX.Font);
	D3DXCreateFont(DirectX.Device, 13, 0, 0, 0, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Arial", &DirectX.espFont);
	D3DXCreateSprite(DirectX.Device, &DirectX.Sprite);
	if (!DirectX.Line)
		D3DXCreateLine(DirectX.Device, &DirectX.Line);
}


void DirectxFunctions::RenderDirectX() {
	DirectX.Device->BeginScene();
	//if (GetForegroundWindow() == window->getTargetWindow().m_hwnd)
	{
		for (int i = 0; i < 255; i++) {
			if (GetAsyncKeyState(i) != 0) {
				if (bKeyPrev[i] == false) {
					bKeyPrev[i] = true;
					bKeys[i] = true;
				} else {
					bKeys[i] = false;
				}
			} else {
				bKeys[i] = false;
				bKeyPrev[i] = false;
			}
		}
		if (bKeys[VK_INSERT]) {
			bMenuEnabled = !bMenuEnabled;
		}

		if (bMenuEnabled) {
			//EXAMPLES to draw boxes or rectangles!
			//DrawRect(p_Device, 20, 20, 200, 200, D3DCOLOR_ARGB(186, 206, 168, 168));
			//DrawBorderBox(10, 10, 200, 200, 1, 255, 9, 255, 0);
			//DrawFilledRectangle(10, 10, 170, 200, 255, 30, 92, 192);

			RECT pos;
			pos.right = 1001;
			pos.bottom = 1001;

			pos.left = 20;
			pos.top = 20;

			//bg
			//Drawing::FilledRect(0, 0, 600, 600, D3DCOLOR_ARGB(1, 200, 0, 0));
			//outer rect
			//Drawing::BorderedRect(0, 0, 254, 254, 1, 1, 1, 1, D3DCOLOR_ARGB(255, 255, 255, 255));
			Drawing::BorderedRect(0, 0, 600, 600, 1, 1, 1, 1, D3DCOLOR_ARGB(255, 255, 255, 255));
			//Drawing::FilledRect(17, 19, 205, 19, D3DCOLOR_ARGB(255, 255, 255, 255));
			//DirectX.Font->DrawTextW(NULL, L"Skyrim Tutorial", -1, &pos, 0, D3DCOLOR_ARGB(255, 5, 5, 5));
			pos.top += 20;


			HDC hScreenDC = CreateDCA("DISPLAY", NULL, NULL, NULL);
			// and a device context to put it in
			HDC hMemoryDC = CreateCompatibleDC(hScreenDC);

			int width = GetDeviceCaps(hScreenDC, HORZRES);
			int height = GetDeviceCaps(hScreenDC, VERTRES);

			// maybe worth checking these are positive values
			HBITMAP hBitmap = CreateCompatibleBitmap(hScreenDC, width, height);

			HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemoryDC, hBitmap);

			BitBlt(hMemoryDC, 0, 0, width, height, hScreenDC, 0, 0, SRCCOPY);
			hBitmap = (HBITMAP)SelectObject(hMemoryDC, hOldBitmap);

			BITMAP bmpScreen;
			GetObject(hBitmap, sizeof(BITMAP), &bmpScreen);

			// clean up
			DeleteDC(hMemoryDC);
			DeleteDC(hScreenDC);

			//D3DXCreateTexture(DirectX.Device, 400,400,0, D3DUSAGE_RENDERTARGET, D3DFMT_A8B8G8R8, )
			if (FAILED(D3DXCreateTextureFromFileInMemory(DirectX.Device, hBitmap, sizeof(hBitmap), DirectX.Texture))) {
				Sleep(1);
			}
			D3DXIMAGE_INFO image_info;
			image_info.Width = 254;
			image_info.Height = 255;
			image_info.Depth = 1;
			image_info.MipLevels = 1;
			image_info.ImageFileFormat = D3DXIFF_BMP;
			image_info.Format = D3DFMT_A8R8G8B8;
			image_info.ResourceType = D3DRTYPE_SURFACE;
			HRESULT hr = D3DXCreateTextureFromFileInMemoryEx(DirectX.Device, hBitmap, sizeof(hBitmap), 254, 255, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, 0, 0, 0, NULL, NULL, DirectX.Texture);
			D3DXVECTOR3 imagepos{ 0,0,0 };
			DirectX.Sprite->Begin(D3DXSPRITE_ALPHABLEND); //begin our sprite with alphablend so it support alpha, like png use alpha
			DirectX.Sprite->Draw(*DirectX.Texture, NULL, NULL, &imagepos, 0xFFFFFFFF); //Draw the sprite, first parameter is the image texture, 4th parameter is the position, last parameter is the color change leave to 0xFFFFFFFF for no change
			DirectX.Sprite->End(); //end the sprite

		}
	}
	DirectX.Device->EndScene();
	DirectX.Device->PresentEx(0, 0, 0, 0, 0);
	DirectX.Device->Clear(0, 0, D3DCLEAR_TARGET, 0, 1.0f, 0);
}