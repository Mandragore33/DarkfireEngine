#pragma once

#include "IVideo.h"

#include <SDL.h>
#include <gl/glew.h>

class CVideo : public IVideo
{
public:
	CVideo();
	virtual ~CVideo();

	virtual bool Reset(int width, int height, bool fullscreen, bool vsync = true);
	virtual void Shutdown();

	virtual void SetClearColor(float r, float g, float b, float a);
	virtual void ClearScreen();

	virtual void PresentFrame();

private:

	SDL_Window *window_;
	SDL_GLContext glContext_;
	SDL_bool fullscreen_;

	float clearColor_[4];
	bool initialized_;
};

