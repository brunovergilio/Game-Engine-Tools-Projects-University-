#ifndef SHADOWMAP_H
#define SHADOWMAP_H

#include "RendererD3D11.h"
#include "BVMath.h"
#include "Light.h"
#include "Camera.h"

using namespace BVGraphics;
using namespace BVMath::SIMD;

namespace BVEngine
{
#define MAX_CASCADES 4U
	class ShadowMap
	{
	public:
		ShadowMap();
		~ShadowMap();

		GraphicsResult Create(const unsigned int resolution, const unsigned int maxPointShadows, const unsigned int maxSpotShadows,
			const unsigned int maxDirShadows);
		void Destroy();

		void SetSpotShadowPass(const unsigned int index);
		void UnsetSpotShadowPass();

		void SetPointShadowPass(const unsigned int index, const Light & light);
		void UnsetPointShadowPass();
		void SetPointShadowParams(CRBVMatrix world);

		void SetDirShadowPass(const Camera & cam, const Light & light, const unsigned int index);
		void UnsetDirShadowPass();
		void SetDirShadowParams(CRBVMatrix world);

		void RenderPointShadowMap(const unsigned int index);
		void RenderSpotShadowMap(const unsigned int index);

		inline const ShaderResourceView * const * const GetPointSRVs() const { return m_pPointSRVs; }
		inline const unsigned int GetMaxPointShadows() const { return m_MaxPointShadows; }

		inline const ShaderResourceView * const * const GetSpotSRVs() const { return m_pSpotSRVs; }
		inline const unsigned int GetMaxSpotShadows() const { return m_MaxSpotShadows; }

		inline const ShaderResourceView * const * const GetDirSRVs() const { return m_pDirSRVs; }
		inline const unsigned int GetMaxDirShadows() const { return m_MaxDirShadows; }

		inline const BVFloat4 GetCascadeOffsetsX() const { return m_OffsetsX; }
		inline const BVFloat4 GetCascadeOffsetsY() const { return m_OffsetsY; }
		inline const BVFloat4 GetCascadeOffsetsScale() const { return m_OffsetsScale; }

		inline const unsigned int GetResolution() const { return m_Resolution; }

	private:
		ShadowMap(const ShadowMap & other) = delete;
		ShadowMap(ShadowMap && other) = delete;
		ShadowMap & operator = (const ShadowMap & other) = delete;
		ShadowMap & operator = (ShadowMap && other) = delete;

	private:
		unsigned int m_Resolution;

		unsigned int m_MaxPointShadows;
		Texture **m_pPointTextures;
		ShaderResourceView **m_pPointSRVs;
		DepthStencilView **m_pPointDSVs;

		unsigned int m_MaxSpotShadows;
		Texture **m_pSpotTextures;
		ShaderResourceView **m_pSpotSRVs;
		DepthStencilView **m_pSpotDSVs;

		unsigned int m_MaxDirShadows;
		BVFloat4 m_OffsetsX;
		BVFloat4 m_OffsetsY;
		BVFloat4 m_OffsetsScale;
		Texture **m_pDirTextures;
		ShaderResourceView **m_pDirSRVs;
		DepthStencilView **m_pDirDSVs;

		struct CBCascadeParams
		{
			BVFloat44 m_CascadeMatrices[MAX_CASCADES];
		} m_CBCascadeParams;
		ConstantBuffer *m_pCBCascadeParams;

		VertexShader *m_pVSSpot;
		VertexShader *m_pVSPoint;
		GeometryShader *m_pGSPoint;
		GeometryShader *m_pGSDir;

		struct CBObject
		{
			BVFloat44 m_WVP;
		} m_CBObject;
		ConstantBuffer *m_pCBObject;

		struct CBShadowParams
		{
			BVFloat44 m_ViewProj[6];
		} m_CBShadowParams;
		ConstantBuffer *m_pCBShadowParams;

		SamplerState *m_pSamplerTrilinear;
		VertexShader *m_pVSScreen;
		PixelShader *m_pPSScreen;

		VertexShader *m_pVSCubeScreen;
		PixelShader *m_pPSCubeScreen;

		DepthStencilState *m_pShadowDSS;
	};
}
#endif