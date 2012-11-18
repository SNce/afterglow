#ifndef _RENDERER
#define _RENDERER

#include <Windows.h>
#include "Renderer\Types\Singleton.h"

#if defined (_OGL)
	#include "Renderer\Driver\OGL\OGLContext.h"
#endif

namespace AfterGlow
{

template <class Driver>
class Renderer : public Singleton<Renderer<Driver>>
{
private:
	Driver* pContext;
public:
	Renderer()
	{
		pContext = new Driver();
	}

	~Renderer()
	{
		delete pContext;
	}

	bool createContext(HWND hwnd)
	{
		return pContext->create30Context(hwnd);
	}

	void setupScene()
	{
		pContext->setupScene();
	}

	void reshapeWindow(int w, int h)
	{
		pContext->reshapeWindow(w, h);
	}

	void preRenderScene()
	{
		pContext->preRenderScene();
	}

	void renderScene()
	{
		pContext->renderScene();
	}

	void postRenderScene()
	{
		pContext->postRenderScene();
	}
};

}

#endif