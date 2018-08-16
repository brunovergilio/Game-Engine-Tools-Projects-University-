#include "BasicShader.h"

BasicShader::BasicShader()
	: m_pCBPerObject(nullptr), m_pCBPerFrame(nullptr), m_pCBOptions(nullptr)
{
}

BasicShader::~BasicShader()
{
	if (m_pCBPerObject)
	{
		delete m_pCBPerObject;
	}

	if (m_pCBPerFrame)
	{
		delete m_pCBPerFrame;
	}

	if (m_pCBOptions)
	{
		delete m_pCBOptions;
	}
}

bool BasicShader::Initialize(const Graphics * const pGraphics, const char * const pShaderName)
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
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	if (!CreateInputLayout(pGraphics, vertexLayout, ARRAYSIZE(vertexLayout),  pCompiledShader.Get(), m_pInputLayout))
	{
		return false;
	}

	m_pCBPerObject = new ConstantBuffer();
	if (!m_pCBPerObject->Initialize(pGraphics, sizeof(CBPerObject)))
	{
		return false;
	}

	m_pCBPerFrame = new ConstantBuffer();
	if (!m_pCBPerFrame->Initialize(pGraphics, sizeof(CBPerFrame)))
	{
		return false;
	}

	m_pCBOptions = new ConstantBuffer();
	if (!m_pCBOptions->Initialize(pGraphics, sizeof(CBOptions)))
	{
		return false;
	}

	return true;
}

void BasicShader::MapData(const Graphics * const pGraphics, const ShaderParameters * const pShaderParameters)
{
	BasicShaderParameters *pParams = (BasicShaderParameters *)pShaderParameters;

	m_CBPerObject.WVPMatrix = pParams->wvp;
	m_CBPerObject.WorldMatrix = pParams->world;
	m_CBPerObject.InvWorld = pParams->invWorld;
	m_CBPerObject.Material = pParams->material;

	m_CBOptions.useColor = pParams->useColor;
	m_CBOptions.useTexture = pParams->useTexture;
	m_CBOptions.useLight = pParams->useLight;
	m_CBOptions.useNormalMapping = pParams->useNormalMapping;

	ID3D11DeviceContext *pContext = pGraphics->GetContext();

	pContext->IASetInputLayout(m_pInputLayout.Get());

	pContext->VSSetShader(m_pVertexShader.Get(), nullptr, 0);
	pContext->PSSetShader(m_pPixelShader.Get(), nullptr, 0);

	m_pCBPerObject->Map(pGraphics, &m_CBPerObject, sizeof(CBPerObject));

	m_pCBPerFrame->Map(pGraphics, &m_CBPerFrame, sizeof(CBPerFrame));

	m_pCBOptions->Map(pGraphics, &m_CBOptions, sizeof(CBOptions));

	ID3D11Buffer *pVSBuffers[] = { m_pCBPerObject->Get() };
	pContext->VSSetConstantBuffers(0, 1, pVSBuffers);

	ID3D11Buffer *pPSBuffers[] = { m_pCBPerObject->Get(), m_pCBPerFrame->Get(), m_pCBOptions->Get() };
	pContext->PSSetConstantBuffers(0, 3, pPSBuffers);

	ID3D11ShaderResourceView *pTextures[] = { nullptr, nullptr };
	if (m_CBOptions.useTexture && pParams->pTexture && pParams->pSampler)
	{
		pTextures[0] = pParams->pTexture->GetTexture();
	}

	if (m_CBOptions.useNormalMapping && pParams->pNormalMap && pParams->pSampler)
	{
		pTextures[1] = pParams->pNormalMap->GetTexture();
	}

	pContext->PSSetShaderResources(0, 2, pTextures);

	if (pParams->pSampler)
	{
		ID3D11SamplerState *pSamplers[] = { pParams->pSampler->GetSampler() };
		pContext->PSSetSamplers(0, 1, pSamplers);
	}
}

void BasicShader::SetPerFrameData(PointLight pointLight, XMFLOAT3 viewPos)
{
	m_CBPerFrame.PointLight = pointLight;
	m_CBPerFrame.ViewPos = viewPos;
}