#ifndef ANIMATIONCOMPRESSIONDEMO_H
#define ANIMATIONCOMPRESSIONDEMO_H

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

class AnimationCompressionDemo : public Engine
{
public:
	AnimationCompressionDemo(const HINSTANCE &hInstance, const char * const pWindowName, const UINT width = 640U, const UINT height = 480U);
	~AnimationCompressionDemo();

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
	AnimationCompressionDemo() = delete;
	AnimationCompressionDemo(const AnimationCompressionDemo &) = delete;
	AnimationCompressionDemo & operator = (const AnimationCompressionDemo &) = delete;

private:
	Graphics *m_pGraphics;

	Camera *m_pCamera;

	AnimatedGameObject *m_pObj;
	AnimatedGameObject *m_pObjCompressed1;
	AnimatedGameObject *m_pObjCompressed2;

	Sampler *m_pSamplerPoint;
	Sampler *m_pSamplerAnisotropic;

	FPS *m_pFPS;

	PointLight m_PointLight;

	SpriteBatch *m_pBatch;
	SpriteFont *m_pFontBatch;
	BDVModel *m_pBDVModel;
	BDVModel *m_pBDVModelCompressed1;
	BDVModel *m_pBDVModelCompressed2;
	AnimatedShader *m_pAnimShader;
};

#endif