#include "IVideo.h"
#include <stdio.h>
#include <gl/glew.h>
#include <SDL_events.h>

bool HandleInput(IVideo**);
void drawRect(int, int);

int main(int argc, char *argv[])
{
	IVideo *pVideo = IVideo::Create();
	printf("IVideo* created: %p\n", pVideo);
	pVideo->Reset(640, 480, false);
	pVideo->SetClearColor(0.0f, 0.0f, 0.25f, 1.0f);

	while (HandleInput(&pVideo))
	{
		pVideo->ClearScreen();
		drawRect(32,32);
		pVideo->PresentFrame();
	}

	delete pVideo;

	return 0;
}

void drawRect(int w, int h)
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glBegin(GL_QUADS);
		glVertex3i(-w/2, -h/2, -1);
		glVertex3i(-w/2, h/2, -1);
		glVertex3i(w/2, h/2, -1);
		glVertex3i(w/2, -h/2, -1);
	glEnd();
}

bool HandleInput(IVideo** ppVideo)
{
	static bool fullscreen = false;
	bool running = true;
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			running = false;
			break;
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym)
			{
			case SDLK_ESCAPE:
			case SDLK_q:
				running = false;
				break;
			case SDLK_f:
				fullscreen = !fullscreen;
				if (fullscreen) (*ppVideo)->Reset(1366, 768, true);
				else (*ppVideo)->Reset(640, 480, false);
				break;
			case SDLK_r:
				delete *ppVideo;
				*ppVideo = IVideo::Create();
				printf("IVideo* created: %p\n", *ppVideo);
				fullscreen = false;
				(*ppVideo)->Reset(640, 480, false);
				(*ppVideo)->SetClearColor(0.0f, 0.0f, 0.25f, 1.0f);
				break;
			}
			break;
		}
	}
	return running;
}