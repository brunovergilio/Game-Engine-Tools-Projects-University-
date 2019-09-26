#include "ShadowMap.h"
#include "BVMatrix_SIMD.h"

using namespace BVMath::SIMD;

namespace BVEngine
{
	ShadowMap::ShadowMap()
		: m_Resolution(0U), m_MaxPointShadows(0U), m_pPointTextures(nullptr), m_pPointSRVs(nullptr), m_pPointDSVs(nullptr),
		m_MaxSpotShadows(0U), m_pSpotTextures(nullptr), m_pSpotSRVs(nullptr), m_pSpotDSVs(nullptr),
		m_pVSSpot(nullptr), m_pVSPoint(nullptr), m_pGSPoint(nullptr), m_pCBObject(nullptr), m_pCBShadowParams(nullptr),
		m_pVSScreen(nullptr), m_pPSScreen(nullptr), m_pSamplerTrilinear(nullptr), m_pShadowDSS(nullptr),
		m_pVSCubeScreen(nullptr), m_pPSCubeScreen(nullptr), m_pDirTextures(nullptr), m_pDirSRVs(nullptr), m_pDirDSVs(nullptr),
		m_pGSDir(nullptr), m_MaxDirShadows(0U), m_pCBCascadeParams(nullptr)
	{
	}


	ShadowMap::~ShadowMap()
	{
		Destroy();
	}


	GraphicsResult ShadowMap::Create(const unsigned int resolution, const unsigned int maxPointShadows, const unsigned int maxSpotShadows,
		const unsigned int maxDirShadows)
	{
		Destroy();

		m_Resolution = resolution;
		m_MaxPointShadows = maxPointShadows;
		m_MaxSpotShadows = maxSpotShadows;

		m_MaxDirShadows = maxDirShadows;

		RendererD3D11 *pRenderer = RendererD3D11::GetMainInstance();
		BV_ASSERT(pRenderer != nullptr, "ShadowMap::Create => Renderer not initialized or assigned");

		GraphicsResult result = GraphicsResult::SUCCESS;

		// Texture description for the buffers
		D3D11_TEXTURE2D_DESC textureDesc = { 0 };
		textureDesc.Width = resolution;
		textureDesc.Height = resolution;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_R32_TYPELESS;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;

		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
		ZeroMemory(&depthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
		depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
		ZeroMemory(&shaderResourceViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		shaderResourceViewDesc.Format = DXGI_FORMAT_R32_FLOAT;
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		shaderResourceViewDesc.Texture2D.MipLevels = 1;

		do
		{
			// Spot light shadows
			m_pSpotTextures = new Texture*[m_MaxSpotShadows]();
			m_pSpotSRVs = new ShaderResourceView*[m_MaxSpotShadows]();
			m_pSpotDSVs = new DepthStencilView*[m_MaxSpotShadows]();
			for (unsigned int i = 0; i < m_MaxSpotShadows; i++)
			{
				m_pSpotTextures[i] = new Texture();
				if (GraphicsResult::TEXTURE_FAIL == (result = m_pSpotTextures[i]->Create2D(textureDesc)))
				{
					break;
				}

				m_pSpotSRVs[i] = new ShaderResourceView();
				if (GraphicsResult::SHADER_RESOURCE_VIEW_FAIL == (result = m_pSpotSRVs[i]->Create(m_pSpotTextures[i]->Get2D(), shaderResourceViewDesc)))
				{
					break;
				}

				m_pSpotDSVs[i] = new DepthStencilView();
				if (GraphicsResult::DEPTH_STENCIL_VIEW_FAIL == (result = m_pSpotDSVs[i]->Create(m_pSpotTextures[i]->Get2D(), depthStencilViewDesc)))
				{
					break;
				}
			}

			textureDesc.ArraySize = 6;
			textureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

			depthStencilViewDesc.Texture2DArray.ArraySize = 6;
			depthStencilViewDesc.Texture2DArray.FirstArraySlice = 0;
			depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;

			shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
			shaderResourceViewDesc.TextureCube.MipLevels = 1;
			shaderResourceViewDesc.TextureCube.MostDetailedMip = 0;

			// Point light shadows
			m_pPointTextures = new Texture*[m_MaxPointShadows]();
			m_pPointSRVs = new ShaderResourceView*[m_MaxPointShadows]();
			m_pPointDSVs = new DepthStencilView*[m_MaxPointShadows]();
			for (unsigned int i = 0; i < m_MaxPointShadows; i++)
			{
				m_pPointTextures[i] = new Texture();
				if (GraphicsResult::TEXTURE_FAIL == (result = m_pPointTextures[i]->Create2D(textureDesc)))
				{
					break;
				}

				m_pPointSRVs[i] = new ShaderResourceView();
				if (GraphicsResult::SHADER_RESOURCE_VIEW_FAIL == (result = m_pPointSRVs[i]->Create(m_pPointTextures[i]->Get2D(), shaderResourceViewDesc)))
				{
					break;
				}

				m_pPointDSVs[i] = new DepthStencilView();
				if (GraphicsResult::DEPTH_STENCIL_VIEW_FAIL == (result = m_pPointDSVs[i]->Create(m_pPointTextures[i]->Get2D(), depthStencilViewDesc)))
				{
					break;
				}
			}

			// Directional light shadows
			textureDesc.ArraySize = MAX_CASCADES;
			textureDesc.MiscFlags = 0;

			shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
			shaderResourceViewDesc.Texture2DArray.FirstArraySlice = 0;
			shaderResourceViewDesc.Texture2DArray.ArraySize = MAX_CASCADES;

			depthStencilViewDesc.Texture2DArray.ArraySize = MAX_CASCADES;

			m_pDirTextures = new Texture*[m_MaxDirShadows]();
			m_pDirSRVs = new ShaderResourceView*[m_MaxDirShadows]();
			m_pDirDSVs = new DepthStencilView*[m_MaxDirShadows]();
			for (unsigned int i = 0; i < m_MaxDirShadows; i++)
			{
				m_pDirTextures[i] = new Texture();
				if (GraphicsResult::TEXTURE_FAIL == (result = m_pDirTextures[i]->Create2D(textureDesc)))
				{
					break;
				}

				m_pDirSRVs[i] = new ShaderResourceView();
				if (GraphicsResult::SHADER_RESOURCE_VIEW_FAIL == (result = m_pDirSRVs[i]->Create(m_pDirTextures[i]->Get2D(), shaderResourceViewDesc)))
				{
					break;
				}

				m_pDirDSVs[i] = new DepthStencilView();
				if (GraphicsResult::DEPTH_STENCIL_VIEW_FAIL == (result = m_pDirDSVs[i]->Create(m_pDirTextures[i]->Get2D(), depthStencilViewDesc)))
				{
					break;
				}
			}

			m_pVSSpot = new VertexShader();
			if (GraphicsResult::SHADER_FAIL == (result = m_pVSSpot->CompileFromFile("ShadowMap.vs.hlsl", "VS_PosOnly")))
			{
				break;
			}

			m_pVSPoint = new VertexShader();
			if (GraphicsResult::SHADER_FAIL == (result = m_pVSPoint->CompileFromFile("ShadowMap.vs.hlsl", "VS_WorldPosOnly")))
			{
				break;
			}

			m_pGSPoint = new GeometryShader();
			if (GraphicsResult::SHADER_FAIL == (result = m_pGSPoint->CompileFromFile("ShadowMap.gs.hlsl", "GS")))
			{
				break;
			}

			m_pGSDir = new GeometryShader();
			if (GraphicsResult::SHADER_FAIL == (result = m_pGSDir->CompileFromFile("CascadeShadowMap.gs.hlsl", "GS")))
			{
				break;
			}

			m_pCBCascadeParams = new ConstantBuffer();
			if (GraphicsResult::HARDWARE_BUFFER_FAIL == (result = m_pCBCascadeParams->CreateDynamic(sizeof(CBCascadeParams))))
			{
				break;
			}

			m_pCBObject = new ConstantBuffer();
			if (GraphicsResult::HARDWARE_BUFFER_FAIL == (result = m_pCBObject->CreateDynamic(sizeof(CBObject))))
			{
				break;
			}

			m_pCBShadowParams = new ConstantBuffer();
			if (GraphicsResult::HARDWARE_BUFFER_FAIL == (result = m_pCBShadowParams->CreateDynamic(sizeof(CBShadowParams))))
			{
				break;
			}

			m_pVSScreen = new VertexShader();
			if (GraphicsResult::SHADER_FAIL == (result = m_pVSScreen->CompileFromFile("ShadowMapMainScreen.vs.hlsl", "VS")))
			{
				break;
			}

			m_pPSScreen = new PixelShader();
			if (GraphicsResult::SHADER_FAIL == (result = m_pPSScreen->CompileFromFile("ShadowMapMainScreen.ps.hlsl", "PS")))
			{
				break;
			}

			m_pVSCubeScreen = new VertexShader();
			if (GraphicsResult::SHADER_FAIL == (result = m_pVSCubeScreen->CompileFromFile("CubeMapMainScreen.vs.hlsl", "VS")))
			{
				break;
			}

			m_pPSCubeScreen = new PixelShader();
			if (GraphicsResult::SHADER_FAIL == (result = m_pPSCubeScreen->CompileFromFile("CubeMapMainScreen.ps.hlsl", "PS")))
			{
				break;
			}

			D3D11_SAMPLER_DESC samplerDesc;
			ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));
			samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.MinLOD = 0.0f;
			samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
			samplerDesc.MaxAnisotropy = 1;
			samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
			m_pSamplerTrilinear = new SamplerState();
			if (GraphicsResult::SAMPLER_STATE_FAIL == (result = m_pSamplerTrilinear->Create(samplerDesc)))
			{
				break;
			}

			D3D11_DEPTH_STENCIL_DESC dssDesc;
			dssDesc.DepthEnable = TRUE;
			dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			dssDesc.DepthFunc = D3D11_COMPARISON_LESS;
			dssDesc.StencilEnable = FALSE;
			dssDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
			dssDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
			const D3D11_DEPTH_STENCILOP_DESC noStencil =
			{ D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_EQUAL };
			dssDesc.FrontFace = noStencil;
			dssDesc.BackFace = noStencil;
			m_pShadowDSS = new DepthStencilState();
			if (GraphicsResult::DEPTH_STENCIL_STATE_FAIL == (result = m_pShadowDSS->Create(dssDesc)))
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


	void ShadowMap::Destroy()
	{
		for (unsigned int i = 0; i < m_MaxPointShadows; i++)
		{
			BV_SAFE_DELETE(m_pPointDSVs[i]);
			BV_SAFE_DELETE(m_pPointSRVs[i]);
			BV_SAFE_DELETE(m_pPointTextures[i]);
		}

		BV_SAFE_DELETE(m_pPointDSVs);
		BV_SAFE_DELETE(m_pPointSRVs);
		BV_SAFE_DELETE(m_pPointTextures);

		for (unsigned int i = 0; i < m_MaxSpotShadows; i++)
		{
			BV_SAFE_DELETE(m_pSpotDSVs[i]);
			BV_SAFE_DELETE(m_pSpotSRVs[i]);
			BV_SAFE_DELETE(m_pSpotTextures[i]);
		}

		BV_SAFE_DELETE(m_pSpotDSVs);
		BV_SAFE_DELETE(m_pSpotSRVs);
		BV_SAFE_DELETE(m_pSpotTextures);

		BV_SAFE_DELETE(m_pVSSpot);
		BV_SAFE_DELETE(m_pVSPoint);
		BV_SAFE_DELETE(m_pGSPoint);
		BV_SAFE_DELETE(m_pCBObject);
		BV_SAFE_DELETE(m_pCBShadowParams);

		BV_SAFE_DELETE(m_pVSScreen);
		BV_SAFE_DELETE(m_pPSScreen);
		BV_SAFE_DELETE(m_pSamplerTrilinear);

		BV_SAFE_DELETE(m_pVSCubeScreen);
		BV_SAFE_DELETE(m_pPSCubeScreen);

		BV_SAFE_DELETE(m_pShadowDSS);

		// Cascaded Shadow Maps
		BV_SAFE_DELETE(m_pCBCascadeParams);

		for (unsigned int i = 0; i < m_MaxDirShadows; i++)
		{
			BV_SAFE_DELETE(m_pDirDSVs[i]);
			BV_SAFE_DELETE(m_pDirSRVs[i]);
			BV_SAFE_DELETE(m_pDirTextures[i]);
		}

		BV_SAFE_DELETE(m_pDirDSVs);
		BV_SAFE_DELETE(m_pDirSRVs);
		BV_SAFE_DELETE(m_pDirTextures);

		BV_SAFE_DELETE(m_pGSDir);
	}


	void ShadowMap::SetSpotShadowPass(const unsigned int index)
	{
		RendererD3D11 *pRenderer = RendererD3D11::GetMainInstance();
		BV_ASSERT(pRenderer != nullptr, "ShadowMap::SetSpotShadowPass => Renderer not initialized or assigned");

		pRenderer->SetShader(ShaderType::Vertex, m_pVSSpot);
		pRenderer->SetShader(ShaderType::Pixel, nullptr);

		RenderTargetView *pNullTarget[] = { nullptr };
		pRenderer->ClearDepthStencilView(m_pSpotDSVs[index], true, 1.0f, false);
		pRenderer->SetRenderTargetViews(pNullTarget, 1, m_pSpotDSVs[index]);

		D3D11_VIEWPORT vp[1] = { { 0, 0, (float)m_Resolution, (float)m_Resolution, 0.0f, 1.0f } };
		pRenderer->GetContext()->RSSetViewports(1, vp);
	}


	void ShadowMap::UnsetSpotShadowPass()
	{
		RendererD3D11 *pRenderer = RendererD3D11::GetMainInstance();
		BV_ASSERT(pRenderer != nullptr, "ShadowMap::UnsetSpotShadowPass => Renderer not initialized or assigned");

		pRenderer->SetMainViewPort();
	}

	void ShadowMap::SetPointShadowPass(const unsigned int index, const Light & light)
	{
		RendererD3D11 *pRenderer = RendererD3D11::GetMainInstance();
		BV_ASSERT(pRenderer != nullptr, "ShadowMap::SetPointShadowPass => Renderer not initialized or assigned");

		pRenderer->SetDepthStencilState(m_pShadowDSS);

		//pRenderer->SetShader(ShaderType::Vertex, m_pVSPoint);
		pRenderer->SetShader(ShaderType::Vertex, m_pVSSpot);
		pRenderer->SetShader(ShaderType::Geometry, m_pGSPoint);
		pRenderer->SetShader(ShaderType::Pixel, nullptr);

		RenderTargetView *pNullTarget[] = { nullptr };
		pRenderer->ClearDepthStencilView(m_pPointDSVs[index], true, 1.0f, false);
		pRenderer->SetRenderTargetViews(pNullTarget, 1, m_pPointDSVs[index]);

		BVMatrix proj = MatrixPerspectiveLH_DX(0.01f, light.m_Range, 1.0f, PI_OVER_TWO);
		BVMatrix views[6];
		BVVector pos = VectorFromFloat3(light.m_Position);
		views[0] = MatrixLookAt(pos, VectorSet(1.0f, 0.0f, 0.0f), VectorSet(0.0f, 1.0f, 0.0f));
		views[1] = MatrixLookAt(pos, VectorSet(-1.0f, 0.0f, 0.0f), VectorSet(0.0f, 1.0f, 0.0f));
		views[2] = MatrixLookAt(pos, VectorSet(0.0f, 1.0f, 0.0f), VectorSet(0.0f, 0.0f, -1.0f));
		views[3] = MatrixLookAt(pos, VectorSet(0.0f, -1.0f, 0.0f), VectorSet(0.0f, 0.0f, 1.0f));
		views[4] = MatrixLookAt(pos, VectorSet(0.0f, 0.0f, 1.0f), VectorSet(0.0f, 1.0f, 0.0f));
		views[5] = MatrixLookAt(pos, VectorSet(0.0f, 0.0f, -1.0f), VectorSet(0.0f, 1.0f, 0.0f));

		for (int i = 0; i < 6; i++)
		{
			MatrixToFloat4x4(views[i] * proj, m_CBShadowParams.m_ViewProj[i]);
		}
		m_pCBShadowParams->Map(&m_CBShadowParams, sizeof(CBShadowParams));

		pRenderer->SetConstantBuffers(ShaderType::Geometry, &m_pCBShadowParams, 1);

		D3D11_VIEWPORT vp[6] = {
			{ 0, 0, (float)m_Resolution, (float)m_Resolution, 0.0f, 1.0f },
			{ 0, 0, (float)m_Resolution, (float)m_Resolution, 0.0f, 1.0f },
			{ 0, 0, (float)m_Resolution, (float)m_Resolution, 0.0f, 1.0f },
			{ 0, 0, (float)m_Resolution, (float)m_Resolution, 0.0f, 1.0f },
			{ 0, 0, (float)m_Resolution, (float)m_Resolution, 0.0f, 1.0f },
			{ 0, 0, (float)m_Resolution, (float)m_Resolution, 0.0f, 1.0f }
		};
		pRenderer->GetContext()->RSSetViewports(6, vp);
	}


	void ShadowMap::UnsetPointShadowPass()
	{
		RendererD3D11 *pRenderer = RendererD3D11::GetMainInstance();
		BV_ASSERT(pRenderer != nullptr, "ShadowMap::UnsetPointShadowPass => Renderer not initialized or assigned");

		pRenderer->SetDepthStencilState(nullptr);

		pRenderer->SetConstantBuffers(ShaderType::Geometry, nullptr, 1);
		pRenderer->SetShader(ShaderType::Geometry, nullptr);

		pRenderer->SetMainViewPort();
	}

	void ShadowMap::SetPointShadowParams(CRBVMatrix world)
	{
		RendererD3D11 *pRenderer = RendererD3D11::GetMainInstance();
		BV_ASSERT(pRenderer != nullptr, "ShadowMap::SetPointShadowParams => Renderer not initialized or assigned");

		MatrixToFloat4x4(world, m_CBObject.m_WVP);
		m_pCBObject->Map(&m_CBObject, sizeof(CBObject));
		pRenderer->SetConstantBuffers(ShaderType::Vertex, &m_pCBObject, 1);
	}

	void ShadowMap::SetDirShadowPass(const Camera & cam, const Light & light, const unsigned int index)
	{
		RendererD3D11 *pRenderer = RendererD3D11::GetMainInstance();
		BV_ASSERT(pRenderer != nullptr, "ShadowMap::SetDirShadowPass => Renderer not initialized or assigned");

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
			BVVector vLook = vPos + VectorFromFloat3(light.m_Direction) * fZ;

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

		float cascadeRanges[MAX_CASCADES + 1] = { nZ, fZ * 0.25f, fZ * 0.5f, fZ * 0.75f, fZ * 1.0f };
		BVVector vFrustumCorners[8];
		BVMatrix cascadeMatrices[MAX_CASCADES];
		for (unsigned int i = 0; i < MAX_CASCADES; i++)
		{
			// Calculate the points on the near plane
			vFrustumCorners[0] = vCamPos + (-vCamRight * fTanFOVX + vCamUp * fTanFOVY + vCamLook) * cascadeRanges[i];
			vFrustumCorners[1] = vCamPos + (vCamRight * fTanFOVX + vCamUp * fTanFOVY + vCamLook) * cascadeRanges[i];
			vFrustumCorners[2] = vCamPos + (vCamRight * fTanFOVX - vCamUp * fTanFOVY + vCamLook) * cascadeRanges[i];
			vFrustumCorners[3] = vCamPos + (-vCamRight * fTanFOVX - vCamUp * fTanFOVY + vCamLook) * cascadeRanges[i];

			// Calculate the points on the far plane
			vFrustumCorners[4] = vCamPos + (-vCamRight * fTanFOVX + vCamUp * fTanFOVY + vCamLook) * cascadeRanges[i + 1];
			vFrustumCorners[5] = vCamPos + (vCamRight * fTanFOVX + vCamUp * fTanFOVY + vCamLook) * cascadeRanges[i + 1];
			vFrustumCorners[6] = vCamPos + (vCamRight * fTanFOVX - vCamUp * fTanFOVY + vCamLook) * cascadeRanges[i + 1];
			vFrustumCorners[7] = vCamPos + (-vCamRight * fTanFOVX - vCamUp * fTanFOVY + vCamLook) * cascadeRanges[i + 1];

			BVVector vMax = VectorSet(-FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX);
			BVVector vMin = VectorSet(FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX);
			BVVector vPoint;
			for (unsigned int j = 0; j < 8; j++)
			{
				vPoint = VectorTransformPoint(vFrustumCorners[j], lightViewProj);

				vMax = VectorMax(vMax, vPoint);
				vMin = VectorMin(vMin, vPoint);
			}

			BVVector vCascadeCenter = 0.5f * (vMin + vMax);
			(&m_OffsetsX.x)[i] = -VectorGetX(vCascadeCenter);
			(&m_OffsetsY.x)[i] = -VectorGetY(vCascadeCenter);
			{
				vCascadeCenter = vMax - vMin;
				float x = VectorGetX(vCascadeCenter);
				float y = VectorGetY(vCascadeCenter);
				(&m_OffsetsScale.x)[i] = 2.0f / max(x, y);
			}

			cascadeMatrices[i] = lightViewProj * MatrixTranslation((&m_OffsetsX.x)[i], (&m_OffsetsY.x)[i], 0.0f) *
				MatrixScaling((&m_OffsetsScale.x)[i], (&m_OffsetsScale.x)[i], 1.0f);

			MatrixToFloat4x4(cascadeMatrices[i], m_CBCascadeParams.m_CascadeMatrices[i]);
		}

		pRenderer->SetShader(ShaderType::Vertex, m_pVSSpot);
		pRenderer->SetShader(ShaderType::Geometry, m_pGSDir);
		pRenderer->SetShader(ShaderType::Pixel, nullptr);

		m_pCBCascadeParams->Map(&m_CBCascadeParams, sizeof(CBCascadeParams));
		pRenderer->SetConstantBuffers(ShaderType::Geometry, &m_pCBCascadeParams, 1);

		RenderTargetView *pNullTarget[] = { nullptr };
		pRenderer->ClearDepthStencilView(m_pDirDSVs[index], true, 1.0f, false);
		pRenderer->SetRenderTargetViews(pNullTarget, 1, m_pDirDSVs[index]);

		D3D11_VIEWPORT vp[4] = {
			{ 0, 0, (float)m_Resolution, (float)m_Resolution, 0.0f, 1.0f },
			{ 0, 0, (float)m_Resolution, (float)m_Resolution, 0.0f, 1.0f },
			{ 0, 0, (float)m_Resolution, (float)m_Resolution, 0.0f, 1.0f },
			{ 0, 0, (float)m_Resolution, (float)m_Resolution, 0.0f, 1.0f }
		};
		pRenderer->GetContext()->RSSetViewports(4, vp);
	}

	void ShadowMap::UnsetDirShadowPass()
	{
		RendererD3D11 *pRenderer = RendererD3D11::GetMainInstance();
		BV_ASSERT(pRenderer != nullptr, "ShadowMap::UnsetDirShadowPass => Renderer not initialized or assigned");

		pRenderer->SetConstantBuffers(ShaderType::Geometry, nullptr, 1);
		pRenderer->SetShader(ShaderType::Geometry, nullptr);

		pRenderer->SetMainViewPort();
	}

	void ShadowMap::SetDirShadowParams(CRBVMatrix world)
	{
		RendererD3D11 *pRenderer = RendererD3D11::GetMainInstance();
		BV_ASSERT(pRenderer != nullptr, "ShadowMap::SetPointShadowParams => Renderer not initialized or assigned");

		MatrixToFloat4x4(world, m_CBObject.m_WVP);
		m_pCBObject->Map(&m_CBObject, sizeof(CBObject));
		pRenderer->SetConstantBuffers(ShaderType::Vertex, &m_pCBObject, 1);
	}

	void ShadowMap::RenderPointShadowMap(const unsigned int index)
	{
		RendererD3D11 *pRenderer = RendererD3D11::GetMainInstance();
		BV_ASSERT(pRenderer != nullptr, "ShadowMap::RenderPointShadowMap => Renderer not initialized or assigned");

		RenderTargetView *pRTV[] = { pRenderer->GetMainRenderTargetView() };
		pRenderer->SetRenderTargetViews(pRTV, 1, nullptr);

		pRenderer->SetShader(ShaderType::Vertex, m_pVSCubeScreen);
		pRenderer->SetShader(ShaderType::Pixel, m_pPSCubeScreen);

		pRenderer->SetShaderResources(ShaderType::Pixel, &m_pPointSRVs[index], 1);
		pRenderer->SetSamplers(ShaderType::Pixel, &m_pSamplerTrilinear, 1);

		pRenderer->SetPrimitiveTopology(Topology::TriangleList);
		pRenderer->SetInputLayout(nullptr);
		pRenderer->SetVertexBuffers(nullptr, 0);
		pRenderer->SetIndexBuffer(nullptr);

		pRenderer->Draw(36);

		pRenderer->SetShaderResources(ShaderType::Pixel, nullptr, 1);
		pRenderer->SetSamplers(ShaderType::Pixel, nullptr, 1);
		pRenderer->SetConstantBuffers(ShaderType::Pixel, nullptr, 1);
	}

	void ShadowMap::RenderSpotShadowMap(const unsigned int index)
	{
		RendererD3D11 *pRenderer = RendererD3D11::GetMainInstance();
		BV_ASSERT(pRenderer != nullptr, "ShadowMap::RenderSpotShadowMap => Renderer not initialized or assigned");

		RenderTargetView *pRTV[] = { pRenderer->GetMainRenderTargetView() };
		pRenderer->SetRenderTargetViews(pRTV, 1, nullptr);

		pRenderer->SetShader(ShaderType::Vertex, m_pVSScreen);
		pRenderer->SetShader(ShaderType::Pixel, m_pPSScreen);

		pRenderer->SetShaderResources(ShaderType::Pixel, &m_pSpotSRVs[index], 1);
		pRenderer->SetSamplers(ShaderType::Pixel, &m_pSamplerTrilinear, 1);

		pRenderer->SetPrimitiveTopology(Topology::TriangleStrip);
		pRenderer->SetInputLayout(nullptr);
		pRenderer->SetVertexBuffers(nullptr, 0);
		pRenderer->SetIndexBuffer(nullptr);

		pRenderer->Draw(4);

		pRenderer->SetShaderResources(ShaderType::Pixel, nullptr, 1);
		pRenderer->SetSamplers(ShaderType::Pixel, nullptr, 1);
	}
}