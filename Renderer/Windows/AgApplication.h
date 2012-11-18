#ifndef _AG_APPLICATION
#define _AG_APPLICATION

#define _OGL

#include "Renderer\Engine\Renderer\Renderer.h"
namespace AfterGlow
{

class AgApplication
{
public:
	AgApplication();
	~AgApplication();

	void Run();

#if defined (_OGL)
	#define DRIVER OGLContext
	// @TODO: support other drivers
#else
	#define DRIVER Error_No_Driver_Defined
#endif
	Renderer<DRIVER>& getRenderer() {return Renderer<DRIVER>::instance();}
};

}

#endif