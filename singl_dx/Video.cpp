#include "Video.h"
#include <SDL_syswm.h>

IVideo *IVideo::instance_ = 0;

IVideo *IVideo::Create()
{
	if (!instance_) instance_ = new CVideo();
	return instance_;
}

CVideo::CVideo()
	: window_(0), d3dDevice_(0), swapChain_(0), driverType_(D3D10_DRIVER_TYPE_HARDWARE), backBufferTarget_(0), depthTexture_(0), depthStencilView_(0), hInstance_(0), hWnd_(0), initialized_(false)
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
		if (depthStencilView_) depthStencilView_->Release();
		depthStencilView_ = 0;

		if (depthTexture_) depthTexture_->Release();
		depthTexture_ = 0;

		if (backBufferTarget_) backBufferTarget_->Release();
		backBufferTarget_ = 0;

		if (swapChain_) swapChain_->Release();
		swapChain_ = 0;

		if (d3dDevice_) d3dDevice_->Release();
		d3dDevice_ = 0;

		SDL_QuitSubSystem(SDL_INIT_VIDEO);
		window_ = 0;
		hInstance_ = 0;
		hWnd_ = 0;
	}
	initialized_ = false;
}

bool CVideo::Reset(int width, int height, bool fullscreen)
{
	Shutdown();

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		return false;

	Uint32 flags = SDL_WINDOW_SHOWN;
	if (fullscreen) flags |= SDL_WINDOW_FULLSCREEN;
	window_ = SDL_CreateWindow("Demo", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags);
	if (!window_)
	{
		SDL_Quit();
		return false;
	}

	SDL_SysWMinfo wminfo;
	SDL_VERSION(&wminfo.version);
	if (!SDL_GetWindowWMInfo(window_, &wminfo))
	{
		SDL_Quit();
		return false;
	}

	hWnd_ = wminfo.info.win.window;
	hInstance_ = (HINSTANCE)GetWindowLong(hWnd_, GWL_HINSTANCE);

	RECT dimensions;
	GetClientRect(hWnd_, &dimensions);

	unsigned int w = dimensions.right - dimensions.left;
	unsigned int h = dimensions.bottom - dimensions.top;

	D3D10_DRIVER_TYPE driverTypes[] =
	{
		D3D10_DRIVER_TYPE_HARDWARE,
		D3D10_DRIVER_TYPE_WARP,
		D3D10_DRIVER_TYPE_REFERENCE
	};

	unsigned int totalDriverTypes = ARRAYSIZE(driverTypes);

	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = w;
	swapChainDesc.BufferDesc.Height = h;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = hWnd_;
	swapChainDesc.Windowed = true;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	unsigned int creationFlags = 0;

	HRESULT result;
	unsigned int driver = 0;

	for (driver = 0; driver < totalDriverTypes; ++driver)
	{
		result = D3D10CreateDeviceAndSwapChain(0, driverTypes[driver], 0, creationFlags,
			D3D10_SDK_VERSION, &swapChainDesc, &swapChain_, &d3dDevice_);

		if (SUCCEEDED(result))
		{
			driverType_ = driverTypes[driver];
			break;
		}
	}

	if (FAILED(result))
		return false;

	ID3D10Texture2D *backBufferTexture;
	result = swapChain_->GetBuffer(0, __uuidof(ID3D10Texture2D), (LPVOID*)&backBufferTexture);

	if (FAILED(result))
		return false;

	result = d3dDevice_->CreateRenderTargetView(backBufferTexture, 0, &backBufferTarget_);

	if (backBufferTexture)
		backBufferTexture->Release();

	if (FAILED(result))
		return false;

	D3D10_TEXTURE2D_DESC depthTexDesc;
	ZeroMemory(&depthTexDesc, sizeof(depthTexDesc));
	depthTexDesc.Width = w;
	depthTexDesc.Height = h;
	depthTexDesc.MipLevels = 1;
	depthTexDesc.ArraySize = 1;
	depthTexDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthTexDesc.SampleDesc.Count = 1;
	depthTexDesc.SampleDesc.Quality = 0;
	depthTexDesc.Usage = D3D10_USAGE_DEFAULT;
	depthTexDesc.BindFlags = D3D10_BIND_DEPTH_STENCIL;

	result = d3dDevice_->CreateTexture2D(&depthTexDesc, NULL, &depthTexture_);

	if (FAILED(result))
		return false;

	D3D10_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format = depthTexDesc.Format;
	descDSV.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;

	result = d3dDevice_->CreateDepthStencilView(depthTexture_, &descDSV, &depthStencilView_);

	if (FAILED(result))
		return false;

	d3dDevice_->OMSetRenderTargets(1, &backBufferTarget_, depthStencilView_);

	D3D10_VIEWPORT viewport;
	viewport.Height = h;
	viewport.Width = w;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;

	d3dDevice_->RSSetViewports(1, &viewport);

	initialized_ = true;
	return true;
}

inline void CVideo::SetClearColor(float r, float g, float b, float a)
{
	clearColor_[0] = r;
	clearColor_[1] = g;
	clearColor_[2] = b;
	clearColor_[3] = a;
}

void CVideo::ClearScreen()
{
	d3dDevice_->ClearRenderTargetView(backBufferTarget_, clearColor_);
	d3dDevice_->ClearDepthStencilView(depthStencilView_, D3D10_CLEAR_DEPTH, 0.0f, 0);
}

void CVideo::PresentFrame()
{
	swapChain_->Present(0, 0);
}