#include "Shader.h"

Shader::Shader()
{
}

Shader::~Shader()
{
}

bool Shader::CompileShader(const Graphics * const pGraphics, const wchar_t * const pFileName, const ShaderType shaderType,
	const char * const pEntryPoint, void * const &pShader, ComPtr<ID3DBlob> *pCompiledShader,
	ComPtr<ID3DBlob> *pShaderError, const D3D11_SO_DECLARATION_ENTRY * const pSOEntry, const UINT numEntries)
{
	ID3D11Device *pDevice = pGraphics->GetDevice();

	ComPtr<ID3DBlob> pBlob;
	ComPtr<ID3DBlob> pErrorBlob;

	HRESULT hr = S_OK;
	hr = D3DCompileFromFile(pFileName, nullptr, nullptr, pEntryPoint, pGraphics->GetShaderNameAndVersion(shaderType), 0, 0, pBlob.ReleaseAndGetAddressOf(), pErrorBlob.ReleaseAndGetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	D3D_FEATURE_LEVEL featureLevel = pGraphics->GetFeatureLevel();
	switch (shaderType)
	{
	case ShaderType::VS:
		hr = pDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, (ID3D11VertexShader**)&pShader);
		break;
	case ShaderType::GS:
		hr = pDevice->CreateGeometryShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, (ID3D11GeometryShader**)&pShader);
		break;
	case ShaderType::GS_WITH_SO:
	case ShaderType::VS_WITH_SO:
		if (featureLevel == D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_10_0 || featureLevel == D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_10_1)
		{
			hr = pDevice->CreateGeometryShaderWithStreamOutput(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), pSOEntry, numEntries, nullptr, 0U, 0U, nullptr, (ID3D11GeometryShader**)&pShader);
		}
		else
		{
			hr = pDevice->CreateGeometryShaderWithStreamOutput(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), pSOEntry, numEntries, nullptr, 0U, D3D11_SO_NO_RASTERIZED_STREAM, nullptr, (ID3D11GeometryShader**)&pShader);
		}
		break;
	case ShaderType::HS:
		hr = pDevice->CreateHullShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, (ID3D11HullShader**)&pShader);
		break;
	case ShaderType::DS:
		hr = pDevice->CreateDomainShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, (ID3D11DomainShader**)&pShader);
		break;
	case ShaderType::PS:
		hr = pDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, (ID3D11PixelShader**)&pShader);
		break;
	case ShaderType::CS:
		hr = pDevice->CreateComputeShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, (ID3D11ComputeShader**)&pShader);
		break;
	}

	if (pCompiledShader)
	{
		*pCompiledShader = pBlob;
	}

	if (pShaderError)
	{
		*pShaderError = pErrorBlob;
	}

	return SUCCEEDED(hr);
}

bool Shader::CreateInputLayout(const Graphics * const pGraphics, const D3D11_INPUT_ELEMENT_DESC * const pVertexLayout, const UINT numElements, ID3DBlob * const pCompiledShader, ComPtr<ID3D11InputLayout> &pInputLayout)
{
	return SUCCEEDED(pGraphics->GetDevice()->CreateInputLayout(pVertexLayout, numElements,
		pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(), pInputLayout.ReleaseAndGetAddressOf()));
}