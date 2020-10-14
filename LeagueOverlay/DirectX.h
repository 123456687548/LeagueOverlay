#ifndef DIRECTX
#define DIRECTX

#include <d3d9.h>
#include <d3dx9.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

#include "Drawing.h"


struct directx_t {
	IDirect3D9Ex* Object;
	IDirect3DDevice9Ex* Device;
	D3DPRESENT_PARAMETERS Param;
	ID3DXFont* Font;
	ID3DXFont* espFont;
	ID3DXFont* FontWarning;
	ID3DXLine* Line;
	ID3DXSprite* Sprite;
	LPDIRECT3DTEXTURE9* Texture;
};

extern directx_t DirectX;

namespace DirectxFunctions {
	void DirectXInit(HWND hwnd);
	void RenderDirectX();
}



#endif