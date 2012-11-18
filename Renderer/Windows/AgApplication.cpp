#include "AgApplication.h"

namespace AfterGlow
{

	AgApplication::AgApplication()
	{
	}

	AgApplication::~AgApplication()
	{
	}

	void AgApplication::Run()
	{
		getRenderer().preRenderScene();
		getRenderer().renderScene();
		getRenderer().postRenderScene();
	}
}