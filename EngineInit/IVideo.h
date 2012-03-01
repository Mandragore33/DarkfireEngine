#pragma once

class IVideo
{
public:
	static IVideo *Create();
	virtual ~IVideo() {}

	virtual bool Reset(int width, int height, bool fullscreen) = 0;
	virtual void Shutdown() = 0;

	virtual void SetClearColor(float r, float g, float b, float a) = 0;
	virtual void ClearScreen() = 0;

	virtual void PresentFrame() = 0;

protected:
	IVideo() {}

	static IVideo *instance_;
};