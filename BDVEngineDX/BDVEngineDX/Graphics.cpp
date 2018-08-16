#include "Graphics.h"

Graphics::Graphics(const HWND hWnd, const UINT width, const UINT height)
	: m_hWnd(hWnd), m_Width(width), m_Height(height), m_Windowed(true), m_VSync(0), m_QualityLevel(0), m_SampleCount(1)
{
}

Graphics::~Graphics()
{
}

bool Graphics::Initialize()
{
	if (!InitD3DResources())
	{
		return false;
	}

	return true;
}

void Graphics::Shutdown()
{
	if (m_pSwapChain)
	{
		if (!m_Windowed)
		{
			m_pSwapChain->SetFullscreenState(FALSE, nullptr);
		}
	}

	if (m_pContext)
	{
		m_pContext->ClearState();
	}
}

void Graphics::Clear(const float r, const float g, const float b)
{
	float backColor[] = { r, g, b, 1.0f };

	m_pContext->ClearRenderTargetView(m_pRenderTargetView.Get(), backColor);
	m_pContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void Graphics::Present()
{
	m_pSwapChain->Present(m_VSync, 0);
}

bool Graphics::SizeChange(const UINT width, const UINT height)
{
	m_Width = width;
	m_Height = height;

	m_pRenderTargetView.Reset();
	m_pDepthStencilView.Reset();
	m_pDepthStencilBuffer.Reset();

	return InitD3DSizeResources();
}

ID3D11Device *Graphics::GetDevice() const
{
	return m_pDevice.Get();
}

ID3D11DeviceContext *Graphics::GetContext() const
{
	return m_pContext.Get();
}

IDXGISwapChain *Graphics::GetSwapChain() const
{
	return m_pSwapChain.Get();
}

D3D_FEATURE_LEVEL Graphics::GetFeatureLevel() const
{
	return m_FeatureLevel;
}

const char *Graphics::GetShaderNameAndVersion(const ShaderType shaderType) const
{
	switch (m_FeatureLevel)
	{
	case D3D_FEATURE_LEVEL_10_0:
		switch (shaderType)
		{
		case ShaderType::VS:
		case ShaderType::VS_WITH_SO:
			return "vs_4_0";
		case ShaderType::GS:
		case ShaderType::GS_WITH_SO:
			return "gs_4_0";
		case ShaderType::PS:
			return "ps_4_0";
		case ShaderType::CS:
			return "cs_4_0";
		default:
			return nullptr;
		}
	case D3D_FEATURE_LEVEL_10_1:
		switch (shaderType)
		{
		case ShaderType::VS:
		case ShaderType::VS_WITH_SO:
			return "vs_4_1";
		case ShaderType::GS:
		case ShaderType::GS_WITH_SO:
			return "gs_4_1";
		case ShaderType::PS:
			return "ps_4_1";
		case ShaderType::CS:
			return "cs_4_1";
		default:
			return nullptr;
		}
	case D3D_FEATURE_LEVEL_11_0:
	case D3D_FEATURE_LEVEL_11_1:
		switch (shaderType)
		{
		case ShaderType::VS:
		case ShaderType::VS_WITH_SO:
			return "vs_5_0";
		case ShaderType::HS:
			return "hs_5_0";
		case ShaderType::DS:
			return "ds_5_0";
		case ShaderType::GS:
		case ShaderType::GS_WITH_SO:
			return "gs_5_0";
		case ShaderType::PS:
			return "ps_5_0";
		case ShaderType::CS:
			return "cs_5_0";
		default:
			return nullptr;
		}
	default:
		return nullptr;
	}
}

// Init Direct3D resources
bool Graphics::InitD3DResources()
{
	UINT flags = 0;// D3D11_CREATE_DEVICE_DEBUG;
	D3D_FEATURE_LEVEL levels[] = { D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0 };
	if (FAILED(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags, levels, 4, D3D11_SDK_VERSION, m_pDevice.ReleaseAndGetAddressOf(),
		&m_FeatureLevel, m_pContext.ReleaseAndGetAddressOf())))
	{
		return false;
	}

	for (m_SampleCount = 16; m_SampleCount >= 1; m_SampleCount /= 2)
	{
		if (FAILED(m_pDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, m_SampleCount, &m_QualityLevel)))
		{
			return false;
		}

		if (m_QualityLevel)
		{
			break;
		}
	}

	Microsoft::WRL::ComPtr<IDXGIFactory> factory;
	Microsoft::WRL::ComPtr<IDXGIAdapter> adapter;
	Microsoft::WRL::ComPtr<IDXGIOutput> output;

	if (FAILED(CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void**>(factory.GetAddressOf()))))
	{
		return false;
	}

	if (FAILED(factory->EnumAdapters(0, adapter.GetAddressOf())))
	{
		return false;
	}

	if (FAILED(adapter->EnumOutputs(0, output.GetAddressOf())))
	{
		return false;
	}

	UINT numModes = 0;
	if (FAILED(output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, nullptr)))
	{
		return false;
	}

	DXGI_MODE_DESC *displayModeList = new DXGI_MODE_DESC[numModes];
	if (FAILED(output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, &displayModeList[0])))
	{
		return false;
	}

	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.OutputWindow = m_hWnd;
	swapChainDesc.Windowed = m_Windowed;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	swapChainDesc.SampleDesc.Count = m_SampleCount;
	swapChainDesc.SampleDesc.Quality = m_QualityLevel ? m_QualityLevel - 1 : 0;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60; // Set to default, then change the value below
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1; // Set to default, then change the value below
	swapChainDesc.BufferDesc.Width = m_Width;
	swapChainDesc.BufferDesc.Height = m_Height;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	for (UINT i = 0; i < numModes; i++)
	{
		if (displayModeList[i].Width == m_Width)
		{
			if (displayModeList[i].Height == m_Height)
			{
				swapChainDesc.BufferDesc.RefreshRate.Numerator = displayModeList[i].RefreshRate.Numerator;
				swapChainDesc.BufferDesc.RefreshRate.Denominator = displayModeList[i].RefreshRate.Denominator;
				break;
			}
		}
	}

	delete[]displayModeList;

	Microsoft::WRL::ComPtr<IDXGIDevice> device;
	if (FAILED(m_pDevice.As(&device)))
	{
		return false;
	}

	if (FAILED(device->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(adapter.ReleaseAndGetAddressOf()))))
	{
		return false;
	}

	if (FAILED(adapter->GetParent(__uuidof(IDXGIFactory), reinterpret_cast<void**>(factory.ReleaseAndGetAddressOf()))))
	{
		return false;
	}

	if (FAILED(factory->CreateSwapChain(m_pDevice.Get(), &swapChainDesc, m_pSwapChain.GetAddressOf())))
	{
		return false;
	}

	if (FAILED(factory->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER | DXGI_MWA_NO_WINDOW_CHANGES)))
	{
		return false;
	}

	D3D11_RASTERIZER_DESC rasterDesc = { };
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.FrontCounterClockwise = true;
	rasterDesc.DepthClipEnable = true;
	if (FAILED(m_pDevice->CreateRasterizerState(&rasterDesc, m_pRasterizerState.ReleaseAndGetAddressOf())))
	{
		return false;
	}
	m_pContext->RSSetState(m_pRasterizerState.Get());

	return SizeChange(m_Width, m_Height);
}

// Init Direct3D size-dependent resources
bool Graphics::InitD3DSizeResources()
{
	DXGI_SWAP_CHAIN_DESC swapChainDesc;

	if (FAILED(m_pSwapChain->GetDesc(&swapChainDesc)))
	{
		return false;
	}

	if (FAILED(m_pSwapChain->ResizeBuffers(swapChainDesc.BufferCount, m_Width, m_Height, swapChainDesc.BufferDesc.Format, swapChainDesc.Flags)))
	{
		return false;
	}

	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
	if (FAILED(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()))))
	{
		return false;
	}

	if (FAILED(m_pDevice->CreateRenderTargetView(backBuffer.Get(), 0, m_pRenderTargetView.GetAddressOf())))
	{
		return false;
	}

	D3D11_TEXTURE2D_DESC depthStencilDesc = { 0 };
	depthStencilDesc.Width = m_Width;
	depthStencilDesc.Height = m_Height;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = m_SampleCount;
	depthStencilDesc.SampleDesc.Quality = m_QualityLevel ? m_QualityLevel - 1 : 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&depthStencilDesc, 0, m_pDepthStencilBuffer.GetAddressOf())))
	{
		return false;
	}

	if (FAILED(m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer.Get(), 0, m_pDepthStencilView.GetAddressOf())))
	{
		return false;
	}

	m_pContext->OMSetRenderTargets(1, m_pRenderTargetView.GetAddressOf(), m_pDepthStencilView.Get());

	m_Viewport.TopLeftX = 0;
	m_Viewport.TopLeftY = 0;
	m_Viewport.Width = static_cast<float>(m_Width);
	m_Viewport.Height = static_cast<float>(m_Height);
	m_Viewport.MinDepth = 0.0f;
	m_Viewport.MaxDepth = 1.0f;

	m_pContext->RSSetViewports(1, &m_Viewport);

	return true;
}