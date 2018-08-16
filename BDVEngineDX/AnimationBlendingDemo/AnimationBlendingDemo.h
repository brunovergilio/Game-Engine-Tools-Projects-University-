#ifndef ANIMATIONBLENDINGDEMO_H
#define ANIMATIONBLENDINGDEMO_H

#include "Engine.h"
#include "Graphics.h"
#include "Camera.h"
#include "Texture.h"
#include "Sampler.h"
#include "FPS.h"
#include "Light.h"
#include "Material.h"
#include "AnimBlendGameObject.h"
#include "SpriteBatch.h"
#include "SpriteFont.h"

class AnimationBlendingDemo : public Engine
{
public:
	AnimationBlendingDemo(const HINSTANCE &hInstance, const char * const pWindowName, const UINT width = 640U, const UINT height = 480U);
	~AnimationBlendingDemo();

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
	AnimationBlendingDemo() = delete;
	AnimationBlendingDemo(const AnimationBlendingDemo &) = delete;
	AnimationBlendingDemo & operator = (const AnimationBlendingDemo &) = delete;

private:
	Graphics *m_pGraphics;

	Camera *m_pCamera;

	AnimBlendGameObject *m_pObj;

	Sampler *m_pSamplerPoint;
	Sampler *m_pSamplerAnisotropic;

	FPS *m_pFPS;

	PointLight m_PointLight;

	SpriteBatch *m_pBatch;
	SpriteFont *m_pFontBatch;
	BDVModel *m_pBDVModel;
	AnimBlendShader *m_pAnimShader;
};

#endif