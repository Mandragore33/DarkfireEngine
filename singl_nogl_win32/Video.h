#pragma once

#include "IVideo.h"

#include <Windows.h>
#include <gl/glew.h>

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
	
	HDC hdc;
	HGLRC hglrc;
	HWND hwnd;
	int screenw;
	int screenh;
	int screenmode;
	int indexPixelFormat;

	float clearColor_[4];
	bool initialized_;
};