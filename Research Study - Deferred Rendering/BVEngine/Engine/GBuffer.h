#ifndef GBUFFER_H
#define GBUFFER_H

#include "Texture.h"
#include "ShaderResourceView.h"
#include "RenderTargetView.h"
#include "DepthStencilView.h"
#include "DepthStencilState.h"
#include "RendererD3D11.h"
#include "BVMath.h"

using namespace BVGraphics;
using namespace BVMath;

namespace BVEngine
{
	class GBuffer
	{
	public:
		GBuffer();
		~GBuffer();

		GraphicsResult Create(const unsigned int width, const unsigned int height, const bool createAll = true);
		void Destroy(const bool destroyAll = true);

		void Set();
		void Unset();

		void RenderGBuffers(const BVFloat44 & proj);
		void RenderGBuffer(const unsigned int index, const BVFloat44 & proj);

	private:
		GBuffer(const GBuffer & other) = delete;
		GBuffer(GBuffer && other) = delete;
		GBuffer & operator = (const GBuffer & other) = delete;
		GBuffer & operator = (GBuffer && other) = delete;

	public:
		Texture *m_pDSVTexture;
		ShaderResourceView *m_pDSVSRV;
		DepthStencilView *m_pDSV;
		DepthStencilView *m_pDSVReadOnly;

		Texture *m_pDiffuseTexture;
		ShaderResourceView *m_pDiffuseSRV;
		RenderTargetView *m_pDiffuseRTV;

		Texture *m_pNormalTexture;
		ShaderResourceView *m_pNormalSRV;
		RenderTargetView *m_pNormalRTV;

		Texture *m_pAccumulationTexture;
		ShaderResourceView *m_pAccumulationSRV;
		RenderTargetView *m_pAccumulationRTV;

		Texture *m_pSpecularTexture;
		ShaderResourceView *m_pSpecularSRV;
		RenderTargetView *m_pSpecularRTV;

		DepthStencilState *m_pDSS;

		struct CBProjParams
		{
			BVFloat4 m_ProjParams;
		} m_CBProjParams;
		ConstantBuffer *m_pCBProjParams;

		VertexShader *m_pVSGBufferScreen;
		PixelShader *m_pPSGBufferScreen;

		SamplerState *m_pSamplerTrilinear;

		VertexShader *m_pVSGBufferFullScreen;
		PixelShader *m_pPSGBufferFullScreen;
		PixelShader *m_pPSGBufferDepthFullScreen;
	};
}

#endif