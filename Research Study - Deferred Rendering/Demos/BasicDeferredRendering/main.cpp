#include "BasicDeferredRendering.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	BasicDeferredRendering app("Basic Deferred Rendering", 1024, 768);
	app.Run();

	return 0;
}