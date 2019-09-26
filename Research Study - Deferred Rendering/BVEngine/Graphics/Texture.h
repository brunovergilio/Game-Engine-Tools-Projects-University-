#ifndef TEXTURE_H
#define TEXTURE_H

#include "GraphicsUtils.h"

namespace BVGraphics
{
	class Texture
	{
	public:
		Texture();
		~Texture();

		GraphicsResult Create1D(const D3D11_TEXTURE1D_DESC & textureDesc, const D3D11_SUBRESOURCE_DATA & data = { 0 });
		GraphicsResult Create2D(const D3D11_TEXTURE2D_DESC & textureDesc, const D3D11_SUBRESOURCE_DATA & data = { 0 });
		GraphicsResult Create3D(const D3D11_TEXTURE3D_DESC & textureDesc, const D3D11_SUBRESOURCE_DATA & data = { 0 });

		void Release();

		inline ID3D11Resource * const Get1D() const { return m_pTexture1D.Get(); }
		inline ID3D11Resource * const Get2D() const { return m_pTexture2D.Get(); }
		inline ID3D11Resource * const Get3D() const { return m_pTexture3D.Get(); }

	private:
		Texture(const Texture & other) = delete;
		Texture(Texture && other) = delete;
		Texture & operator = (const Texture & other) = delete;
		Texture & operator = (Texture && other) = delete;

	private:
		ComPtr<ID3D11Texture1D> m_pTexture1D;
		ComPtr<ID3D11Texture2D> m_pTexture2D;
		ComPtr<ID3D11Texture3D> m_pTexture3D;
	};
}
#endif