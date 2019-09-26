#include "Shader.h"
#include "RendererD3D11.h"

namespace BVGraphics
{
	Shader::Shader()
	{
	}


	Shader::~Shader()
	{
	}


	GraphicsResult Shader::CompileFromFile(const char * const pFilename, const char * const pEntryPoint, const char * const pShaderAndVersion,
		ComPtr<ID3DBlob> & pCompiledShader, ComPtr<ID3DBlob> & pShaderError)
	{
		GraphicsResult result = GraphicsResult::SUCCESS;
		
		char fullPath[MAX_PATH] = {};
		GetCurrentDirectory(MAX_PATH, fullPath);
		char * pEnginePath = strstr(fullPath, "\\BVEngine\\");
		pEnginePath += strlen("\\BVEngine\\");
		pEnginePath[0] = 0;
		strcat_s(fullPath, MAX_PATH, "Data\\Shaders\\");
		strcat_s(fullPath, MAX_PATH, pFilename);

		wchar_t shaderFilename[MAX_PATH] = { 0 };
		size_t size;
		mbstowcs_s(&size, shaderFilename, strlen(fullPath) + 1, fullPath, _TRUNCATE);

		HRESULT hr = S_OK;
		unsigned int flags = 0;
#ifdef _DEBUG
		flags |= D3DCOMPILE_DEBUG;
#endif
		hr = D3DCompileFromFile(shaderFilename, nullptr, nullptr, pEntryPoint, pShaderAndVersion, flags, 0,
			pCompiledShader.ReleaseAndGetAddressOf(), pShaderError.ReleaseAndGetAddressOf());
		if (FAILED(hr))
		{
			result = GraphicsResult::SHADER_FAIL;
		}

		return result;
	}


	VertexShader::VertexShader()
	{
	}


	VertexShader::~VertexShader()
	{
		Release();
	}

	GraphicsResult VertexShader::CompileFromFile(const char * const pFilename, const char * const pEntryPoint,
		InputLayout * const pInputLayout, const D3D11_INPUT_ELEMENT_DESC * const pInputElem, const unsigned int elemCount)
	{
		ID3D11Device *pDevice = RendererD3D11::GetMainInstance()->GetDevice();
		BV_ASSERT(pDevice != nullptr, "VertexShader::CompileFromFile => D3D Device not initialized or assigned");

		GraphicsResult result = GraphicsResult::SUCCESS;

		ComPtr<ID3DBlob> pCompiledShader, pShaderError;
		do
		{
			result = Shader::CompileFromFile(pFilename, pEntryPoint, "vs_5_0", pCompiledShader, pShaderError);
			if (GraphicsResult::SHADER_FAIL == result)
			{
				break;
			}

			HRESULT hr = pDevice->CreateVertexShader(pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(),
				nullptr, m_pVertexShader.ReleaseAndGetAddressOf());
			if (FAILED(hr))
			{
				result = GraphicsResult::SHADER_FAIL;
				break;
			}

			if (pInputLayout)
			{
				hr = pDevice->CreateInputLayout(pInputElem, elemCount, pCompiledShader->GetBufferPointer(),
					pCompiledShader->GetBufferSize(), pInputLayout->m_pInputLayout.ReleaseAndGetAddressOf());
				if (FAILED(hr))
				{
					result = GraphicsResult::SHADER_FAIL;
					break;
				}
			}
		} while (0);

		return result;
	}

	void VertexShader::Release()
	{
		if (m_pVertexShader)
		{
			m_pVertexShader.Reset();
			m_pVertexShader = nullptr;
		}
	}


	HullShader::HullShader()
	{
	}


	HullShader::~HullShader()
	{
		Release();
	}


	GraphicsResult HullShader::CompileFromFile(const char * const pFilename, const char * const pEntryPoint)
	{
		ID3D11Device *pDevice = RendererD3D11::GetMainInstance()->GetDevice();
		BV_ASSERT(pDevice != nullptr, "HullShader::CompileFromFile => D3D Device not initialized or assigned");

		GraphicsResult result = GraphicsResult::SUCCESS;

		ComPtr<ID3DBlob> pCompiledShader, pShaderError;
		result = Shader::CompileFromFile(pFilename, pEntryPoint, "hs_5_0", pCompiledShader, pShaderError);

		if (GraphicsResult::SUCCESS == result)
		{
			HRESULT hr = pDevice->CreateHullShader(pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(),
				nullptr, m_pHullShader.ReleaseAndGetAddressOf());
			if (FAILED(hr))
			{
				result = GraphicsResult::SHADER_FAIL;
			}
		}

		return result;
	}


	void HullShader::Release()
	{
		if (m_pHullShader)
		{
			m_pHullShader.Reset();
			m_pHullShader = nullptr;
		}
	}


	DomainShader::DomainShader()
	{
	}


	DomainShader::~DomainShader()
	{
		Release();
	}


	GraphicsResult DomainShader::CompileFromFile(const char * const pFilename, const char * const pEntryPoint)
	{
		ID3D11Device *pDevice = RendererD3D11::GetMainInstance()->GetDevice();
		BV_ASSERT(pDevice != nullptr, "DomainShader::CompileFromFile => D3D Device not initialized or assigned");

		GraphicsResult result = GraphicsResult::SUCCESS;

		ComPtr<ID3DBlob> pCompiledShader, pShaderError;
		result = Shader::CompileFromFile(pFilename, pEntryPoint, "ds_5_0", pCompiledShader, pShaderError);

		if (GraphicsResult::SUCCESS == result)
		{
			HRESULT hr = pDevice->CreateDomainShader(pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(),
				nullptr, m_pDomainShader.ReleaseAndGetAddressOf());
			if (FAILED(hr))
			{
				result = GraphicsResult::SHADER_FAIL;
			}
		}

		return result;
	}


	void DomainShader::Release()
	{
		if (m_pDomainShader)
		{
			m_pDomainShader.Reset();
			m_pDomainShader = nullptr;
		}
	}


	GeometryShader::GeometryShader()
	{
	}


	GeometryShader::~GeometryShader()
	{
		Release();
	}


	GraphicsResult GeometryShader::CompileFromFile(const char * const pFilename, const char * const pEntryPoint)
	{
		ID3D11Device *pDevice = RendererD3D11::GetMainInstance()->GetDevice();
		BV_ASSERT(pDevice != nullptr, "GeometryShader::CompileFromFile => D3D Device not initialized or assigned");

		GraphicsResult result = GraphicsResult::SUCCESS;

		ComPtr<ID3DBlob> pCompiledShader, pShaderError;
		result = Shader::CompileFromFile(pFilename, pEntryPoint, "gs_5_0", pCompiledShader, pShaderError);

		if (GraphicsResult::SUCCESS == result)
		{
			HRESULT hr = pDevice->CreateGeometryShader(pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(),
				nullptr, m_pGeometryShader.ReleaseAndGetAddressOf());
			if (FAILED(hr))
			{
				result = GraphicsResult::SHADER_FAIL;
			}
		}

		return result;
	}


	void GeometryShader::Release()
	{
		if (m_pGeometryShader)
		{
			m_pGeometryShader.Reset();
			m_pGeometryShader = nullptr;
		}
	}


	PixelShader::PixelShader()
	{
	}


	PixelShader::~PixelShader()
	{
		Release();
	}

	GraphicsResult PixelShader::CompileFromFile(const char * const pFilename, const char * const pEntryPoint)
	{
		ID3D11Device *pDevice = RendererD3D11::GetMainInstance()->GetDevice();
		BV_ASSERT(pDevice != nullptr, "PixelShader::CompileFromFile => D3D Device not initialized or assigned");

		GraphicsResult result = GraphicsResult::SUCCESS;

		ComPtr<ID3DBlob> pCompiledShader, pShaderError;
		result = Shader::CompileFromFile(pFilename, pEntryPoint, "ps_5_0", pCompiledShader, pShaderError);

		if (GraphicsResult::SUCCESS == result)
		{
			HRESULT hr = pDevice->CreatePixelShader(pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(),
				nullptr, m_pPixelShader.ReleaseAndGetAddressOf());
			if (FAILED(hr))
			{
				result = GraphicsResult::SHADER_FAIL;
			}
		}

		return result;
	}

	void PixelShader::Release()
	{
		if (m_pPixelShader)
		{
			m_pPixelShader.Reset();
			m_pPixelShader = nullptr;
		}
	}


	ComputeShader::ComputeShader()
	{
	}


	ComputeShader::~ComputeShader()
	{
		Release();
	}


	GraphicsResult ComputeShader::CompileFromFile(const char * const pFilename, const char * const pEntryPoint)
	{
		ID3D11Device *pDevice = RendererD3D11::GetMainInstance()->GetDevice();
		BV_ASSERT(pDevice != nullptr, "ComputeShader::CompileFromFile => D3D Device not initialized or assigned");

		GraphicsResult result = GraphicsResult::SUCCESS;

		ComPtr<ID3DBlob> pCompiledShader, pShaderError;
		result = Shader::CompileFromFile(pFilename, pEntryPoint, "cs_5_0", pCompiledShader, pShaderError);

		if (GraphicsResult::SUCCESS == result)
		{
			HRESULT hr = pDevice->CreateComputeShader(pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(),
				nullptr, m_pComputeShader.ReleaseAndGetAddressOf());
			if (FAILED(hr))
			{
				result = GraphicsResult::SHADER_FAIL;
			}
		}

		return result;
	}


	void ComputeShader::Release()
	{
		if (m_pComputeShader)
		{
			m_pComputeShader.Reset();
			m_pComputeShader = nullptr;
		}
	}
}