#include "Texture.h"
#include "DDSTextureLoader\DDSTextureLoader.h"
#include "WICTextureLoader\WICTextureLoader.h"

Texture::Texture()
	: m_Width(0U), m_Height(0U)
{
}

Texture::~Texture()
{
}

Texture::Texture(ID3D11ShaderResourceView *pView)
{
	m_pTexture = pView;
}

bool Texture::Initialize(const Graphics * const &pGraphics, const char * const pFileName)
{
	char textureName[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, textureName);
	strcat_s(textureName, MAX_PATH, "\\Data\\Textures\\");
	strcat_s(textureName, MAX_PATH, pFileName);

	ID3D11Device *pDevice = pGraphics->GetDevice();

	wchar_t textureNameW[MAX_PATH];
	size_t numConv = 0;

	mbstowcs_s(&numConv, textureNameW, strlen(textureName) + 1, textureName, _TRUNCATE);

	ComPtr<ID3D11Resource> pResource;

	HRESULT hr = S_OK;
	if (strstr(pFileName, ".dds"))
	{
		hr = CreateDDSTextureFromFile(pDevice, textureNameW, pResource.ReleaseAndGetAddressOf(), m_pTexture.ReleaseAndGetAddressOf());
	}
	else
	{
		hr = CreateWICTextureFromFile(pDevice, textureNameW, pResource.ReleaseAndGetAddressOf(), m_pTexture.ReleaseAndGetAddressOf());
	}

	if (pResource.Get())
	{
		ID3D11Texture2D *pTexture = reinterpret_cast<ID3D11Texture2D*>(pResource.Get());

		D3D11_TEXTURE2D_DESC texDesc;
		pTexture->GetDesc(&texDesc);

		m_Width = (float)texDesc.Width;
		m_Height = (float)texDesc.Height;
	}

	return SUCCEEDED(hr);
}

bool Texture::Initialize(const Graphics * const &pGraphics, const unsigned char * const pBytes, const UINT width, const UINT height, const UINT depth)
{
	ID3D11Texture2D* pTexture;
	D3D11_TEXTURE2D_DESC textureDesc;
	HRESULT hr;
	
	// Initialize the render target texture description.
	ZeroMemory(&textureDesc, sizeof(textureDesc));
	
	// Setup the render target texture description.
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 1;
	//textureDesc.MipLevels = 0;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	//textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	//textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
	textureDesc.MiscFlags = 0;
	
	D3D11_SUBRESOURCE_DATA res;
	res.SysMemPitch = width * depth;
	res.SysMemSlicePitch = width * height * depth;
	res.pSysMem = pBytes;
	
	// Create texture.
	//result = pGraphics->GetDevice()->CreateTexture2D(&textureDesc, nullptr, &Tex);
	hr = pGraphics->GetDevice()->CreateTexture2D(&textureDesc, &res, &pTexture);
	
	//pGraphics->GetContext()->UpdateSubresource(Tex, 0, nullptr, dib, (bmInfo.bmiHeader.biWidth * 4), 0);
	
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	//srvDesc.Texture2D.MipLevels = -1;//textureDesc.MipLevels;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;//textureDesc.MipLevels - 1;
	
	hr = pGraphics->GetDevice()->CreateShaderResourceView(pTexture, &srvDesc, m_pTexture.ReleaseAndGetAddressOf());
	//pGraphics->GetContext()->GenerateMips(pView);

	m_Width = (float)width;
	m_Height = (float)height;

	return SUCCEEDED(hr);
}

ID3D11ShaderResourceView *Texture::GetTexture() const
{
	return m_pTexture.Get();
}

float Texture::GetWidth() const
{
	return m_Width;
}

float Texture::GetHeight() const
{
	return m_Height;
}