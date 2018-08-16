#ifndef SHADER_H
#define SHADER_H

#include "Graphics.h"

template<int N>
struct Alignment
{
	void * operator new(size_t size)
	{
		return _aligned_malloc(size, N);
	};

	void * operator new[](size_t size)
	{
		return operator new(size);
	};

	void operator delete(void *pMem)
	{
		_aligned_free(pMem);
	};

	void operator delete[](void *pMem)
	{
		return operator delete(pMem);
	};
};

struct ShaderParameters : public Alignment<16>
{
	ShaderParameters() {};
	virtual ~ShaderParameters() = 0 {};
};

class Shader
{
public:
	Shader();
	virtual ~Shader();
	virtual bool Initialize(const Graphics * const pGraphics, const char * const pShaderName) = 0;
	virtual void MapData(const Graphics * const pGraphics, const ShaderParameters * const pShaderParameters) = 0;

protected:
	bool CompileShader(const Graphics * const pGraphics, const wchar_t * const pFileName, const ShaderType shaderType,
		const char * const pEntryPoint, void * const &pShader, ComPtr<ID3DBlob> *pCompiledShader = nullptr,
		ComPtr<ID3DBlob> *pShaderError = nullptr, const D3D11_SO_DECLARATION_ENTRY * const pSOEntry = nullptr, const UINT numEntries = 0U);
	bool CreateInputLayout(const Graphics * const pGraphics, const D3D11_INPUT_ELEMENT_DESC * const pVertexLayout, const UINT numElements, ID3DBlob * const pCompiledShader, ComPtr<ID3D11InputLayout> &pInputLayout);

private:
	// Not needed
	Shader(const Shader &) = delete;
	Shader & operator = (const Shader &) = delete;

protected:
	ComPtr<ID3D11InputLayout> m_pInputLayout;
	ComPtr<ID3D11VertexShader> m_pVertexShader;
	ComPtr<ID3D11PixelShader> m_pPixelShader;
};

#endif