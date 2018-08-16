#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "Utils.h"

enum class ShaderType : int
{
	VS,
	VS_WITH_SO,
	GS,
	GS_WITH_SO,
	HS,
	DS,
	PS,
	CS
};

class Graphics
{
public:
	Graphics(const HWND hWnd, const UINT width = 640U, const UINT height = 480U);
	virtual ~Graphics();

	bool Initialize();
	void Shutdown();

	void Clear(const float r = 0.0f, const float g = 0.0f, const float b = 0.0f);
	void Present();

	// Handle window size changes
	bool SizeChange(const UINT width, const UINT height);

	ID3D11Device *GetDevice() const;
	ID3D11DeviceContext *GetContext() const;
	IDXGISwapChain *GetSwapChain() const;

	D3D_FEATURE_LEVEL GetFeatureLevel() const;
	const char *GetShaderNameAndVersion(const ShaderType shaderType) const;

private:
	// Init Direct3D resources
	bool InitD3DResources();
	// Init Direct3D size-dependent resources
	bool InitD3DSizeResources();

	// Not needed
	Graphics() = delete;
	Graphics(const Graphics &) = delete;
	Graphics & operator = (const Graphics &) = delete;

private:
	HWND m_hWnd;

	UINT m_Width;
	UINT m_Height;

	bool m_Windowed;
	UINT m_VSync;

	UINT m_QualityLevel;
	UINT m_SampleCount;

	D3D_FEATURE_LEVEL m_FeatureLevel;
	ComPtr<ID3D11Device> m_pDevice;
	ComPtr<ID3D11DeviceContext> m_pContext;
	ComPtr<IDXGISwapChain> m_pSwapChain;
	ComPtr<ID3D11RenderTargetView> m_pRenderTargetView;
	ComPtr<ID3D11DepthStencilView> m_pDepthStencilView;
	ComPtr<ID3D11Texture2D> m_pDepthStencilBuffer;
	ComPtr<ID3D11RasterizerState> m_pRasterizerState;
	D3D11_VIEWPORT m_Viewport;

};

#endif