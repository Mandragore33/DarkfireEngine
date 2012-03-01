#include "Video.h"

#define WNDCLASSNAME "DarkfireWindow"

IVideo *IVideo::instance_ = 0;

IVideo *IVideo::Create()
{
	if (!instance_) instance_ = new CVideo();
	return instance_;
}

CVideo::CVideo()
	: initialized_(false)
{
	for (int i = 0; i < 4; i++)
		clearColor_[i] = 0.0f;
}


CVideo::~CVideo()
{
	Shutdown();
	instance_ = 0;
}

void CVideo::Shutdown()
{
	wglMakeCurrent(hdc, NULL);	// release device context in use by rc
    wglDeleteContext(hglrc);	// delete rendering context

    PostQuitMessage(0);		// make sure the window will be destroyed
}

bool CVideo::Reset(int width, int height, bool fullscreen)
{
	MSG msg;
	WNDCLASSEX ex;

	ex.cbSize = sizeof(WNDCLASSEX);
    ex.style = CS_HREDRAW|CS_VREDRAW|CS_OWNDC;
    ex.lpfnWndProc = WinProc;
    ex.cbClsExtra = 0;
    ex.cbWndExtra = 0;
    ex.hInstance = hinstance;
    ex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    ex.hCursor = LoadCursor(NULL, IDC_ARROW);
    ex.hbrBackground = NULL;
    ex.lpszMenuName = NULL;
    ex.lpszClassName = WNDCLASSNAME;
    ex.hIconSm = NULL;

	RegisterClassEx(&ex);

	// center position of the window
    int posx = (GetSystemMetrics(SM_CXSCREEN) / 2) - (width / 2);
    int posy = (GetSystemMetrics(SM_CYSCREEN) / 2) - (height / 2);

    // set up the window for a windowed application by default
    long wndStyle = WS_OVERLAPPEDWINDOW;

    if (fullscreen)	// create a full-screen application if requested
    {
        wndStyle = WS_POPUP;
        posx = 0;
        posy = 0;

        // change resolution before the window is created
        SysSetDisplayMode(screenw, screenh, SCRDEPTH);
    }

    // create the window
    hwnd = CreateWindowEx(NULL,
                  WNDCLASSNAME,
                  WNDNAME,
                  wndStyle|WS_CLIPCHILDREN|WS_CLIPSIBLINGS,
                  posx, posy,
                  width, height,
                  NULL,
                  NULL,
                  hinst,
                  NULL);

    if ((hdc = GetDC(hwnd)) == NULL)
	{
		MessageBox(hwnd, "Failed to Get the Window Device Context",
                         "Device Context Error", MB_OK);
		exit(1);		// temp
	}

	SetGLFormat();

	if ((hglrc = wglCreateContext(hdc)) == NULL)
	{
		MessageBox(hwnd, "Failed to Create the OpenGL Rendering Context",
                         "OpenGL Rendering Context Error", MB_OK);
		exit(1);
	}

	if (!wglMakeCurrent(hdc, hglrc))
	{
		MessageBox(hwnd, "Failed to make OpenGL Rendering Context current",
                         "OpenGL Rendering Context Error", MB_OK);
		exit(1);
	}

	Resize(SCRWIDTH, SCRHEIGHT);

	SetClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);

	return true;
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
	SwapBuffers(hdc);
}