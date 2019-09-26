#ifndef RENDERERD3D11_H
#define RENDERERD3D11_H

#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "InputLayout.h"
#include "ConstantBuffer.h"
#include "StructuredBuffer.h"
#include "ShaderResourceView.h"
#include "SamplerState.h"
#include "RasterizerState.h"
#include "BlendState.h"
#include "DepthStencilState.h"
#include "Shader.h"
#include "RenderTargetView.h"
#include "DepthStencilView.h"
#include "Texture.h"

using namespace Microsoft::WRL;

namespace BVGraphics
{
#define BV_MAX_VERTEX_BUFFERS 4
#define BV_MAX_CONSTANT_BUFFERS 16
#define BV_MAX_SRVS 16
#define BV_MAX_SAMPLERS 16
#define BV_MAX_RTVS 8

	enum class ShaderType : int
	{
		Vertex,
		Hull,
		Domain,
		Geometry,
		Pixel,
		Compute
	};

	enum class Topology : int
	{
		TriangleList = 4,
		TriangleStrip = 5,
		ControlPoint1 = 33
	};

	class RendererD3D11
	{
	public:
		RendererD3D11();
		~RendererD3D11();

		GraphicsResult Initialize(HWND hWnd, const unsigned int width, const unsigned int height);
		void Shutdown();
		GraphicsResult Resize(const unsigned int width, const unsigned int height);
		GraphicsResult ToggleFullScreen(const bool fullScreen);

		void GetResolution(unsigned int & width, unsigned int & height);
		void GetMSAA(unsigned int & sampleCount, unsigned int & qualityLevel);

		void ClearDepthStencilView(const DepthStencilView * const pDSV,
			const bool depth = true, const float depthVal = 1.0f, const bool stencil = true, const unsigned char stencilVal = 0);
		void ClearRenderTargetView(const RenderTargetView * const pRTV, const float r = 0.0f, const float g = 0.0f, const float b = 0.0f);
		void SetRenderTargetViews(const RenderTargetView * const * const pRTVs, const unsigned int numRTVs, const DepthStencilView * const pDSV);

		void SetMainViewPort();

		void SetPrimitiveTopology(const Topology topology);
		void SetInputLayout(const InputLayout * const pIA);
		void SetVertexBuffers(const VertexBuffer * const * const pVB, const unsigned int numBuffers);
		void SetIndexBuffer(const IndexBuffer * const pIB, const DXGI_FORMAT format = DXGI_FORMAT_R32_UINT);
		void SetConstantBuffers(const ShaderType shaderType, const ConstantBuffer * const * const pCB, const unsigned int numBuffers);
		void SetShader(const ShaderType shaderType, const Shader * const pShader);
		void SetShaderResources(const ShaderType shaderType, const ShaderResourceView * const * const pSRVs, const unsigned int numSRVs);
		void SetSamplers(const ShaderType shaderType, const SamplerState * const * const pSamplers, const unsigned int numSamplers);
		void SetRasterizerState(const RasterizerState * const pRasterizerState);
		void SetBlendState(const BlendState * const pBlendState, const float * const blendFactor, const unsigned int sampleMask = 0xFFFFFFFF);
		void SetDepthStencilState(const DepthStencilState * const pDepthStencilStance, const unsigned int stencilRef = 0);
		void Draw(const unsigned int vertexCount);
		void DrawIndexed(const unsigned int indexCount);
		void DrawIndexedInstanced(const unsigned int indexCount, const unsigned int instanceCount);
		void Present();

		////////////////////
		// Inline methods
		inline ID3D11Device * GetDevice() const { return m_pDevice.Get(); }
		inline ID3D11DeviceContext * GetContext() const { return m_pContext.Get(); }

		inline RenderTargetView * const GetMainRenderTargetView() const { return m_pMainRenderTarget; }
		inline DepthStencilView * const GetMainDepthStencilView() const { return m_pDepthStencilView; }

		// For easy access
		inline void MakeMainInstance() { m_pMainInstance = this; }
		inline static RendererD3D11 * GetMainInstance() { return m_pMainInstance; }

	private:
		GraphicsResult CreateSizeResources(const unsigned int width, const unsigned int height);

		RendererD3D11(const RendererD3D11 & other) = delete;
		RendererD3D11(RendererD3D11 && other) = delete;
		RendererD3D11 & operator = (const RendererD3D11 & other) = delete;
		RendererD3D11 & operator = (RendererD3D11 && other) = delete;

	private:
		// Renderer's reference object
		static RendererD3D11 * m_pMainInstance;

		// Direct3D 11 Device / Context / SwapChain objects
		ComPtr<ID3D11Device> m_pDevice;
		ComPtr<ID3D11DeviceContext> m_pContext;
		ComPtr<IDXGISwapChain> m_pSwapChain;

		// Main Render Target
		RenderTargetView *m_pMainRenderTarget;
		
		// Main Depth Stencil View
		Texture *m_pDepthStencilViewTexture;
		DepthStencilView *m_pDepthStencilView;

		// Main Viewport
		D3D11_VIEWPORT m_Viewport;

		// MSAA samples
		unsigned int m_SampleCount;
		unsigned int m_QualityLevel;

		// Window Status
		bool m_Windowed;
	};
}
#endif