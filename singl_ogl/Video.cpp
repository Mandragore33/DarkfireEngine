#include "Video.h"
#include <stdio.h>

IVideo *IVideo::instance_ = 0;

IVideo *IVideo::Create()
{
	if (!instance_) instance_ = new CVideo();
	return instance_;
}

CVideo::CVideo()
	: window_(0), glContext_(0), initialized_(false)
{
	for (int i = 0; i < 4; i++)
		clearColor_[i] = 0.0f;
}


CVideo::~CVideo()
{
	Shutdown();
	instance_ = 0;
}

inline void CVideo::Shutdown()
{
	if (initialized_)
	{
		SDL_QuitSubSystem(SDL_INIT_VIDEO);
		window_ = 0;
	}
	initialized_ = false;
}

void SetView(int, int);	// TEMP

bool CVideo::Reset(int width, int height, bool fullscreen)
{
	if (initialized_)
	{
		if (fullscreen_)
		{
			if (fullscreen)
			{
				SDL_SetWindowFullscreen(window_, SDL_FALSE);
				SDL_SetWindowSize(window_, width, height);
				SDL_SetWindowFullscreen(window_, SDL_TRUE);
			}
			else
			{
				SDL_SetWindowFullscreen(window_, SDL_FALSE);
				SDL_SetWindowSize(window_, width, height);
				fullscreen_ = SDL_FALSE;
			}
		}
		else
		{
			if (fullscreen)
			{
				SDL_SetWindowSize(window_, width, height);
				SDL_SetWindowFullscreen(window_, SDL_TRUE);
				fullscreen_ = SDL_TRUE;
			}
			else
				SDL_SetWindowSize(window_, width, height);
		}
	}
	else
	{
		if (SDL_Init(SDL_INIT_VIDEO) < 0)
			return false;

		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);

		Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
		if (fullscreen)
		{
			flags |= SDL_WINDOW_FULLSCREEN;
			fullscreen_ = SDL_TRUE;
		}
		else
			fullscreen_ = SDL_FALSE;

		window_ = SDL_CreateWindow("Demo", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags);
		if (!window_)
		{
			SDL_QuitSubSystem(SDL_INIT_VIDEO);
			return false;
		}

		glContext_ = SDL_GL_CreateContext(window_);

		GLenum err = glewInit();
		if (err != GLEW_OK)
		{
			SDL_QuitSubSystem(SDL_INIT_VIDEO);
			return false;
		}

		if (GLEW_EXT_texture_filter_anisotropic)
			printf("EXT_texture_filter_anisotropic is supported!\n");
		else
			printf("EXT_texture_filter_anisotropic is NOT supported.\n");

		glClearColor(clearColor_[0], clearColor_[1], clearColor_[2], clearColor_[3]);
		SDL_GL_SetSwapInterval(1);
	}

	SetView(width,height);	

	initialized_ = true;
	return true;
}

void SetView(int w, int h)	// TEMP
{
	glViewport(0,0,w,h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-w/2, w/2, -h/2, h/2, 0.01f, 100.0f);
}

void CVideo::SetClearColor(float r, float g, float b, float a)
{
	clearColor_[0] = r;
	clearColor_[1] = g;
	clearColor_[2] = b;
	clearColor_[3] = a;
	glClearColor(r, g, b, a);
}


void CVideo::ClearScreen()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void CVideo::PresentFrame()
{
	SDL_GL_SwapWindow(window_);
}