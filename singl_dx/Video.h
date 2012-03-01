#pragma once

#include "IVideo.h"

#include <SDL.h>
#include <D3D10.h>
#include <D3DX10.h>

class CVideo : public IVideo
{
public:
	CVideo();
	virtual ~CVideo();

	virtual bool Reset(int width, int height, bool fullscreen);
	virtual void Shutdown();

	virtual void SetClearColor(float r, float g, float b, float a);
	virtual void ClearScreen();

	virtual void PresentFrame();

private:
	SDL_Window *window_;
	ID3D10Device *d3dDevice_;
	IDXGISwapChain *swapChain_;
	D3D10_DRIVER_TYPE driverType_;
	ID3D10RenderTargetView *backBufferTarget_;
	ID3D10Texture2D *depthTexture_;
	ID3D10DepthStencilView *depthStencilView_;
	HINSTANCE hInstance_;
	HWND hWnd_;

	float clearColor_[4];
	bool initialized_;
};

