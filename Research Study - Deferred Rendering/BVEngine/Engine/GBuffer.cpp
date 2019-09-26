#include "GBuffer.h"


namespace BVEngine
{
	GBuffer::GBuffer()
		: m_pDSVTexture(nullptr), m_pDSVSRV(nullptr), m_pDSV(nullptr), m_pDSVReadOnly(nullptr),
		m_pDiffuseTexture(nullptr), m_pDiffuseSRV(nullptr), m_pDiffuseRTV(nullptr),
		m_pNormalTexture(nullptr), m_pNormalSRV(nullptr), m_pNormalRTV(nullptr),
		m_pAccumulationTexture(nullptr), m_pAccumulationSRV(nullptr), m_pAccumulationRTV(nullptr),
		m_pSpecularTexture(nullptr), m_pSpecularSRV(nullptr), m_pSpecularRTV(nullptr),
		m_pDSS(nullptr), m_pCBProjParams(nullptr), m_pVSGBufferScreen(nullptr), m_pPSGBufferScreen(nullptr), m_pSamplerTrilinear(nullptr),
		m_pVSGBufferFullScreen(nullptr), m_pPSGBufferFullScreen(nullptr), m_pPSGBufferDepthFullScreen(nullptr)
	{
	}


	GBuffer::~GBuffer()
	{
		Destroy();
	}


	GraphicsResult GBuffer::Create(const unsigned int width, const unsigned int height, const bool createAll)
	{
		RendererD3D11 *pRenderer = RendererD3D11::GetMainInstance();
		BV_ASSERT(pRenderer != nullptr, "GBuffer::Create => Renderer not initialized or assigned");

		unsigned int sampleCount, qualityLevel;
		pRenderer->GetMSAA(sampleCount, qualityLevel);

		Destroy(createAll);

		m_pDSVTexture = new Texture();
		m_pDSVSRV = new ShaderResourceView();
		m_pDSV = new DepthStencilView();
		m_pDSVReadOnly = new DepthStencilView();

		m_pDiffuseTexture = new Texture();
		m_pDiffuseSRV = new ShaderResourceView();
		m_pDiffuseRTV = new RenderTargetView();

		m_pNormalTexture = new Texture();
		m_pNormalSRV = new ShaderResourceView();
		m_pNormalRTV = new RenderTargetView();

		m_pAccumulationTexture = new Texture();
		m_pAccumulationSRV = new ShaderResourceView();
		m_pAccumulationRTV = new RenderTargetView();

		m_pSpecularTexture = new Texture();
		m_pSpecularSRV = new ShaderResourceView();
		m_pSpecularRTV = new RenderTargetView();

		GraphicsResult result = GraphicsResult::SUCCESS;

		// Texture description for the buffers
		D3D11_TEXTURE2D_DESC textureDesc = { 0 };
		textureDesc.Width = width;
		textureDesc.Height = height;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
		textureDesc.SampleDesc.Count = sampleCount;
		textureDesc.SampleDesc.Quality = qualityLevel ? qualityLevel - 1 : 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;

		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
		ZeroMemory(&depthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
		depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilViewDesc.ViewDimension = sampleCount > 1 ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;

		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
		ZeroMemory(&shaderResourceViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		shaderResourceViewDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		shaderResourceViewDesc.ViewDimension = sampleCount > 1 ? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;
		//shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		shaderResourceViewDesc.Texture2D.MipLevels = sampleCount > 1 ? 0 : 1;

		// =======================
		// GBuffers
		// =======================

		// =======================
		// Depth
		// =======================

		do
		{
			if (GraphicsResult::TEXTURE_FAIL == m_pDSVTexture->Create2D(textureDesc))
			{
				result = GraphicsResult::GRAPHICS_FAIL;
				break;
			}

			if (GraphicsResult::SHADER_RESOURCE_VIEW_FAIL == (result = m_pDSVSRV->Create(m_pDSVTexture->Get2D(), shaderResourceViewDesc)))
			{
				break;
			}

			if (GraphicsResult::DEPTH_STENCIL_VIEW_FAIL == (result = m_pDSV->Create(m_pDSVTexture->Get2D(), depthStencilViewDesc)))
			{
				break;
			}

			depthStencilViewDesc.Flags = D3D11_DSV_READ_ONLY_DEPTH | D3D11_DSV_READ_ONLY_STENCIL;
			if (GraphicsResult::DEPTH_STENCIL_VIEW_FAIL == (result = m_pDSVReadOnly->Create(m_pDSVTexture->Get2D(), depthStencilViewDesc)))
			{
				break;
			}
			// =======================



			// =======================
			// Diffuse
			// =======================

			textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
			textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			shaderResourceViewDesc.Format = textureDesc.Format;

			D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
			ZeroMemory(&renderTargetViewDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
			renderTargetViewDesc.Format = textureDesc.Format;
			renderTargetViewDesc.ViewDimension = sampleCount > 1 ? D3D11_RTV_DIMENSION_TEXTURE2DMS : D3D11_RTV_DIMENSION_TEXTURE2D;

			if (GraphicsResult::TEXTURE_FAIL == (result = m_pDiffuseTexture->Create2D(textureDesc)))
			{
				break;
			}

			if (GraphicsResult::SHADER_RESOURCE_VIEW_FAIL == (result = m_pDiffuseSRV->Create(m_pDiffuseTexture->Get2D(), shaderResourceViewDesc)))
			{
				break;
			}

			if (GraphicsResult::RENDER_TARGET_VIEW_FAIL == (result = m_pDiffuseRTV->Create(m_pDiffuseTexture->Get2D(), renderTargetViewDesc)))
			{
				break;
			}


			// =======================
			// Normal
			// =======================
			textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			renderTargetViewDesc.Format = textureDesc.Format;
			shaderResourceViewDesc.Format = textureDesc.Format;

			if (GraphicsResult::TEXTURE_FAIL == (result = m_pNormalTexture->Create2D(textureDesc)))
			{
				break;
			}

			if (GraphicsResult::SHADER_RESOURCE_VIEW_FAIL == (result = m_pNormalSRV->Create(m_pNormalTexture->Get2D(), shaderResourceViewDesc)))
			{
				break;
			}

			if (GraphicsResult::RENDER_TARGET_VIEW_FAIL == (result = m_pNormalRTV->Create(m_pNormalTexture->Get2D(), renderTargetViewDesc)))
			{
				break;
			}

			// =======================
			// Accumulation
			// =======================
			textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			renderTargetViewDesc.Format = textureDesc.Format;
			shaderResourceViewDesc.Format = textureDesc.Format;

			if (GraphicsResult::TEXTURE_FAIL == (result = m_pAccumulationTexture->Create2D(textureDesc)))
			{
				break;
			}

			if (GraphicsResult::SHADER_RESOURCE_VIEW_FAIL == (result = m_pAccumulationSRV->Create(m_pAccumulationTexture->Get2D(), shaderResourceViewDesc)))
			{
				break;
			}

			if (GraphicsResult::RENDER_TARGET_VIEW_FAIL == (result = m_pAccumulationRTV->Create(m_pAccumulationTexture->Get2D(), renderTargetViewDesc)))
			{
				break;
			}

			// =======================
			// Specular
			// =======================
			if (GraphicsResult::TEXTURE_FAIL == (result = m_pSpecularTexture->Create2D(textureDesc)))
			{
				break;
			}

			if (GraphicsResult::SHADER_RESOURCE_VIEW_FAIL == (result = m_pSpecularSRV->Create(m_pSpecularTexture->Get2D(), shaderResourceViewDesc)))
			{
				break;
			}

			if (GraphicsResult::RENDER_TARGET_VIEW_FAIL == (result = m_pSpecularRTV->Create(m_pSpecularTexture->Get2D(), renderTargetViewDesc)))
			{
				break;
			}


			if (createAll)
			{
				m_pDSS = new DepthStencilState();

				D3D11_DEPTH_STENCIL_DESC descDepth;
				descDepth.DepthEnable = TRUE;
				descDepth.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
				descDepth.DepthFunc = D3D11_COMPARISON_LESS;
				descDepth.StencilEnable = TRUE;
				descDepth.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
				descDepth.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
				const D3D11_DEPTH_STENCILOP_DESC stencilMarkOp = { D3D11_STENCIL_OP_REPLACE, D3D11_STENCIL_OP_REPLACE, D3D11_STENCIL_OP_REPLACE, D3D11_COMPARISON_ALWAYS };
				descDepth.FrontFace = stencilMarkOp;
				descDepth.BackFace = stencilMarkOp;
				if (GraphicsResult::DEPTH_STENCIL_STATE_FAIL == (result = m_pDSS->Create(descDepth)))
				{
					break;
				}

				m_pCBProjParams = new ConstantBuffer();
				if (GraphicsResult::HARDWARE_BUFFER_FAIL == (result = m_pCBProjParams->CreateDynamic(sizeof(CBProjParams))))
				{
					break;
				}

				m_pVSGBufferScreen = new VertexShader();
				if (GraphicsResult::SHADER_FAIL == (result = m_pVSGBufferScreen->CompileFromFile("GBufferMainScreen.vs.hlsl",
					"VS", nullptr, 0)))
				{
					break;
				}

				m_pPSGBufferScreen = new PixelShader();
				if (GraphicsResult::SHADER_FAIL == (result = m_pPSGBufferScreen->CompileFromFile("GBufferMainScreen.ps.hlsl",
					"PS")))
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

				m_pVSGBufferFullScreen = new VertexShader();
				if (GraphicsResult::SHADER_FAIL == (result = m_pVSGBufferFullScreen->CompileFromFile("GBufferFullScreen.vs.hlsl",
					"VS", nullptr, 0)))
				{
					break;
				}

				m_pPSGBufferFullScreen = new PixelShader();
				if (GraphicsResult::SHADER_FAIL == (result = m_pPSGBufferFullScreen->CompileFromFile("GBufferFullScreen.ps.hlsl",
					"PS")))
				{
					break;
				}

				m_pPSGBufferDepthFullScreen = new PixelShader();
				if (GraphicsResult::SHADER_FAIL == (result = m_pPSGBufferDepthFullScreen->CompileFromFile("GBufferFullScreen.ps.hlsl",
					"PS_Depth")))
				{
					break;
				}
			}
		} while (0);

		if (GraphicsResult::SUCCESS != result)
		{
			Destroy(createAll);
		}

		return result;
	}


	void GBuffer::Destroy(const bool destroyAll)
	{
		if (destroyAll)
		{
			if (m_pDSS) { delete m_pDSS; m_pDSS = nullptr; };
			if (m_pCBProjParams) { delete m_pCBProjParams; m_pCBProjParams = nullptr; }
			if (m_pVSGBufferScreen) { delete m_pVSGBufferScreen; m_pVSGBufferScreen = nullptr; }
			if (m_pPSGBufferScreen) { delete m_pPSGBufferScreen; m_pPSGBufferScreen = nullptr; }
			if (m_pSamplerTrilinear) { delete m_pSamplerTrilinear; m_pSamplerTrilinear = nullptr; }

			if (m_pVSGBufferFullScreen) { delete m_pVSGBufferFullScreen; m_pVSGBufferFullScreen = nullptr; }
			if (m_pPSGBufferFullScreen) { delete m_pPSGBufferFullScreen; m_pPSGBufferFullScreen = nullptr; }
			if (m_pPSGBufferDepthFullScreen) { delete m_pPSGBufferDepthFullScreen; m_pPSGBufferDepthFullScreen = nullptr; }
		}

		if (m_pSpecularRTV) { delete m_pSpecularRTV; m_pSpecularRTV = nullptr; };
		if (m_pSpecularSRV) { delete m_pSpecularSRV; m_pSpecularSRV = nullptr; };
		if (m_pSpecularTexture) { delete m_pSpecularTexture; m_pSpecularTexture = nullptr; };

		if (m_pAccumulationRTV) { delete m_pAccumulationRTV; m_pAccumulationRTV = nullptr; };
		if (m_pAccumulationSRV) { delete m_pAccumulationSRV; m_pAccumulationSRV = nullptr; };
		if (m_pAccumulationTexture) { delete m_pAccumulationTexture; m_pAccumulationTexture = nullptr; };
		
		if (m_pNormalRTV) { delete m_pNormalRTV; m_pNormalRTV = nullptr; };
		if (m_pNormalSRV) { delete m_pNormalSRV; m_pNormalSRV = nullptr; };
		if (m_pNormalTexture) { delete m_pNormalTexture; m_pNormalTexture = nullptr; };

		if (m_pDiffuseRTV) { delete m_pDiffuseRTV; m_pDiffuseRTV = nullptr; };
		if (m_pDiffuseSRV) { delete m_pDiffuseSRV; m_pDiffuseSRV = nullptr; };
		if (m_pDiffuseTexture) { delete m_pDiffuseTexture; m_pDiffuseTexture = nullptr; };

		if (m_pDSVReadOnly) { delete m_pDSVReadOnly; m_pDSVReadOnly = nullptr; };
		if (m_pDSV) { delete m_pDSV; m_pDSV = nullptr; };
		if (m_pDSVSRV) { delete m_pDSVSRV; m_pDSVSRV = nullptr; };
		if (m_pDSVTexture) { delete m_pDSVTexture; m_pDSVTexture = nullptr; };
	}


	void GBuffer::Set()
	{
		RendererD3D11 *pRenderer = RendererD3D11::GetMainInstance();
		BV_ASSERT(pRenderer != nullptr, "GBuffer::Set => Renderer not initialized or assigned");

		pRenderer->ClearDepthStencilView(m_pDSV);

		pRenderer->ClearRenderTargetView(m_pDiffuseRTV);
		pRenderer->ClearRenderTargetView(m_pNormalRTV);
		pRenderer->ClearRenderTargetView(m_pAccumulationRTV);
		pRenderer->ClearRenderTargetView(m_pSpecularRTV);

		RenderTargetView * const pRTVs[] = { m_pDiffuseRTV, m_pNormalRTV, m_pAccumulationRTV, m_pSpecularRTV };
		pRenderer->SetRenderTargetViews(pRTVs, 4, m_pDSV);

		pRenderer->SetDepthStencilState(m_pDSS, 1);
	}


	void GBuffer::Unset()
	{
		RendererD3D11 *pRenderer = RendererD3D11::GetMainInstance();
		BV_ASSERT(pRenderer != nullptr, "GBuffer::Unset => Renderer not initialized or assigned");

		pRenderer->SetDepthStencilState(nullptr, 0);

		RenderTargetView * const pRTVs[] = { nullptr, nullptr, nullptr, nullptr };
		pRenderer->SetRenderTargetViews(pRTVs, 4, m_pDSVReadOnly);
	}


	void GBuffer::RenderGBuffers(const BVFloat44 & proj)
	{
		RendererD3D11 *pRenderer = RendererD3D11::GetMainInstance();
		BV_ASSERT(pRenderer != nullptr, "GBuffer::RenderGBuffers => Renderer not initialized or assigned");

		RenderTargetView *pRTV[] = { pRenderer->GetMainRenderTargetView() };
		pRenderer->SetRenderTargetViews(pRTV, 1, nullptr);

		m_CBProjParams.m_ProjParams.x = 1.0f / proj.r[0].x;
		m_CBProjParams.m_ProjParams.y = 1.0f / proj.r[1].y;
		m_CBProjParams.m_ProjParams.z = proj.r[3].z;
		m_CBProjParams.m_ProjParams.w = proj.r[2].z;
		m_pCBProjParams->Map(&m_CBProjParams, sizeof(CBProjParams));
		pRenderer->SetConstantBuffers(ShaderType::Pixel, &m_pCBProjParams, 1);

		pRenderer->SetShader(ShaderType::Vertex, m_pVSGBufferScreen);
		pRenderer->SetShader(ShaderType::Pixel, m_pPSGBufferScreen);

		ShaderResourceView *pTextures[] = { m_pDSVSRV, m_pDiffuseSRV, m_pNormalSRV,
			m_pAccumulationSRV, m_pSpecularSRV };
		pRenderer->SetShaderResources(ShaderType::Pixel, pTextures, 5);

		pRenderer->SetSamplers(ShaderType::Pixel, &m_pSamplerTrilinear, 1);

		pRenderer->SetPrimitiveTopology(Topology::TriangleStrip);
		pRenderer->SetInputLayout(nullptr);
		pRenderer->SetVertexBuffers(nullptr, 0);
		pRenderer->SetIndexBuffer(nullptr);

		pRenderer->Draw(20);

		pRenderer->SetShaderResources(ShaderType::Pixel, nullptr, 5);

		pRenderer->SetRenderTargetViews(pRTV, 1, m_pDSV);
	}


	void GBuffer::RenderGBuffer(const unsigned int index, const BVFloat44 & proj)
	{
		RendererD3D11 *pRenderer = RendererD3D11::GetMainInstance();
		BV_ASSERT(pRenderer != nullptr, "GBuffer::RenderGBuffer => Renderer not initialized or assigned");

		RenderTargetView *pRTV[] = { pRenderer->GetMainRenderTargetView() };
		pRenderer->SetRenderTargetViews(pRTV, 1, nullptr);

		pRenderer->SetShader(ShaderType::Vertex, m_pVSGBufferFullScreen);

		if (0U == index)
		{
			m_CBProjParams.m_ProjParams.z = proj.r[3].z;
			m_CBProjParams.m_ProjParams.w = proj.r[2].z;
			m_pCBProjParams->Map(&m_CBProjParams, sizeof(CBProjParams));
			pRenderer->SetConstantBuffers(ShaderType::Pixel, &m_pCBProjParams, 1);
			pRenderer->SetShader(ShaderType::Pixel, m_pPSGBufferDepthFullScreen);
		}
		else
		{
			pRenderer->SetShader(ShaderType::Pixel, m_pPSGBufferFullScreen);
		}

		switch (index)
		{
		case 1:
			pRenderer->SetShaderResources(ShaderType::Pixel, &m_pDiffuseSRV, 1);
			break;
		case 2:
			pRenderer->SetShaderResources(ShaderType::Pixel, &m_pNormalSRV, 1);
			break;
		case 3:
			pRenderer->SetShaderResources(ShaderType::Pixel, &m_pAccumulationSRV, 1);
			break;
		case 4:
			pRenderer->SetShaderResources(ShaderType::Pixel, &m_pSpecularSRV, 1);
			break;
		case 0:
		default:
			pRenderer->SetShaderResources(ShaderType::Pixel, &m_pDSVSRV, 1);
			break;
		}

		pRenderer->SetSamplers(ShaderType::Pixel, &m_pSamplerTrilinear, 1);

		pRenderer->SetPrimitiveTopology(Topology::TriangleStrip);
		pRenderer->SetInputLayout(nullptr);
		pRenderer->SetVertexBuffers(nullptr, 0);
		pRenderer->SetIndexBuffer(nullptr);

		pRenderer->Draw(4);

		pRenderer->SetShaderResources(ShaderType::Pixel, nullptr, 1);

		pRenderer->SetRenderTargetViews(pRTV, 1, m_pDSV);
	}
}