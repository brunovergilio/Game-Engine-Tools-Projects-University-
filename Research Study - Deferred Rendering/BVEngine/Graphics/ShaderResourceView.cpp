#include "ShaderResourceView.h"
#include "RendererD3D11.h"

#include "DirectXTex\DDSTextureLoader\DDSTextureLoader.h"
#include "DirectXTex\WICTextureLoader\WICTextureLoader.h"

using namespace DirectX;

namespace BVGraphics
{
	ShaderResourceView::ShaderResourceView()
	{
	}


	ShaderResourceView::~ShaderResourceView()
	{
		Release();
	}

	GraphicsResult ShaderResourceView::Create(ID3D11Resource * const pResource, const D3D11_SHADER_RESOURCE_VIEW_DESC & shaderResourceViewDesc)
	{
		ID3D11Device *pDevice = RendererD3D11::GetMainInstance()->GetDevice();
		BV_ASSERT(pDevice != nullptr, "ShaderResourceView::Create => D3D Device not initialized or assigned");

		GraphicsResult result = GraphicsResult::SUCCESS;

		if (FAILED(pDevice->CreateShaderResourceView(pResource, &shaderResourceViewDesc, m_pShaderResourceView.ReleaseAndGetAddressOf())))
		{
			result = GraphicsResult::SHADER_RESOURCE_VIEW_FAIL;
		}

		return result;
	}

	GraphicsResult ShaderResourceView::CreateFromFile(const char * const pFilename)
	{
		ID3D11Device *pDevice = RendererD3D11::GetMainInstance()->GetDevice();
		BV_ASSERT(pDevice != nullptr, "ShaderResourceView::CreateFromFile => D3D Device not initialized or assigned");

		GraphicsResult result = GraphicsResult::SUCCESS;

		char fullPath[MAX_PATH] = {};
		GetCurrentDirectory(MAX_PATH, fullPath);
		char * pEnginePath = strstr(fullPath, "\\BVEngine\\");
		pEnginePath += strlen("\\BVEngine\\");
		pEnginePath[0] = 0;
		strcat_s(fullPath, MAX_PATH, "Data\\Textures\\");
		strcat_s(fullPath, MAX_PATH, pFilename);

		wchar_t textureName[MAX_PATH];
		size_t size = 0;
		mbstowcs_s(&size, textureName, strlen(fullPath) + 1, fullPath, _TRUNCATE);

		HRESULT hr = S_OK;
		if (strstr(fullPath, ".dds"))
		{
			hr = CreateDDSTextureFromFile(pDevice, textureName, nullptr, m_pShaderResourceView.ReleaseAndGetAddressOf());
		}
		else
		{
			hr = CreateWICTextureFromFile(pDevice, textureName, nullptr, m_pShaderResourceView.ReleaseAndGetAddressOf());
		}

		if (FAILED(hr))
		{
			result = GraphicsResult::SHADER_RESOURCE_VIEW_FAIL;
		}

		return result;
	}

	GraphicsResult ShaderResourceView::CreateFromMemory(const unsigned char * const pBytes, const unsigned int width, const unsigned int height, const unsigned int depth)
	{
		ID3D11Device *pDevice = RendererD3D11::GetMainInstance()->GetDevice();
		BV_ASSERT(pDevice != nullptr, "ShaderResourceView::CreateFromMemory => D3D Device not initialized or assigned");

		GraphicsResult result = GraphicsResult::SUCCESS;

		ComPtr<ID3D11Texture2D> pTexture;

		D3D11_TEXTURE2D_DESC textureDesc;
		ZeroMemory(&textureDesc, sizeof(textureDesc));
		textureDesc.Width = width;
		textureDesc.Height = height;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA res;
		res.SysMemPitch = width * depth;
		res.SysMemSlicePitch = width * height * depth;
		res.pSysMem = pBytes;

		if (FAILED(pDevice->CreateTexture2D(&textureDesc, &res, pTexture.ReleaseAndGetAddressOf())))
		{
			result = GraphicsResult::SHADER_RESOURCE_VIEW_FAIL;
		}

		if (GraphicsResult::SUCCESS == result)
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
			srvDesc.Format = textureDesc.Format;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MipLevels = 1;
			srvDesc.Texture2D.MostDetailedMip = 0;

			if (FAILED(pDevice->CreateShaderResourceView(pTexture.Get(), &srvDesc, m_pShaderResourceView.ReleaseAndGetAddressOf())))
			{
				result = GraphicsResult::SHADER_RESOURCE_VIEW_FAIL;
			}
		}

		return result;
	}
	void ShaderResourceView::Release()
	{
		if (m_pShaderResourceView)
		{
			m_pShaderResourceView.Reset();
			m_pShaderResourceView = nullptr;
		}
	}
}