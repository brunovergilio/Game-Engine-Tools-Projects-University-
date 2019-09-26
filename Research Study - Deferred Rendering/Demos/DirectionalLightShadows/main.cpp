#include "DirectionalLightShadows.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	DirectionalLightShadows app("Directional Light Shadow Test", 1024, 768);
	app.Run();

	return 0;
}