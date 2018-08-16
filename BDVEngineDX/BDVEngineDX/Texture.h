#ifndef TEXTURE_H
#define TEXTURE_H

#include "Graphics.h"

class Texture
{
public:
	Texture();
	Texture(ID3D11ShaderResourceView *pView);
	~Texture();
	
	bool Initialize(const Graphics * const &pGraphics, const char * const pFileName);
	bool Initialize(const Graphics * const &pGraphics, const unsigned char * const pBytes, const UINT width, const UINT height, const UINT depth);

	ID3D11ShaderResourceView *GetTexture() const;
	
	float GetWidth() const;
	float GetHeight() const;

private:
	// Not needed
	Texture(const Texture & texture) = delete;
	Texture & operator = (const Texture & texture) = delete;

private:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pTexture;

	float m_Width;
	float m_Height;
};

#endif // !_TEXTURE_H_