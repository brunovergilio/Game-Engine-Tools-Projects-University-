#ifndef SHADERRESOURCEVIEW_H
#define SHADERRESOURCEVIEW_H

#include "GraphicsUtils.h"

namespace BVGraphics
{
	class ShaderResourceView
	{
	public:
		ShaderResourceView();
		~ShaderResourceView();

		GraphicsResult Create(ID3D11Resource * const pResource, const D3D11_SHADER_RESOURCE_VIEW_DESC & shaderResourceViewDesc);
		GraphicsResult CreateFromFile(const char * const pFileName);
		GraphicsResult CreateFromMemory(const unsigned char * const pBytes, const unsigned int width,
			const unsigned int height, const unsigned int depth);

		void Release();

		inline void Set(ID3D11ShaderResourceView * const pSRV) { m_pShaderResourceView = pSRV; }
		inline ID3D11ShaderResourceView * const Get() const { return m_pShaderResourceView.Get(); }
		inline ID3D11ShaderResourceView * const * const GetAddress() const { return m_pShaderResourceView.GetAddressOf(); }

	private:
		ShaderResourceView(const ShaderResourceView & other) = delete;
		ShaderResourceView(ShaderResourceView && other) = delete;
		ShaderResourceView & operator = (const ShaderResourceView & other) = delete;
		ShaderResourceView & operator = (ShaderResourceView && other) = delete;

	private:
		ComPtr<ID3D11ShaderResourceView> m_pShaderResourceView;
	};
}
#endif