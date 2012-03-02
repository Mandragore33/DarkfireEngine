#include "IVideo.h"
#include <stdio.h>
#include <gl/glew.h>
#include <SDL_events.h>

static GLuint rectBuffer;

bool HandleInput(IVideo**);
void prepareRect(int, int);
void drawRect();

int main(int argc, char *argv[])
{
	IVideo *pVideo = IVideo::Create();
	printf("IVideo* created: %p\n", pVideo);
	pVideo->Reset(640, 480, false);
	pVideo->SetClearColor(0.0f, 0.0f, 0.25f, 1.0f);
	prepareRect(32,32);
	while (HandleInput(&pVideo))
	{
		pVideo->ClearScreen();
		drawRect();
		pVideo->PresentFrame();
	}

	delete pVideo;

	return 0;
}

void prepareRect(int w, int h)
{
	GLfloat data[12] = {
		-w/2.0f, -h/2.0f, -1.0f,
		-w/2.0f, h/2.0f, -1.0f,
		w/2.0f, h/2.0f, -1.0f,
		w/2.0f, -h/2.0f, -1.0f
	};

	glGenBuffers(1, &rectBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, rectBuffer);
	glBufferData(GL_ARRAY_BUFFER, 12*sizeof(float), data, GL_STATIC_DRAW);
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, 0);
}

void drawRect()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glDrawArrays(GL_QUADS, 0, 4);
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