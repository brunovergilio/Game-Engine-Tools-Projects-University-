#ifndef DEFERREDSHADING_H
#define DEFERREDSHADING_H

#include "Light.h"
#include "GBuffer.h"
#include "ShadowMap.h"

using namespace BVGraphics;
using namespace BVMath::SIMD;

namespace BVEngine
{
	class DeferredShading
	{
	public:
		DeferredShading();
		~DeferredShading();

		GraphicsResult Create(const unsigned int width, const unsigned int height, const unsigned int maxInstances = 200U);
		void Destroy();

		void ResizeGBuffer(const unsigned int width, const unsigned int height);

		void SetGBufferPass();
		void UnsetGBufferPass();

		void SetLightPass(CRBVMatrix view, CRBVMatrix proj);
		void UnsetLightPass();

		void RenderLightQuads(Light * const pLights, const unsigned int numLights);
		void RenderLightVolumes(CRBVMatrix view, CRBVMatrix proj, Light * const pLights, const unsigned int numLights);
		void RenderLightVolumesInstanced(CRBVMatrix view, CRBVMatrix proj, Light * const pLights, const unsigned int numLights);

		void RenderLights(const Camera & cam, Light * const pLights, const unsigned int numLights,
			const ShadowMap * const pShadowMap = nullptr, const unsigned int * const pShadowIndices = nullptr);

		void RenderGBuffers(const BVFloat44 & proj);
		void RenderGBuffer(const unsigned int index, const BVFloat44 & proj);

	private:
		DeferredShading(const DeferredShading & other) = delete;
		DeferredShading(DeferredShading && other) = delete;
		DeferredShading & operator = (const DeferredShading & other) = delete;
		DeferredShading & operator = (DeferredShading && other) = delete;

	private:
		// GBuffer
		GBuffer *m_pGBuffer;

		// Models
		VertexBuffer *m_pVBPointLight;
		IndexBuffer *m_pIBPointLight;

		VertexBuffer *m_pVBSpotLight;
		IndexBuffer *m_pIBSpotLight;

		VertexBuffer *m_pVBInstanced;

		// Input Layouts
		InputLayout *m_pInputLayout;
		InputLayout *m_pInputLayoutInstanced;

		// Constant Buffers
		struct CBLightWVP
		{
			BVFloat44 m_WVP;
		} m_CBLightWVP;
		ConstantBuffer *m_pCBLightWVP;

		struct CBLight
		{
			Light m_Light;
		} m_CBLight;
		ConstantBuffer *m_pCBLight;

		struct CBLightParams
		{
			BVFloat4 m_ProjParams;
			BVFloat44 m_InvView;
		} m_CBLightParams;
		ConstantBuffer *m_pCBLightParams;

		struct CBShadowMap
		{
			BVFloat44 m_ShadowMap;
			BVFloat2 m_PointProj;
			float m_ResolutionRCP;
			float m_Pad;
			BVFloat4 m_OffsetsX;
			BVFloat4 m_OffsetsY;
			BVFloat4 m_OffsetsScale;
		} m_CBShadowMap;
		ConstantBuffer *m_pCBShadowMap;

		// Shaders
		VertexShader *m_pVShaderScreenQuad;
		VertexShader *m_pVShaderLightVolume;
		PixelShader *m_pPShaderLight;

		VertexShader *m_pVSLightVolumeInstanced;
		PixelShader *m_pPSLightInstanced;

		// States
		RasterizerState *m_pRSBackCull;
		RasterizerState *m_pRSFrontCullNoClip;
		BlendState *m_pBlendAdd;
		DepthStencilState *m_pDSSLess;
		DepthStencilState *m_pDSSGreaterEqual;
		SamplerState *m_pSamplerClamp;

		struct LightInstance
		{
			BVFloat44 m_WVP;
			Light m_Light;
		} *m_pLightInstances;
		unsigned int m_MaxInstances;

		bool m_UseLightVolume;
	};
}
#endif