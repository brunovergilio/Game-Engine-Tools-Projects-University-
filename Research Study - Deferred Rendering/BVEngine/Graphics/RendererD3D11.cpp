#include "RendererD3D11.h"

namespace BVGraphics
{
	RendererD3D11 * RendererD3D11::m_pMainInstance = nullptr;

	RendererD3D11::RendererD3D11()
		: m_Windowed(true), m_pMainRenderTarget(nullptr), m_pDepthStencilViewTexture(nullptr), m_pDepthStencilView(nullptr)
	{
		m_pMainRenderTarget = new RenderTargetView();
		m_pDepthStencilView = new DepthStencilView();
		m_pDepthStencilViewTexture = new Texture();
	}


	RendererD3D11::~RendererD3D11()
	{
		delete m_pDepthStencilViewTexture;
		delete m_pDepthStencilView;
		delete m_pMainRenderTarget;
	}


	GraphicsResult RendererD3D11::Initialize(HWND hWnd, const unsigned int width, const unsigned int height)
	{
		GraphicsResult result = GraphicsResult::SUCCESS;

		HRESULT hr = S_OK;
		UINT flags = 0;
#ifdef _DEBUG
		flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
		//ComPtr<ID3D11Device> pDevice;
		//ComPtr<ID3D11DeviceContext> pContext;
		if (FAILED(hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags, nullptr, 0, D3D11_SDK_VERSION, m_pDevice.ReleaseAndGetAddressOf(),
			nullptr, m_pContext.ReleaseAndGetAddressOf())))
		{
			result = GraphicsResult::GRAPHICS_FAIL;
		}

		//pDevice.As(&m_pDevice);
		//pContext.As(&m_pContext);

		for (m_SampleCount = D3D11_MAX_MULTISAMPLE_SAMPLE_COUNT; m_SampleCount >= 1; m_SampleCount /= 2)
		{
			if (GraphicsResult::GRAPHICS_FAIL == result ||
				FAILED(hr = m_pDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, m_SampleCount, &m_QualityLevel)))
			{
				result = GraphicsResult::GRAPHICS_FAIL;
			}

			if (m_QualityLevel)
			{
				break;
			}
		}

		m_SampleCount = 1;
		m_QualityLevel = 0;

		Microsoft::WRL::ComPtr<IDXGIFactory> factory;
		Microsoft::WRL::ComPtr<IDXGIAdapter> adapter;
		Microsoft::WRL::ComPtr<IDXGIOutput> output;

		//ComPtr<IDXGIDevice3> dxgiDevice;
		//m_pDevice.As(&dxgiDevice);

		//ComPtr<IDXGIAdapter> dxgiAdapter;
		//dxgiDevice->GetAdapter(&dxgiAdapter);

		//ComPtr<IDXGIFactory2> dxgiFactory;
		//dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory));
		if (GraphicsResult::GRAPHICS_FAIL == result || FAILED(hr = CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void**>(factory.GetAddressOf()))))
		{
			result = GraphicsResult::GRAPHICS_FAIL;
		}

		if (GraphicsResult::GRAPHICS_FAIL == result || FAILED(hr = factory->EnumAdapters(0, adapter.GetAddressOf())))
		{
			result = GraphicsResult::GRAPHICS_FAIL;
		}

		if (GraphicsResult::GRAPHICS_FAIL == result || FAILED(hr = adapter->EnumOutputs(0, output.GetAddressOf())))
		{
			result = GraphicsResult::GRAPHICS_FAIL;
		}

		UINT numModes = 0;
		if (GraphicsResult::GRAPHICS_FAIL == result || FAILED(hr = output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, nullptr)))
		{
			result = GraphicsResult::GRAPHICS_FAIL;
		}

		std::vector<DXGI_MODE_DESC> displayModeList(numModes);
		if (GraphicsResult::GRAPHICS_FAIL == result || FAILED(hr = output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, &displayModeList[0])))
		{
			result = GraphicsResult::GRAPHICS_FAIL;
		}

		DXGI_SWAP_CHAIN_DESC swapChainDesc = { 0 };
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 1;
		swapChainDesc.OutputWindow = hWnd;
		swapChainDesc.Windowed = m_Windowed;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		swapChainDesc.SampleDesc.Count = m_SampleCount;
		swapChainDesc.SampleDesc.Quality = m_QualityLevel ? m_QualityLevel - 1 : 0;
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		swapChainDesc.BufferDesc.Width = width;
		swapChainDesc.BufferDesc.Height = height;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

		//DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };
		//swapChainDesc.Width = width; // Match the size of the window.
		//swapChainDesc.Height = height;
		//swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; // This is the most common swap chain format.
		//swapChainDesc.Stereo = false;
		//swapChainDesc.SampleDesc.Count = m_SampleCount;
		//swapChainDesc.SampleDesc.Quality = m_QualityLevel ? m_QualityLevel - 1 : 0;
		//swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		//swapChainDesc.BufferCount = 1; // Use double-buffering to minimize latency.
		//swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD; // All Windows Store apps must use this SwapEffect.
		//swapChainDesc.Flags = 0;
		//swapChainDesc.Scaling = DXGI_SCALING_NONE;
		//swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
		//dxgiFactory->CreateSwapChainForHwnd(m_pDevice.Get(), hWnd, &swapChainDesc, nullptr, nullptr, m_pSwapChain.ReleaseAndGetAddressOf());

		for (UINT i = 0; i < numModes; i++)
		{
			if (displayModeList[i].Width == width)
			{
				if (displayModeList[i].Height == height)
				{
					swapChainDesc.BufferDesc.RefreshRate.Numerator = displayModeList[i].RefreshRate.Numerator;
					swapChainDesc.BufferDesc.RefreshRate.Denominator = displayModeList[i].RefreshRate.Denominator;
					break;
				}
			}
		}

		Microsoft::WRL::ComPtr<IDXGIDevice> device;
		if (GraphicsResult::GRAPHICS_FAIL == result || FAILED(hr = m_pDevice.As(&device)))
		{
			result = GraphicsResult::GRAPHICS_FAIL;
		}

		if (GraphicsResult::GRAPHICS_FAIL == result || FAILED(hr = device->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(adapter.ReleaseAndGetAddressOf()))))
		{
			result = GraphicsResult::GRAPHICS_FAIL;
		}

		if (GraphicsResult::GRAPHICS_FAIL == result || FAILED(hr = adapter->GetParent(__uuidof(IDXGIFactory), reinterpret_cast<void**>(factory.ReleaseAndGetAddressOf()))))
		{
			result = GraphicsResult::GRAPHICS_FAIL;
		}

		if (GraphicsResult::GRAPHICS_FAIL == result || FAILED(hr = factory->CreateSwapChain(m_pDevice.Get(), &swapChainDesc, m_pSwapChain.ReleaseAndGetAddressOf())))
		{
			result = GraphicsResult::GRAPHICS_FAIL;
		}

		if (GraphicsResult::GRAPHICS_FAIL == result || FAILED(hr = factory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER | DXGI_MWA_NO_WINDOW_CHANGES)))
		{
			result = GraphicsResult::GRAPHICS_FAIL;
		}

		MakeMainInstance();

		return GraphicsResult::SUCCESS == result ? Resize(width, height) : result;
	}


	void RendererD3D11::Shutdown()
	{
		if (m_pSwapChain)
		{
			if (!m_Windowed)
			{
				ToggleFullScreen(false);
			}
		}

		if (m_pContext)
		{
			m_pContext->ClearState();
		}
	}


	GraphicsResult RendererD3D11::Resize(const unsigned int width, const unsigned int height)
	{
		ID3D11RenderTargetView *pRTV[] = { nullptr };
		m_pContext->OMSetRenderTargets(1, pRTV, nullptr);

		m_pContext->ClearState();

		return CreateSizeResources(width, height);
	}


	GraphicsResult RendererD3D11::ToggleFullScreen(const bool fullScreen)
	{
		GraphicsResult result = GraphicsResult::SUCCESS;

		m_Windowed = !fullScreen;

		HRESULT hr = S_OK;
		DXGI_SWAP_CHAIN_DESC swapChainDesc;

		if (FAILED(hr = m_pSwapChain->GetDesc(&swapChainDesc)))
		{
			result = GraphicsResult::GRAPHICS_FAIL;
		}

		if (!m_Windowed)
		{
			DXGI_MODE_DESC fullScreenMode = swapChainDesc.BufferDesc;
			Microsoft::WRL::ComPtr<IDXGIOutput> output;
			if (GraphicsResult::GRAPHICS_FAIL == result || FAILED(hr = m_pSwapChain->GetContainingOutput(output.GetAddressOf())))
			{
				result = GraphicsResult::GRAPHICS_FAIL;
			}

			if (GraphicsResult::GRAPHICS_FAIL == result || FAILED(hr = output->FindClosestMatchingMode(&swapChainDesc.BufferDesc, &fullScreenMode, m_pDevice.Get())))
			{
				result = GraphicsResult::GRAPHICS_FAIL;
			}

			if (GraphicsResult::GRAPHICS_FAIL == result || FAILED(hr = m_pSwapChain->ResizeTarget(&fullScreenMode)))
			{
				result = GraphicsResult::GRAPHICS_FAIL;
			}

			if (GraphicsResult::GRAPHICS_FAIL == result || FAILED(hr = m_pSwapChain->SetFullscreenState(TRUE, nullptr)))
			{
				result = GraphicsResult::GRAPHICS_FAIL;
			}

			fullScreenMode.RefreshRate.Denominator = 0;
			fullScreenMode.RefreshRate.Numerator = 0;
			if (GraphicsResult::GRAPHICS_FAIL == result || FAILED(hr = m_pSwapChain->ResizeTarget(&fullScreenMode)))
			{
				result = GraphicsResult::GRAPHICS_FAIL;
			}

			swapChainDesc.BufferDesc = fullScreenMode;
		}
		else
		{
			if (GraphicsResult::GRAPHICS_FAIL == result || FAILED(hr = m_pSwapChain->ResizeTarget(&swapChainDesc.BufferDesc)))
			{
				result = GraphicsResult::GRAPHICS_FAIL;
			}

			if (GraphicsResult::GRAPHICS_FAIL == result || FAILED(hr = m_pSwapChain->SetFullscreenState(FALSE, nullptr)))
			{
				result = GraphicsResult::GRAPHICS_FAIL;
			}
			swapChainDesc.BufferDesc.RefreshRate.Denominator = 0;
			swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
			if (GraphicsResult::GRAPHICS_FAIL == result || FAILED(hr = m_pSwapChain->ResizeTarget(&swapChainDesc.BufferDesc)))
			{
				result = GraphicsResult::GRAPHICS_FAIL;
			}
		}

		if (GraphicsResult::SUCCESS == result)
		{
			result = Resize(swapChainDesc.BufferDesc.Width, swapChainDesc.BufferDesc.Height);
		}

		return result;
	}

	void RendererD3D11::GetResolution(unsigned int & width, unsigned int & height)
	{
		DXGI_SWAP_CHAIN_DESC desc;
		m_pSwapChain->GetDesc(&desc);
		width = desc.BufferDesc.Width;
		height = desc.BufferDesc.Height;
	}

	void RendererD3D11::GetMSAA(unsigned int & sampleCount, unsigned int & qualityLevel)
	{
		sampleCount = m_SampleCount;
		qualityLevel = m_QualityLevel;
	}


	GraphicsResult RendererD3D11::CreateSizeResources(const unsigned int width, const unsigned int height)
	{
		GraphicsResult result = GraphicsResult::SUCCESS;

		m_pMainRenderTarget->Release();

		HRESULT hr = S_OK;
		DXGI_SWAP_CHAIN_DESC swapChainDesc;

		///////////////////////////
		// Resize the SwapChain
		if (FAILED(hr = m_pSwapChain->GetDesc(&swapChainDesc)))
		{
			result = GraphicsResult::GRAPHICS_FAIL;
		}

		if (GraphicsResult::GRAPHICS_FAIL == result || FAILED(hr = m_pSwapChain->ResizeBuffers(swapChainDesc.BufferCount, width, height, swapChainDesc.BufferDesc.Format, swapChainDesc.Flags)))
		{
			result = GraphicsResult::GRAPHICS_FAIL;
		}


		///////////////////////////
		// Get the back buffer texture and create the main Render Target
		Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
		if (GraphicsResult::GRAPHICS_FAIL == result || FAILED(hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()))))
		{
			result = GraphicsResult::GRAPHICS_FAIL;
		}

		if (GraphicsResult::GRAPHICS_FAIL == result || GraphicsResult::GRAPHICS_FAIL == m_pMainRenderTarget->Create(backBuffer.Get()))
		{
			result = GraphicsResult::GRAPHICS_FAIL;
		}


		///////////////////////////
		// Create the Depth Stencil View's texture, target and shader resource view
		D3D11_TEXTURE2D_DESC depthStencilViewTextureDesc = { 0 };
		depthStencilViewTextureDesc.Width = width;
		depthStencilViewTextureDesc.Height = height;
		depthStencilViewTextureDesc.MipLevels = 1;
		depthStencilViewTextureDesc.ArraySize = 1;
		depthStencilViewTextureDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
		depthStencilViewTextureDesc.SampleDesc.Count = m_SampleCount;
		depthStencilViewTextureDesc.SampleDesc.Quality = m_QualityLevel ? m_QualityLevel - 1 : 0;
		depthStencilViewTextureDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilViewTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthStencilViewTextureDesc.CPUAccessFlags = 0;
		depthStencilViewTextureDesc.MiscFlags = 0;

		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
		ZeroMemory(&depthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
		depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilViewDesc.ViewDimension = m_SampleCount > 1 ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;

		if (GraphicsResult::GRAPHICS_FAIL == result || GraphicsResult::GRAPHICS_FAIL == m_pDepthStencilViewTexture->Create2D(depthStencilViewTextureDesc))
		{
			result = GraphicsResult::GRAPHICS_FAIL;
		}

		if (GraphicsResult::GRAPHICS_FAIL == result || GraphicsResult::GRAPHICS_FAIL == m_pDepthStencilView->Create(m_pDepthStencilViewTexture->Get2D(), depthStencilViewDesc))
		{
			result = GraphicsResult::GRAPHICS_FAIL;
		}

		m_Viewport.TopLeftX = 0;
		m_Viewport.TopLeftY = 0;
		m_Viewport.Width = static_cast<float>(width);
		m_Viewport.Height = static_cast<float>(height);
		m_Viewport.MinDepth = 0.0f;
		m_Viewport.MaxDepth = 1.0f;

		m_pContext->RSSetViewports(1, &m_Viewport);

		return result;
	}


	void RendererD3D11::ClearDepthStencilView(const DepthStencilView * const pDSV, const bool depth, const float depthVal, const bool stencil, const unsigned char stencilVal)
	{
		m_pContext->ClearDepthStencilView(pDSV->Get(), (depth ? D3D11_CLEAR_DEPTH : 0) | (stencil ? D3D11_CLEAR_STENCIL : 0), depthVal, stencilVal);
	}


	void RendererD3D11::ClearRenderTargetView(const RenderTargetView * const pRTV, const float r, const float g, const float b)
	{
		const float backColor[] = { r, g, b, 1.0f };

		m_pContext->ClearRenderTargetView(pRTV->Get(), backColor);
	}


	void RendererD3D11::SetRenderTargetViews(const RenderTargetView * const * const pRTVs, const unsigned int numRTVs, const DepthStencilView * const pDSV)
	{
		ID3D11RenderTargetView *pRenderTargets[BV_MAX_RTVS] = {};

		for (size_t i = 0; i < numRTVs; i++)
		{
			pRenderTargets[i] = pRTVs ? (pRTVs[i] ? pRTVs[i]->Get() : nullptr) : nullptr;
		}

		m_pContext->OMSetRenderTargets(numRTVs, pRenderTargets, pDSV ? pDSV->Get() : nullptr);
	}

	void RendererD3D11::SetMainViewPort()
	{
		DXGI_SWAP_CHAIN_DESC desc;
		m_pSwapChain->GetDesc(&desc);
		
		D3D11_VIEWPORT vp[1] = { { 0, 0, desc.BufferDesc.Width, desc.BufferDesc.Height, 0.0f, 1.0f } };
		m_pContext->RSSetViewports(1, vp);
	}


	void RendererD3D11::SetPrimitiveTopology(const Topology topology)
	{
		m_pContext->IASetPrimitiveTopology(static_cast<D3D11_PRIMITIVE_TOPOLOGY>(topology));
	}

	void RendererD3D11::SetInputLayout(const InputLayout * const pIA)
	{
		m_pContext->IASetInputLayout(pIA ? pIA->Get() : nullptr);
	}


	void RendererD3D11::SetVertexBuffers(const VertexBuffer * const * const pVB, const unsigned int numBuffers)
	{
		ID3D11Buffer *pBuffers[BV_MAX_VERTEX_BUFFERS] = {};
		unsigned int offsets[BV_MAX_VERTEX_BUFFERS] = {};
		unsigned int strides[BV_MAX_VERTEX_BUFFERS] = {};
		for (size_t i = 0; i < numBuffers; i++)
		{
			pBuffers[i] = pVB ? (pVB[i] ? pVB[i]->Get() : nullptr) : nullptr;
			strides[i] = pVB ? (pVB[i] ? pVB[i]->GetVertexSize() : 0) : 0;
			offsets[i] = 0;
		}

		m_pContext->IASetVertexBuffers(0, numBuffers, pBuffers, strides, offsets);
	}

	void RendererD3D11::SetIndexBuffer(const IndexBuffer * const pIB, const DXGI_FORMAT format)
	{
		m_pContext->IASetIndexBuffer(pIB ? pIB->Get() : nullptr, format, 0);
	}

	void RendererD3D11::SetShader(const ShaderType shaderType, const Shader * const pShader)
	{
		switch (shaderType)
		{
		case ShaderType::Vertex:
			m_pContext->VSSetShader(pShader ? reinterpret_cast<const VertexShader * const>(pShader)->GetShader() : nullptr, nullptr, 0);
			break;
		case ShaderType::Hull:
			m_pContext->HSSetShader(pShader ? reinterpret_cast<const HullShader * const>(pShader)->GetShader() : nullptr, nullptr, 0);
			break;
		case ShaderType::Domain:
			m_pContext->DSSetShader(pShader ? reinterpret_cast<const DomainShader * const>(pShader)->GetShader() : nullptr, nullptr, 0);
			break;
		case ShaderType::Geometry:
			m_pContext->GSSetShader(pShader ? reinterpret_cast<const GeometryShader * const>(pShader)->GetShader() : nullptr, nullptr, 0);
			break;
		case ShaderType::Pixel:
			m_pContext->PSSetShader(pShader ? reinterpret_cast<const PixelShader * const>(pShader)->GetShader() : nullptr, nullptr, 0);
			break;
		case ShaderType::Compute:
			m_pContext->CSSetShader(pShader ? reinterpret_cast<const ComputeShader * const>(pShader)->GetShader() : nullptr, nullptr, 0);
			break;
		}
	}

	void RendererD3D11::SetConstantBuffers(const ShaderType shaderType, const ConstantBuffer * const * const pCB, const unsigned int numBuffers)
	{
		ID3D11Buffer *pBuffers[BV_MAX_CONSTANT_BUFFERS] = {};
		for (size_t i = 0; i < numBuffers; i++)
		{
			pBuffers[i] = pCB ? (pCB[i] ? pCB[i]->Get() : nullptr) : nullptr;
		}

		switch (shaderType)
		{
		case ShaderType::Vertex:
			m_pContext->VSSetConstantBuffers(0, numBuffers, pBuffers);
			break;
		case ShaderType::Hull:
			m_pContext->HSSetConstantBuffers(0, numBuffers, pBuffers);
			break;
		case ShaderType::Domain:
			m_pContext->DSSetConstantBuffers(0, numBuffers, pBuffers);
			break;
		case ShaderType::Geometry:
			m_pContext->GSSetConstantBuffers(0, numBuffers, pBuffers);
			break;
		case ShaderType::Pixel:
			m_pContext->PSSetConstantBuffers(0, numBuffers, pBuffers);
			break;
		case ShaderType::Compute:
			m_pContext->CSSetConstantBuffers(0, numBuffers, pBuffers);
			break;
		}
	}

	void RendererD3D11::SetShaderResources(const ShaderType shaderType, const ShaderResourceView * const * const pSRVs, const unsigned int numSRVs)
	{
		ID3D11ShaderResourceView *pViews[BV_MAX_SRVS] = {};
		for (size_t i = 0; i < numSRVs; i++)
		{
			pViews[i] = pSRVs ? (pSRVs[i] ? pSRVs[i]->Get() : nullptr) : nullptr;
		}

		switch (shaderType)
		{
		case ShaderType::Vertex:
			m_pContext->VSSetShaderResources(0, numSRVs, pViews);
			break;
		case ShaderType::Hull:
			m_pContext->HSSetShaderResources(0, numSRVs, pViews);
			break;
		case ShaderType::Domain:
			m_pContext->DSSetShaderResources(0, numSRVs, pViews);
			break;
		case ShaderType::Geometry:
			m_pContext->GSSetShaderResources(0, numSRVs, pViews);
			break;
		case ShaderType::Pixel:
			m_pContext->PSSetShaderResources(0, numSRVs, pViews);
			break;
		case ShaderType::Compute:
			m_pContext->CSSetShaderResources(0, numSRVs, pViews);
			break;
		}
	}

	void RendererD3D11::SetSamplers(const ShaderType shaderType, const SamplerState * const * const pSamplers, const unsigned int numSamplers)
	{
		ID3D11SamplerState *pSam[BV_MAX_SAMPLERS] = {};
		for (size_t i = 0; i < numSamplers; i++)
		{
			pSam[i] = pSamplers ? (pSamplers[i] ? pSamplers[i]->Get() : nullptr) : nullptr;
		}

		switch (shaderType)
		{
		case ShaderType::Vertex:
			m_pContext->VSSetSamplers(0, numSamplers, pSam);
			break;
		case ShaderType::Hull:
			m_pContext->HSSetSamplers(0, numSamplers, pSam);
			break;
		case ShaderType::Domain:
			m_pContext->DSSetSamplers(0, numSamplers, pSam);
			break;
		case ShaderType::Geometry:
			m_pContext->GSSetSamplers(0, numSamplers, pSam);
			break;
		case ShaderType::Pixel:
			m_pContext->PSSetSamplers(0, numSamplers, pSam);
			break;
		case ShaderType::Compute:
			m_pContext->CSSetSamplers(0, numSamplers, pSam);
			break;
		}
	}


	void RendererD3D11::SetRasterizerState(const RasterizerState * const pRasterizerState)
	{
		m_pContext->RSSetState(pRasterizerState ? pRasterizerState->Get() : nullptr);
	}


	void RendererD3D11::SetBlendState(const BlendState * const pBlendState, const float * const blendFactor, const unsigned int sampleMask)
	{
		m_pContext->OMSetBlendState(pBlendState ? pBlendState->Get() : nullptr, blendFactor, sampleMask);
	}

	void RendererD3D11::SetDepthStencilState(const DepthStencilState * const pDepthStencilStance, const unsigned int stencilRef)
	{
		m_pContext->OMSetDepthStencilState(pDepthStencilStance ? pDepthStencilStance->Get() : nullptr, stencilRef);
	}

	void RendererD3D11::Draw(const unsigned int vertexCount)
	{
		m_pContext->Draw(vertexCount, 0);
	}

	void RendererD3D11::DrawIndexed(const unsigned int indexCount)
	{
		m_pContext->DrawIndexed(indexCount, 0, 0);
	}

	void RendererD3D11::DrawIndexedInstanced(const unsigned int indexCount, const unsigned int instanceCount)
	{
		m_pContext->DrawIndexedInstanced(indexCount, instanceCount, 0, 0, 0);
	}

	void RendererD3D11::Present()
	{
		m_pSwapChain->Present(0, 0);
	}
}