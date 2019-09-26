#include "Texture.h"
#include "RendererD3D11.h"

namespace BVGraphics
{
	Texture::Texture()
	{
	}


	Texture::~Texture()
	{
		Release();
	}


	GraphicsResult Texture::Create1D(const D3D11_TEXTURE1D_DESC & textureDesc, const D3D11_SUBRESOURCE_DATA & data)
	{
		ID3D11Device *pDevice = RendererD3D11::GetMainInstance()->GetDevice();
		BV_ASSERT(pDevice != nullptr, "Texture::Create1D => D3D Device not initialized or assigned");

		GraphicsResult result = GraphicsResult::SUCCESS;

		const D3D11_SUBRESOURCE_DATA *pData = data.pSysMem ? &data : nullptr;
		if (FAILED(pDevice->CreateTexture1D(&textureDesc, pData, m_pTexture1D.ReleaseAndGetAddressOf())))
		{
			result = GraphicsResult::TEXTURE_FAIL;
		}

		return result;
	}


	GraphicsResult Texture::Create2D(const D3D11_TEXTURE2D_DESC & textureDesc, const D3D11_SUBRESOURCE_DATA & data)
	{
		ID3D11Device *pDevice = RendererD3D11::GetMainInstance()->GetDevice();
		BV_ASSERT(pDevice != nullptr, "Texture::Create2D => D3D Device not initialized or assigned");

		GraphicsResult result = GraphicsResult::SUCCESS;

		const D3D11_SUBRESOURCE_DATA *pData = data.pSysMem ? &data : nullptr;
		if (FAILED(pDevice->CreateTexture2D(&textureDesc, pData, m_pTexture2D.ReleaseAndGetAddressOf())))
		{
			result = GraphicsResult::TEXTURE_FAIL;
		}

		return result;
	}


	GraphicsResult Texture::Create3D(const D3D11_TEXTURE3D_DESC & textureDesc, const D3D11_SUBRESOURCE_DATA & data)
	{
		ID3D11Device *pDevice = RendererD3D11::GetMainInstance()->GetDevice();
		BV_ASSERT(pDevice != nullptr, "Texture::Create3D => D3D Device not initialized or assigned");

		GraphicsResult result = GraphicsResult::SUCCESS;

		const D3D11_SUBRESOURCE_DATA *pData = data.pSysMem ? &data : nullptr;
		if (FAILED(pDevice->CreateTexture3D(&textureDesc, pData, m_pTexture3D.ReleaseAndGetAddressOf())))
		{
			result = GraphicsResult::TEXTURE_FAIL;
		}

		return result;
	}
	void Texture::Release()
	{
		if (m_pTexture1D)
		{
			m_pTexture1D.Reset();
			m_pTexture1D = nullptr;
		}

		if (m_pTexture2D)
		{
			m_pTexture2D.Reset();
			m_pTexture2D = nullptr;
		}

		if (m_pTexture3D)
		{
			m_pTexture3D.Reset();
			m_pTexture3D = nullptr;
		}
	}
}