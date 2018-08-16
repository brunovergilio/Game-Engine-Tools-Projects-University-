#include "SpriteShader.h"

SpriteShader::SpriteShader()
{
}

SpriteShader::~SpriteShader()
{
	if (m_pCBPerObject)
	{
		delete m_pCBPerObject;
	}
}

bool SpriteShader::Initialize(const Graphics * const pGraphics, const char * const pShaderName)
{
	char tempName[MAX_PATH];

	wchar_t vsShaderName[MAX_PATH];
	wchar_t psShaderName[MAX_PATH];
	size_t numConv = 0;

	GetCurrentDirectory(MAX_PATH, tempName);
	strcat_s(tempName, MAX_PATH, "\\Data\\HLSL\\");
	strcat_s(tempName, MAX_PATH, pShaderName);
	strcat_s(tempName, MAX_PATH, ".vs.hlsl");
	mbstowcs_s(&numConv, vsShaderName, strlen(tempName) + 1, tempName, _TRUNCATE);

	GetCurrentDirectory(MAX_PATH, tempName);
	strcat_s(tempName, MAX_PATH, "\\Data\\HLSL\\");
	strcat_s(tempName, MAX_PATH, pShaderName);
	strcat_s(tempName, MAX_PATH, ".ps.hlsl");
	mbstowcs_s(&numConv, psShaderName, strlen(tempName) + 1, tempName, _TRUNCATE);
	
	ComPtr<ID3DBlob> pCompiledShader;
	void *pShader = nullptr;
	if (!CompileShader(pGraphics, vsShaderName, ShaderType::VS, "VS", pShader, &pCompiledShader))
	{
		return false;
	}
	m_pVertexShader = (ID3D11VertexShader *)pShader;

	if (!CompileShader(pGraphics, psShaderName, ShaderType::PS, "PS", pShader))
	{
		return false;
	}
	m_pPixelShader = (ID3D11PixelShader *)pShader;

	D3D11_INPUT_ELEMENT_DESC vertexLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	if (!CreateInputLayout(pGraphics, vertexLayout, ARRAYSIZE(vertexLayout), pCompiledShader.Get(), m_pInputLayout))
	{
		return false;
	}

	m_pCBPerObject = new ConstantBuffer();
	if (!m_pCBPerObject->Initialize(pGraphics, sizeof(CBPerObject)))
	{
		return false;
	}

	return true;
}

void SpriteShader::MapData(const Graphics * const pGraphics, const ShaderParameters * const pShaderParameters)
{
	SpriteShaderParameters *pParams = (SpriteShaderParameters *)pShaderParameters;

	m_CBPerObject.projMatrix = pParams->projMatrix;
	m_CBPerObject.textureInfo = pParams->textureInfo;

	ID3D11DeviceContext *pContext = pGraphics->GetContext();

	pContext->IASetInputLayout(m_pInputLayout.Get());

	pContext->VSSetShader(m_pVertexShader.Get(), nullptr, 0);
	pContext->PSSetShader(m_pPixelShader.Get(), nullptr, 0);

	m_pCBPerObject->Map(pGraphics, &m_CBPerObject, sizeof(CBPerObject));

	ID3D11Buffer *pBuffers[] = { m_pCBPerObject->Get() };
	pContext->VSSetConstantBuffers(0, 1, pBuffers);

	ID3D11ShaderResourceView *pTextures[] = { pParams->pTexture->GetTexture() };
	pContext->PSSetShaderResources(0, 1, pTextures);

	ID3D11SamplerState *pSamplers[] = { pParams->pSampler->GetSampler() };
	pContext->PSSetSamplers(0, 1, pSamplers);
}