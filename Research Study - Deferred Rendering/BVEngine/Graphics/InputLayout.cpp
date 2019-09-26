#include "InputLayout.h"
#include "RendererD3D11.h"

namespace BVGraphics
{
	InputLayout::InputLayout()
	{
	}


	InputLayout::~InputLayout()
	{
		Release();
	}


	GraphicsResult BVGraphics::InputLayout::CompileFromFile(const char * const pFilename, const char * const pEntryPoint, const D3D11_INPUT_ELEMENT_DESC * const pInputElem, const size_t elemCount)
	{
		ID3D11Device *pDevice = RendererD3D11::GetMainInstance()->GetDevice();
		BV_ASSERT(pDevice != nullptr, "VertexShader::CompileFromFile => D3D Device not initialized or assigned");

		GraphicsResult result = GraphicsResult::SUCCESS;

		wchar_t shaderFilename[MAX_PATH] = { 0 };
		size_t size;
		mbstowcs_s(&size, shaderFilename, strlen(pFilename) + 1, pFilename, _TRUNCATE);

		unsigned int flags = 0;
#ifdef _DEBUG
		flags |= D3DCOMPILE_DEBUG;
#endif
		ComPtr<ID3DBlob> pCompiledShader, pShaderError;
		HRESULT hr = S_OK;
		do
		{
			hr = D3DCompileFromFile(shaderFilename, nullptr, nullptr, pEntryPoint, "vs_5_0", flags, 0,
				pCompiledShader.ReleaseAndGetAddressOf(), pShaderError.ReleaseAndGetAddressOf());
			if (FAILED(hr))
			{
				result = GraphicsResult::SHADER_FAIL;
				break;
			}

			hr = pDevice->CreateInputLayout(pInputElem, elemCount, pCompiledShader->GetBufferPointer(),
				pCompiledShader->GetBufferSize(), m_pInputLayout.ReleaseAndGetAddressOf());
			if (FAILED(hr))
			{
				result = GraphicsResult::SHADER_FAIL;
				break;
			}
		} while (0);

		return result;
	}
	void InputLayout::Release()
	{
		if (m_pInputLayout)
		{
			m_pInputLayout.Reset();
			m_pInputLayout = nullptr;
		}
	}
}