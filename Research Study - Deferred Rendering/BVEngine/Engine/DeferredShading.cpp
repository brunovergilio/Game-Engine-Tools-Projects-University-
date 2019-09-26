#include "DeferredShading.h"
#include "Primitive.h"
#include "BVMatrix_SIMD.h"

namespace BVEngine
{
	BVEngine::DeferredShading::DeferredShading()
		: m_pGBuffer(nullptr), m_pVBPointLight(nullptr), m_pIBPointLight(nullptr), m_pVBSpotLight(nullptr), m_pIBSpotLight(nullptr),
		m_pInputLayout(nullptr), m_pCBLightWVP(nullptr), m_pCBLight(nullptr), m_pCBLightParams(nullptr), m_pCBShadowMap(nullptr),
		m_pVShaderScreenQuad(nullptr), m_pVShaderLightVolume(nullptr), m_pPShaderLight(nullptr),
		m_pRSFrontCullNoClip(nullptr), m_pRSBackCull(nullptr),
		m_pBlendAdd(nullptr), m_pDSSLess(nullptr), m_pDSSGreaterEqual(nullptr), m_UseLightVolume(false),
		m_pVBInstanced(nullptr), m_pInputLayoutInstanced(nullptr),
		m_pVSLightVolumeInstanced(nullptr), m_pPSLightInstanced(nullptr), m_pLightInstances(nullptr), m_pSamplerClamp(nullptr)
	{
	}

	BVEngine::DeferredShading::~DeferredShading()
	{
		Destroy();
	}

	GraphicsResult BVEngine::DeferredShading::Create(const unsigned int width, const unsigned int height, const unsigned int maxInstances)
	{
		Destroy();

		m_pGBuffer = new GBuffer();
		m_pVBPointLight = new VertexBuffer();
		m_pIBPointLight = new IndexBuffer();
		m_pVBSpotLight = new VertexBuffer();
		m_pIBSpotLight = new IndexBuffer();
		m_pInputLayout = new InputLayout();
		m_pCBLightWVP = new ConstantBuffer();
		m_pCBLight = new ConstantBuffer();
		m_pCBLightParams = new ConstantBuffer();
		m_pVShaderScreenQuad = new VertexShader();
		m_pVShaderLightVolume = new VertexShader();
		m_pPShaderLight = new PixelShader();
		m_pRSBackCull = new RasterizerState();
		m_pRSFrontCullNoClip = new RasterizerState();
		m_pBlendAdd = new BlendState();
		m_pDSSLess = new DepthStencilState();
		m_pDSSGreaterEqual = new DepthStencilState();

		m_pVBInstanced = new VertexBuffer();
		m_pInputLayoutInstanced = new InputLayout();
		m_pVSLightVolumeInstanced = new VertexShader();
		m_pPSLightInstanced = new PixelShader();
		m_MaxInstances = maxInstances;
		m_pLightInstances = new LightInstance[m_MaxInstances];

		m_pCBShadowMap = new ConstantBuffer();

		m_pSamplerClamp = new SamplerState();

		GraphicsResult result = GraphicsResult::SUCCESS;

		do
		{
			if (GraphicsResult::SUCCESS != (result = m_pGBuffer->Create(width, height)))
			{
				break;
			}

			{
				Primitive prim;
				prim.CreateGeosphere(1.0f, 3);
				Vertex3DLightVolume *pVertices = new Vertex3DLightVolume[prim.m_NumVertices];
				for (size_t i = 0; i < prim.m_NumVertices; i++)
				{
					memcpy(&pVertices[i], &prim.m_pVertices[i], Vertex3DLightVolume::m_VertexSize);
				}
				if (GraphicsResult::HARDWARE_BUFFER_FAIL == (result = m_pVBPointLight->CreateDefault(pVertices, prim.m_NumVertices, Vertex3DLightVolume::m_VertexSize)))
				{
					break;
				}
				delete[] pVertices;

				if (GraphicsResult::HARDWARE_BUFFER_FAIL == (result = m_pIBPointLight->Create(prim.m_pIndices, prim.m_NumIndices)))
				{
					break;
				}

				prim.CreateCone(24, 3, 1.0f, 1.0f, false);
				pVertices = new Vertex3DLightVolume[prim.m_NumVertices];
				for (size_t i = 0; i < prim.m_NumVertices; i++)
				{
					memcpy(&pVertices[i], &prim.m_pVertices[i], Vertex3DLightVolume::m_VertexSize);
				}
				if (GraphicsResult::HARDWARE_BUFFER_FAIL == (result = m_pVBSpotLight->CreateDefault(pVertices, prim.m_NumVertices, Vertex3DLightVolume::m_VertexSize)))
				{
					break;
				}
				delete[] pVertices;

				if (GraphicsResult::HARDWARE_BUFFER_FAIL == (result = m_pIBSpotLight->Create(prim.m_pIndices, prim.m_NumIndices)))
				{
					break;
				}
			}

			if (GraphicsResult::HARDWARE_BUFFER_FAIL == (result = m_pCBLightWVP->CreateDynamic(sizeof(CBLightWVP))))
			{
				break;
			}

			if (GraphicsResult::HARDWARE_BUFFER_FAIL == (result = m_pCBLight->CreateDynamic(sizeof(CBLight))))
			{
				break;
			}

			if (GraphicsResult::HARDWARE_BUFFER_FAIL == (result = m_pCBLightParams->CreateDynamic(sizeof(CBLightParams))))
			{
				break;
			}

			if (GraphicsResult::SHADER_FAIL == (result = m_pVShaderScreenQuad->CompileFromFile("Light.vs.hlsl", "VS_ScreenQuad")))
			{
				break;
			}

			if (GraphicsResult::SHADER_FAIL == (result = m_pVShaderLightVolume->CompileFromFile("Light.vs.hlsl", "VS_LightVolume",
				m_pInputLayout, Vertex3DLightVolume::m_InputLayout, Vertex3DLightVolume::m_InputLayoutSize)))
			{
				break;
			}

			if (GraphicsResult::SHADER_FAIL == (result = m_pPShaderLight->CompileFromFile("Light.ps.hlsl", "PS")))
			{
				break;
			}

			if (GraphicsResult::RASTERIZER_STATE_FAIL == (result = m_pRSFrontCullNoClip->Create(CullMode::FRONT, false, false)))
			{
				break;
			}

			if (GraphicsResult::RASTERIZER_STATE_FAIL == (result = m_pRSBackCull->Create()))
			{
				break;
			}

			D3D11_BLEND_DESC blendDesc;
			blendDesc.AlphaToCoverageEnable = FALSE;
			blendDesc.IndependentBlendEnable = FALSE;
			const D3D11_RENDER_TARGET_BLEND_DESC defaultRenderTargetBlendDesc =
			{
				TRUE,
				D3D11_BLEND_ONE, D3D11_BLEND_ONE, D3D11_BLEND_OP_ADD,
				D3D11_BLEND_ONE, D3D11_BLEND_ONE, D3D11_BLEND_OP_ADD,
				D3D11_COLOR_WRITE_ENABLE_ALL,
			};
			for (UINT i = 0; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i) { blendDesc.RenderTarget[i] = defaultRenderTargetBlendDesc; }
			if (GraphicsResult::BLEND_STATE_FAIL == (result = m_pBlendAdd->Create(blendDesc)))
			{
				break;
			}

			D3D11_DEPTH_STENCIL_DESC dssDesc;
			dssDesc.DepthEnable = TRUE;
			dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
			dssDesc.DepthFunc = D3D11_COMPARISON_LESS;
			dssDesc.StencilEnable = TRUE;
			dssDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
			dssDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
			const D3D11_DEPTH_STENCILOP_DESC noSkyStencilOp =
			{ D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_EQUAL };
			dssDesc.FrontFace = noSkyStencilOp;
			dssDesc.BackFace = noSkyStencilOp;
			if (GraphicsResult::DEPTH_STENCIL_STATE_FAIL == (result = m_pDSSLess->Create(dssDesc)))
			{
				break;
			}

			dssDesc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;
			if (GraphicsResult::DEPTH_STENCIL_STATE_FAIL == (result = m_pDSSGreaterEqual->Create(dssDesc)))
			{
				break;
			}


			if (GraphicsResult::HARDWARE_BUFFER_FAIL == (result = m_pVBInstanced->CreateDynamic(maxInstances, sizeof(LightInstance))))
			{
				break;
			}

			if (GraphicsResult::SHADER_FAIL == (result = m_pVSLightVolumeInstanced->CompileFromFile("LightInstanced.vs.hlsl", "VS_LightVolume",
				m_pInputLayoutInstanced, Vertex3DLightVolume::m_InputLayoutInstanced, Vertex3DLightVolume::m_InputLayoutInstancedSize)))
			{
				break;
			}

			if (GraphicsResult::SHADER_FAIL == (result = m_pPSLightInstanced->CompileFromFile("LightInstanced.ps.hlsl", "PS")))
			{
				break;
			}

			if (GraphicsResult::HARDWARE_BUFFER_FAIL == (result = m_pCBShadowMap->CreateDynamic(sizeof(CBShadowMap))))
			{
				break;
			}

			D3D11_SAMPLER_DESC samplerClampDesc;
			ZeroMemory(&samplerClampDesc, sizeof(D3D11_SAMPLER_DESC));
			samplerClampDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
			samplerClampDesc.AddressU = samplerClampDesc.AddressV = samplerClampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
			samplerClampDesc.MaxAnisotropy = 1;
			samplerClampDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
			samplerClampDesc.MaxLOD = D3D11_FLOAT32_MAX;
			if (GraphicsResult::SAMPLER_STATE_FAIL == (result = m_pSamplerClamp->Create(samplerClampDesc)))
			{
				break;
			}
		} while (0);

		if (GraphicsResult::SUCCESS != result)
		{
			Destroy();
		}

		return result;
	}

	void BVEngine::DeferredShading::Destroy()
	{
		BV_SAFE_DELETE(m_pGBuffer);
		BV_SAFE_DELETE(m_pVBPointLight);
		BV_SAFE_DELETE(m_pIBPointLight);
		BV_SAFE_DELETE(m_pVBSpotLight);
		BV_SAFE_DELETE(m_pIBSpotLight);
		BV_SAFE_DELETE(m_pInputLayout);
		BV_SAFE_DELETE(m_pCBLightWVP);
		BV_SAFE_DELETE(m_pCBLight);
		BV_SAFE_DELETE(m_pCBLightParams);
		BV_SAFE_DELETE(m_pVShaderScreenQuad);
		BV_SAFE_DELETE(m_pVShaderLightVolume);
		BV_SAFE_DELETE(m_pPShaderLight);
		BV_SAFE_DELETE(m_pRSBackCull);
		BV_SAFE_DELETE(m_pRSFrontCullNoClip);
		BV_SAFE_DELETE(m_pBlendAdd);
		BV_SAFE_DELETE(m_pDSSLess);
		BV_SAFE_DELETE(m_pDSSGreaterEqual);

		BV_SAFE_DELETE(m_pVBInstanced);
		BV_SAFE_DELETE(m_pInputLayoutInstanced);
		BV_SAFE_DELETE(m_pVSLightVolumeInstanced);
		BV_SAFE_DELETE(m_pPSLightInstanced);
		BV_SAFE_DELETE(m_pLightInstances);

		BV_SAFE_DELETE(m_pCBShadowMap);

		BV_SAFE_DELETE(m_pSamplerClamp);
	}

	void DeferredShading::ResizeGBuffer(const unsigned int width, const unsigned int height)
	{
		m_pGBuffer->Create(width, height, false);
	}

	void BVEngine::DeferredShading::SetGBufferPass()
	{
		m_pGBuffer->Set();
	}

	void BVEngine::DeferredShading::UnsetGBufferPass()
	{
		m_pGBuffer->Unset();
	}

	void DeferredShading::SetLightPass(CRBVMatrix view, CRBVMatrix proj)
	{
		RendererD3D11 *pRenderer = RendererD3D11::GetMainInstance();
		BV_ASSERT(pRenderer != nullptr, "DeferredShading::SetLightQuadPass => Renderer not initialized or assigned");

		RenderTargetView * const pRTV[] = { pRenderer->GetMainRenderTargetView() };
		pRenderer->SetRenderTargetViews(pRTV, 1, m_pGBuffer->m_pDSVReadOnly);

		{
			BVFloat44 p;
			MatrixToFloat4x4(proj, p);

			m_CBLightParams.m_ProjParams.x = 1.0f / p.r[0].x;
			m_CBLightParams.m_ProjParams.y = 1.0f / p.r[1].y;
			m_CBLightParams.m_ProjParams.z = p.r[3].z;
			m_CBLightParams.m_ProjParams.w = p.r[2].z;
			MatrixToFloat4x4(MatrixInverse(view), m_CBLightParams.m_InvView);
			m_pCBLightParams->Map(&m_CBLightParams, sizeof(CBLightParams));
		}

		const ShaderResourceView * const pTextures[] = { m_pGBuffer->m_pDSVSRV, m_pGBuffer->m_pDiffuseSRV, m_pGBuffer->m_pNormalSRV,
			m_pGBuffer->m_pAccumulationSRV, m_pGBuffer->m_pSpecularSRV };
		ConstantBuffer *pLightBuffers[] = { m_pCBLight, m_pCBLightParams, m_pCBShadowMap };

		pRenderer->SetShader(ShaderType::Pixel, m_pPShaderLight);
		pRenderer->SetConstantBuffers(ShaderType::Pixel, pLightBuffers, 3);
		pRenderer->SetShaderResources(ShaderType::Pixel, pTextures, 5);

		pRenderer->SetBlendState(m_pBlendAdd, nullptr, 0xFFFFFFFF);
	}


	void DeferredShading::UnsetLightPass()
	{
		RendererD3D11 *pRenderer = RendererD3D11::GetMainInstance();
		BV_ASSERT(pRenderer != nullptr, "DeferredShading::UnsetLightVolumePass => Renderer not initialized or assigned");

		pRenderer->SetRasterizerState(m_pRSBackCull);
		pRenderer->SetDepthStencilState(nullptr, 0);
		pRenderer->SetBlendState(nullptr, nullptr, 0xFFFFFFFF);

		pRenderer->SetShaderResources(ShaderType::Pixel, nullptr, 5);
		pRenderer->SetShader(ShaderType::Pixel, nullptr);
	}


	void DeferredShading::RenderLightQuads(Light * const pLights, const unsigned int numLights)
	{
		RendererD3D11 *pRenderer = RendererD3D11::GetMainInstance();
		BV_ASSERT(pRenderer != nullptr, "DeferredShading::RenderLightQuads => Renderer not initialized or assigned");

		pRenderer->SetRasterizerState(m_pRSBackCull);
		pRenderer->SetDepthStencilState(m_pDSSLess, 1);
		pRenderer->SetShader(ShaderType::Vertex, m_pVShaderScreenQuad);
		pRenderer->SetSamplers(ShaderType::Pixel, &m_pSamplerClamp, 1);

		pRenderer->SetPrimitiveTopology(Topology::TriangleStrip);
		pRenderer->SetInputLayout(nullptr);
		pRenderer->SetVertexBuffers(nullptr, 0);
		pRenderer->SetIndexBuffer(nullptr);

		for (unsigned int i = 0; i < numLights; i++)
		{
			if (!pLights[i].m_Enabled)
			{
				continue;
			}

			m_CBLight.m_Light = pLights[i];
			m_pCBLight->Map(&m_CBLight, sizeof(CBLight));

			pRenderer->Draw(4);
		}
	}

	void DeferredShading::RenderLightVolumes(CRBVMatrix view, CRBVMatrix proj, Light * const pLights, const unsigned int numLights)
	{
		RendererD3D11 *pRenderer = RendererD3D11::GetMainInstance();
		BV_ASSERT(pRenderer != nullptr, "DeferredShading::RenderLightVolumes => Renderer not initialized or assigned");

		pRenderer->SetRasterizerState(m_pRSFrontCullNoClip);
		pRenderer->SetDepthStencilState(m_pDSSGreaterEqual, 1);

		pRenderer->SetPrimitiveTopology(Topology::TriangleList);
		pRenderer->SetInputLayout(m_pInputLayout);
		pRenderer->SetShader(ShaderType::Vertex, m_pVShaderLightVolume);
		pRenderer->SetConstantBuffers(ShaderType::Vertex, &m_pCBLightWVP, 1);

		BVMatrix scale;
		BVMatrix rotation;
		BVMatrix translation;
		BVMatrix wvp;
		BVMatrix rotZ;

		for (unsigned int i = 0; i < numLights; i++)
		{
			if (!pLights[i].m_Enabled)
			{
				continue;
			}

			if (LightType::Point == pLights[i].m_LightType)
			{
				scale = MatrixScaling(pLights[i].m_Range, pLights[i].m_Range, pLights[i].m_Range);
				translation = MatrixTranslation(pLights[i].m_Position.x, pLights[i].m_Position.y, pLights[i].m_Position.z);
				wvp = scale * translation * view * proj;
				MatrixToFloat4x4(wvp, m_CBLightWVP.m_WVP);
				m_pCBLightWVP->Map(&m_CBLightWVP, sizeof(CBLightWVP));

				m_CBLight.m_Light = pLights[i];
				m_pCBLight->Map(&m_CBLight, sizeof(CBLight));

				pRenderer->SetVertexBuffers(&m_pVBPointLight, 1);
				pRenderer->SetIndexBuffer(m_pIBPointLight);
				pRenderer->DrawIndexed(m_pIBPointLight->GetNumIndices());
			}
			else
			{
				float radius = tanf(pLights[i].m_CosOuterCone) * pLights[i].m_Range;
				scale = MatrixScaling(radius, pLights[i].m_Range, radius);

				BVVector vDir = VectorNormalize(VectorFromFloat3(pLights[i].m_Direction));

				BVVector vConeOrient = VectorSet(0.0f, -1.0f, 0.0f);
				BVVector vNewOrient = VectorNormalize(vDir - VectorFromFloat3(pLights[i].m_Position));

				VectorToFloat3(vNewOrient, pLights[i].m_Direction);

				rotation = MatrixIdentity();
				if (!VectorIsEqual(vConeOrient, vNewOrient, 0.01f))
				{
					BVVector vCross = VectorNormalize(VectorCross(vConeOrient, vNewOrient));
					rotation = MatrixRotationAxis(vCross, acosf(VectorDot(vConeOrient, vNewOrient)));
				}

				translation = MatrixTranslation(pLights[i].m_Position.x, pLights[i].m_Position.y, pLights[i].m_Position.z);

				wvp = scale * rotation * translation;

				MatrixToFloat4x4(wvp, m_CBLightWVP.m_WVP);
				m_pCBLightWVP->Map(&m_CBLightWVP, sizeof(CBLightWVP));

				m_CBLight.m_Light = pLights[i];
				m_pCBLight->Map(&m_CBLight, sizeof(CBLight));

				pRenderer->SetVertexBuffers(&m_pVBSpotLight, 1);
				pRenderer->SetIndexBuffer(m_pIBSpotLight);
				pRenderer->DrawIndexed(m_pIBSpotLight->GetNumIndices());
			}
		}
	}

	void DeferredShading::RenderLightVolumesInstanced(CRBVMatrix view, CRBVMatrix proj, Light * const pLights, const unsigned int numLights)
	{
		RendererD3D11 *pRenderer = RendererD3D11::GetMainInstance();
		BV_ASSERT(pRenderer != nullptr, "DeferredShading::RenderLightVolumesInstanced => Renderer not initialized or assigned");

		pRenderer->SetRasterizerState(m_pRSFrontCullNoClip);
		pRenderer->SetDepthStencilState(m_pDSSGreaterEqual, 1);

		pRenderer->SetPrimitiveTopology(Topology::TriangleList);
		pRenderer->SetInputLayout(m_pInputLayout);
		pRenderer->SetShader(ShaderType::Vertex, m_pVShaderLightVolume);
		pRenderer->SetConstantBuffers(ShaderType::Vertex, &m_pCBLightWVP, 1);

		BVMatrix scale;
		BVMatrix rotation;
		BVMatrix translation;
		BVMatrix wvp;
		BVMatrix rotZ;

		pRenderer->SetInputLayout(m_pInputLayoutInstanced);
		pRenderer->SetShader(ShaderType::Vertex, m_pVSLightVolumeInstanced);
		pRenderer->SetShader(ShaderType::Pixel, m_pPSLightInstanced);
		pRenderer->SetConstantBuffers(ShaderType::Pixel, &m_pCBLightParams, 1);

		VertexBuffer *pVBs[2];
		if (LightType::Point == pLights[0].m_LightType) // Assume the first one for all the others
		{
			pVBs[0] = m_pVBPointLight;
			pVBs[1] = m_pVBInstanced;
			for (unsigned int i = 0; i < numLights; i++)
			{
				scale = MatrixScaling(pLights[i].m_Range, pLights[i].m_Range, pLights[i].m_Range);
				translation = MatrixTranslation(pLights[i].m_Position.x, pLights[i].m_Position.y, pLights[i].m_Position.z);
				wvp = scale * translation * view * proj;
				MatrixToFloat4x4(wvp, m_pLightInstances[i].m_WVP);
				m_pLightInstances[i].m_Light = pLights[i];
			}
			m_pVBInstanced->Map(m_pLightInstances, sizeof(LightInstance) * m_MaxInstances);

			pRenderer->SetVertexBuffers(pVBs, 2);
			pRenderer->SetIndexBuffer(m_pIBPointLight);
			pRenderer->DrawIndexedInstanced(m_pIBPointLight->GetNumIndices(), numLights);
			pRenderer->SetVertexBuffers(nullptr, 2);
		}
		else
		{
			pVBs[0] = m_pVBSpotLight;
			pVBs[1] = m_pVBInstanced;
			for (unsigned int i = 0; i < numLights; i++)
			{
				float radius = tanf(pLights[i].m_CosOuterCone) * pLights[i].m_Range;
				scale = MatrixScaling(radius, pLights[i].m_Range, radius);

				BVVector vDir = VectorNormalize(VectorFromFloat3(pLights[i].m_Direction));

				BVVector vConeOrient = VectorSet(0.0f, -1.0f, 0.0f);
				BVVector vNewOrient = VectorNormalize(vDir - VectorFromFloat3(pLights[i].m_Position));

				VectorToFloat3(vNewOrient, pLights[i].m_Direction);

				rotation = MatrixIdentity();
				if (!VectorIsEqual(vConeOrient, vNewOrient, 0.01f))
				{
					BVVector vCross = VectorNormalize(VectorCross(vConeOrient, vNewOrient));
					rotation = MatrixRotationAxis(vCross, acosf(VectorDot(vConeOrient, vNewOrient)));
				}

				translation = MatrixTranslation(pLights[i].m_Position.x, pLights[i].m_Position.y, pLights[i].m_Position.z);

				wvp = scale * rotation * translation;
				MatrixToFloat4x4(wvp, m_pLightInstances[i].m_WVP);
				m_pLightInstances[i].m_Light = pLights[i];
			}
			m_pVBInstanced->Map(m_pLightInstances, sizeof(LightInstance) * m_MaxInstances);

			pRenderer->SetVertexBuffers(pVBs, 2);
			pRenderer->SetIndexBuffer(m_pIBSpotLight);
			pRenderer->DrawIndexedInstanced(m_pIBSpotLight->GetNumIndices(), numLights);
			pRenderer->SetVertexBuffers(nullptr, 2);
		}
	}


	void BVEngine::DeferredShading::RenderLights(const Camera & cam, Light * const pLights, const unsigned int numLights,
		const ShadowMap * const pShadowMap, const unsigned int * const pShadowIndices)
	{
		RendererD3D11 *pRenderer = RendererD3D11::GetMainInstance();
		BV_ASSERT(pRenderer != nullptr, "DeferredShading::RenderLights => Renderer not initialized or assigned");

		RenderTargetView * const pRTV[] = { pRenderer->GetMainRenderTargetView() };
		pRenderer->SetRenderTargetViews(pRTV, 1, m_pGBuffer->m_pDSVReadOnly);

		BVMatrix view = cam.GetViewM();
		BVMatrix proj = cam.GetProjM();

		{
			BVFloat44 p;
			MatrixToFloat4x4(proj, p);

			m_CBLightParams.m_ProjParams.x = 1.0f / p.r[0].x;
			m_CBLightParams.m_ProjParams.y = 1.0f / p.r[1].y;
			m_CBLightParams.m_ProjParams.z = p.r[3].z;
			m_CBLightParams.m_ProjParams.w = p.r[2].z;
			MatrixToFloat4x4(MatrixInverse(view), m_CBLightParams.m_InvView);
			m_pCBLightParams->Map(&m_CBLightParams, sizeof(CBLightParams));
		}

		ConstantBuffer *pLightBuffers[] = { m_pCBLight, m_pCBLightParams, m_pCBShadowMap };

		pRenderer->SetShader(ShaderType::Pixel, m_pPShaderLight);
		pRenderer->SetConstantBuffers(ShaderType::Pixel, pLightBuffers, 3);

		pRenderer->SetBlendState(m_pBlendAdd, nullptr, 0xFFFFFFFF);

		pRenderer->SetSamplers(ShaderType::Pixel, &m_pSamplerClamp, 1);

		BVMatrix scale;
		BVMatrix rotation;
		BVMatrix translation;
		BVMatrix wvp;
		BVMatrix rotZ;
		for (unsigned int i = 0; i < numLights; i++)
		{
			if (!pLights[i].m_Enabled)
			{
				continue;
			}

			const ShaderResourceView * pTextures[8] = { m_pGBuffer->m_pDSVSRV, m_pGBuffer->m_pDiffuseSRV, m_pGBuffer->m_pNormalSRV,
				m_pGBuffer->m_pAccumulationSRV, m_pGBuffer->m_pSpecularSRV, nullptr, nullptr, nullptr };

			if (LightType::Directional == pLights[i].m_LightType || LightType::Hemispheric == pLights[i].m_LightType)
			{
				if (LightType::Directional == pLights[i].m_LightType && pLights[i].m_CastsShadow && pShadowMap && pShadowIndices)
				{
					float nZ = cam.GetNearZ();
					float fZ = cam.GetFarZ();
					float fov = cam.GetFovY();
					float ar = cam.GetAspectRatio();
					BVMatrix invView = cam.GetInvViewM();
					BVVector vCamPos = invView.r[3];
					BVVector vCamLook = invView.r[2];
					BVVector vCamUp = invView.r[1];
					BVVector vCamRight = invView.r[0];

					// Shadow Matrix
					BVMatrix lightView;
					{
						BVVector vPos = vCamPos + vCamLook * fZ * 0.5f;
						BVVector vLook = vPos + VectorFromFloat3(pLights[i].m_Direction) * fZ;

						BVVector vUp = VectorSet(0.0f, 1.0f, 0.0f);
						if (VectorIsEqual(vLook, vUp, 0.01f))
						{
							vUp = VectorSet(0.0f, 0.0f, -1.0f);
						}
						lightView = MatrixLookAtLH(vPos, vLook, vUp);
					}

					const float fTanFOVX = tanf(ar * fov * 0.5f);
					const float fTanFOVY = tanf(ar);

					BVVector vCenter = vCamPos + vCamLook * (nZ + 0.5f * (nZ + fZ));
					float radius = VectorLength(vCamPos + (-vCamRight * fTanFOVX + vCamUp * fTanFOVY + vCamLook) * fZ - vCenter);

					BVMatrix lightViewProj = lightView * MatrixOrthographicLH_DX(radius, radius, -radius, radius);

					if (pShadowMap)
					{
						MatrixToFloat4x4(lightViewProj, m_CBShadowMap.m_ShadowMap);
						m_CBShadowMap.m_ResolutionRCP = 1.0f / (float)pShadowMap->GetResolution();
						m_CBShadowMap.m_OffsetsX = pShadowMap->GetCascadeOffsetsX();
						m_CBShadowMap.m_OffsetsY = pShadowMap->GetCascadeOffsetsY();
						m_CBShadowMap.m_OffsetsScale = pShadowMap->GetCascadeOffsetsScale();

						m_pCBShadowMap->Map(&m_CBShadowMap, sizeof(CBShadowMap));
					}

					pTextures[7] = pShadowMap ? pShadowMap->GetDirSRVs()[pShadowIndices[i]] : nullptr;
				}

				pRenderer->SetShaderResources(ShaderType::Pixel, pTextures, 8);

				pRenderer->SetRasterizerState(m_pRSBackCull);
				pRenderer->SetDepthStencilState(m_pDSSLess, 1);
				pRenderer->SetShader(ShaderType::Vertex, m_pVShaderScreenQuad);

				pRenderer->SetPrimitiveTopology(Topology::TriangleStrip);
				pRenderer->SetInputLayout(nullptr);
				pRenderer->SetVertexBuffers(nullptr, 0);
				pRenderer->SetIndexBuffer(nullptr);

				m_CBLight.m_Light = pLights[i];
				m_pCBLight->Map(&m_CBLight, sizeof(CBLight));

				pRenderer->Draw(4);
			}
			else
			{
				pRenderer->SetRasterizerState(m_pRSFrontCullNoClip);
				pRenderer->SetDepthStencilState(m_pDSSGreaterEqual, 1);

				pRenderer->SetPrimitiveTopology(Topology::TriangleList);
				pRenderer->SetInputLayout(m_pInputLayout);

				pRenderer->SetShader(ShaderType::Vertex, m_pVShaderLightVolume);
				pRenderer->SetConstantBuffers(ShaderType::Vertex, &m_pCBLightWVP, 1);

				if (LightType::Point == pLights[i].m_LightType)
				{
					if (pShadowMap)
					{
						BVMatrix pointProj = MatrixPerspectiveLH_DX(0.01f, pLights[i].m_Range, 1.0f, PI_OVER_TWO);
						MatrixToFloat4x4(pointProj, m_CBShadowMap.m_ShadowMap);
						m_CBShadowMap.m_PointProj.x = m_CBShadowMap.m_ShadowMap.r[2].z;
						m_CBShadowMap.m_PointProj.y = m_CBShadowMap.m_ShadowMap.r[3].z;
						m_CBShadowMap.m_ResolutionRCP = 1.0f / (float)pShadowMap->GetResolution();
						m_pCBShadowMap->Map(&m_CBShadowMap, sizeof(CBShadowMap));
						pTextures[6] = pShadowMap ? pShadowMap->GetPointSRVs()[pShadowIndices[i]] : nullptr;
					}

					pRenderer->SetShaderResources(ShaderType::Pixel, pTextures, 8);

					scale = MatrixScaling(pLights[i].m_Range, pLights[i].m_Range, pLights[i].m_Range);
					translation = MatrixTranslation(pLights[i].m_Position.x, pLights[i].m_Position.y, pLights[i].m_Position.z);
					wvp = scale * translation * view * proj;
					MatrixToFloat4x4(wvp, m_CBLightWVP.m_WVP);
					m_pCBLightWVP->Map(&m_CBLightWVP, sizeof(CBLightWVP));

					m_CBLight.m_Light = pLights[i];
					m_pCBLight->Map(&m_CBLight, sizeof(CBLight));

					pRenderer->SetVertexBuffers(&m_pVBPointLight, 1);
					pRenderer->SetIndexBuffer(m_pIBPointLight);
					pRenderer->DrawIndexed(m_pIBPointLight->GetNumIndices());
				}
				else
				{
					BVVector lightPos = VectorFromFloat3(pLights[i].m_Position);
					BVVector lightDir = VectorFromFloat3(pLights[i].m_Direction);
					BVVector lightUp = VectorSet(0.0f, 1.0f, 0.0f);
					if (VectorIsEqual(lightDir, lightUp, 0.01f))
					{
						lightUp = VectorSet(0.0f, 0.0f, -1.0f);
					}

					if (pShadowMap)
					{
						BVMatrix lightVP = MatrixLookAtLH(lightPos, lightDir, lightUp) *
							MatrixPerspectiveLH_DX(1.0f, pLights[i].m_Range, 1.0f, acosf(pLights[i].m_CosOuterCone) * 2.0f);
						MatrixToFloat4x4(lightVP, m_CBShadowMap.m_ShadowMap);
						m_CBShadowMap.m_ResolutionRCP = 1.0f / (float)pShadowMap->GetResolution();
						m_pCBShadowMap->Map(&m_CBShadowMap, sizeof(CBShadowMap));
						pTextures[5] = pShadowMap ? pShadowMap->GetSpotSRVs()[pShadowIndices[i]] : nullptr;
					}

					pRenderer->SetShaderResources(ShaderType::Pixel, pTextures, 8);

					float radius = tanf(pLights[i].m_CosOuterCone) * pLights[i].m_Range;
					scale = MatrixScaling(radius, pLights[i].m_Range, radius);

					BVVector vDir = VectorNormalize(VectorFromFloat3(pLights[i].m_Direction));

					BVVector vConeOrient = VectorSet(0.0f, -1.0f, 0.0f);
					BVVector vNewOrient = VectorNormalize(vDir - VectorFromFloat3(pLights[i].m_Position));

					VectorToFloat3(vNewOrient, pLights[i].m_Direction);

					if (!VectorIsEqual(vConeOrient, vNewOrient, 0.01f))
					{
						BVVector vCross = VectorNormalize(VectorCross(vConeOrient, vNewOrient));
						rotation = MatrixRotationAxis(vCross, acosf(VectorDot(vConeOrient, vNewOrient)));
					}
					else
					{
						rotation = MatrixIdentity();
					}

					translation = MatrixTranslation(pLights[i].m_Position.x, pLights[i].m_Position.y, pLights[i].m_Position.z);

					wvp = scale * rotation * translation;

					MatrixToFloat4x4(wvp, m_CBLightWVP.m_WVP);
					m_pCBLightWVP->Map(&m_CBLightWVP, sizeof(CBLightWVP));

					m_CBLight.m_Light = pLights[i];
					m_pCBLight->Map(&m_CBLight, sizeof(CBLight));

					pRenderer->SetVertexBuffers(&m_pVBSpotLight, 1);
					pRenderer->SetIndexBuffer(m_pIBSpotLight);
					pRenderer->DrawIndexed(m_pIBSpotLight->GetNumIndices());
				}
			}
		}

		pRenderer->SetRasterizerState(m_pRSBackCull);
		pRenderer->SetDepthStencilState(nullptr, 0);
		pRenderer->SetBlendState(nullptr, nullptr, 0xFFFFFFFF);
		
		pRenderer->SetShaderResources(ShaderType::Pixel, nullptr, 8);
		pRenderer->SetShader(ShaderType::Pixel, nullptr);
	}

	void BVEngine::DeferredShading::RenderGBuffers(const BVFloat44 & proj)
	{
		m_pGBuffer->RenderGBuffers(proj);
	}

	void DeferredShading::RenderGBuffer(const unsigned int index, const BVFloat44 & proj)
	{
		m_pGBuffer->RenderGBuffer(index, proj);
	}
}