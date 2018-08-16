#ifndef HARDWARETRANSFORMATIONDEMO_H
#define HARDWARETRANSFORMATIONDEMO_H

#include "Engine.h"
#include "Graphics.h"
#include "Camera.h"
#include "Texture.h"
#include "Sampler.h"
#include "FPS.h"
#include "Light.h"
#include "Material.h"
#include "AnimatedGameObject.h"
#include "SpriteBatch.h"
#include "SpriteFont.h"

class HardwareTransformationDemo : public Engine
{
public:
	HardwareTransformationDemo(const HINSTANCE &hInstance, const char * const pWindowName, const UINT width = 640U, const UINT height = 480U);
	~HardwareTransformationDemo();

private:
	bool Initialize() override;
	void Shutdown() override;
	void Update() override;
	void Render() override;

	// Init window resources
	bool InitWindowResources();
	// Windows Callback Procedure
	LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	// Not needed
	HardwareTransformationDemo() = delete;
	HardwareTransformationDemo(const HardwareTransformationDemo &) = delete;
	HardwareTransformationDemo & operator = (const HardwareTransformationDemo &) = delete;

private:
	Graphics *m_pGraphics;

	Camera *m_pCamera;

	AnimatedGameObject *m_pObj1;
	AnimatedGameObject *m_pObj2;

	Sampler *m_pSamplerPoint;
	Sampler *m_pSamplerAnisotropic;

	FPS *m_pFPS;

	Texture *m_pZombieTex;

	PointLight m_PointLight;

	SpriteBatch *m_pBatch;
	SpriteFont *m_pFontBatch;
	BDVModel *m_pBDVModel1;
	BDVModel *m_pBDVModel2;
	AnimatedShader *m_pAnimShader;
};

#endif