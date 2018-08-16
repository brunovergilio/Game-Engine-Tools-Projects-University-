#ifndef NormalMappingDemoDEMO_H
#define NormalMappingDemoDEMO_H

#include "Engine.h"
#include "Graphics.h"
#include "Camera.h"
#include "Texture.h"
#include "Sampler.h"
#include "FPS.h"
#include "Light.h"
#include "Material.h"
#include "BasicGameObject.h"
#include "SpriteBatch.h"
#include "SpriteFont.h"

class NormalMappingDemo : public Engine
{
public:
	NormalMappingDemo(const HINSTANCE &hInstance, const char * const pWindowName, const UINT width = 640U, const UINT height = 480U);
	~NormalMappingDemo();

private:
	bool Initialize() override;
	void Shutdown() override;
	void Update() override;
	void Render() override;

	// Init window resources
	bool InitWindowResources();
	// Windows Callback Procedure
	LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	bool IsKeyPressed(const int key);

	// Not needed
	NormalMappingDemo() = delete;
	NormalMappingDemo(const NormalMappingDemo &) = delete;
	NormalMappingDemo & operator = (const NormalMappingDemo &) = delete;

private:
	Graphics *m_pGraphics;

	Camera *m_pCamera;

	BasicGameObject *m_pObj1;
	BasicGameObject *m_pObj2;

	Texture *m_pBlueWallTex;
	Texture *m_pBlueWallNormalMapTex;

	Texture *m_pBlackWallTex;
	Texture *m_pBlackWallNormalMapTex;

	Sampler *m_pSamplerPoint;
	Sampler *m_pSamplerAnisotropic;

	FPS *m_pFPS;

	PointLight m_PointLight;

	SpriteBatch *m_pBatch;
	SpriteFont *m_pFontBatch;
	BDVModel *m_pBDVModel;
	BasicShader *m_pBasicShader;
};

#endif