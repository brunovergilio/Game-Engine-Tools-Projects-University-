#include "PointLightShadows.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	PointLightShadows app("Point Light Shadow Test", 1024, 768);
	app.Run();

	return 0;
}