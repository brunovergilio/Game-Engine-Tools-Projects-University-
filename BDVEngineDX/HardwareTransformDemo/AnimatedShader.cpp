#include "AnimatedShader.h"

AnimatedShader::AnimatedShader()
{
}

AnimatedShader::~AnimatedShader()
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

	if (m_pCBSkin)
	{
		delete m_pCBSkin;
	}

	if (m_pCBInterpolationInfo)
	{
		delete m_pCBInterpolationInfo;
	}
}

bool AnimatedShader::Initialize(const Graphics * const pGraphics, const char * const pShaderName)
{
	char tempName[MAX_PATH];

	wchar_t vsTransformShaderName[MAX_PATH];
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
		{ "BONEINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
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

	m_pCBPerFrame = new ConstantBuffer();
	if (!m_pCBPerFrame->Initialize(pGraphics, sizeof(CBPerFrame)))
	{
		return false;
	}

	m_pCBSkin = new ConstantBuffer();
	if (!m_pCBSkin->Initialize(pGraphics, sizeof(XMFLOAT4X4) * MAX_NUM_JOINTS))
	{
		return false;
	}

	m_pCBOptions = new ConstantBuffer();
	if (!m_pCBOptions->Initialize(pGraphics, sizeof(CBOptions)))
	{
		return false;
	}

	//////////////////////
	// Interpolation Stuff
	//////////////////////

	////////////////
	// Input Buffer
	D3D11_BUFFER_DESC vbDesc = { 0 };
	vbDesc.ByteWidth = sizeof(int) * 2 * MAX_NUM_JOINTS;
	vbDesc.Usage = D3D11_USAGE_DEFAULT;
	vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	struct JointInfo
	{
		int jointIndex;
		int numJoints;
	} *pJointInfo;

	pJointInfo = new JointInfo[MAX_NUM_JOINTS];
	for (int i = 0; i < MAX_NUM_JOINTS; i++)
	{
		pJointInfo[i].jointIndex = i;
		pJointInfo[i].numJoints = MAX_NUM_JOINTS;
	}

	D3D11_SUBRESOURCE_DATA vbData = { 0 };
	vbData.pSysMem = (void *)pJointInfo;

	ID3D11Device *pDevice = pGraphics->GetDevice();
	if (FAILED(pDevice->CreateBuffer(&vbDesc, &vbData, m_pJointBuffer.ReleaseAndGetAddressOf())))
	{
		return false;
	}

	delete[] pJointInfo;

	/////////////////////
	// Stream-Out Buffer
	D3D11_BUFFER_DESC streamOutDesc = { 0 };
	streamOutDesc.ByteWidth = sizeof(XMFLOAT4X4) * MAX_NUM_JOINTS;
	streamOutDesc.Usage = D3D11_USAGE_DEFAULT;
	streamOutDesc.BindFlags = D3D11_BIND_STREAM_OUTPUT;

	if (FAILED(pDevice->CreateBuffer(&streamOutDesc, nullptr, m_pStreamOutBuffer.ReleaseAndGetAddressOf())))
	{
		return false;
	}

	///////////
	// Shaders
	GetCurrentDirectory(MAX_PATH, tempName);
	strcat_s(tempName, MAX_PATH, "\\Data\\HLSL\\");
	strcat_s(tempName, MAX_PATH, pShaderName);
	strcat_s(tempName, MAX_PATH, "Transform");
	strcat_s(tempName, MAX_PATH, ".vs.hlsl");
	mbstowcs_s(&numConv, vsTransformShaderName, strlen(tempName) + 1, tempName, _TRUNCATE);

	ComPtr<ID3DBlob> pCompiledTransformShader;
	if (!CompileShader(pGraphics, vsTransformShaderName, ShaderType::VS, "VS", pShader, &pCompiledTransformShader))
	{
		return false;
	}
	m_pVSTransform = (ID3D11VertexShader *)pShader;

	D3D11_SO_DECLARATION_ENTRY entry[] =
	{
		{ 0, "ROWA", 0, 0, 4, 0 },
		{ 0, "ROWB", 0, 0, 4, 0 },
		{ 0, "ROWC", 0, 0, 4, 0 },
		{ 0, "ROWD", 0, 0, 4, 0 }
	};
	if (!CompileShader(pGraphics, vsTransformShaderName, ShaderType::VS_WITH_SO, "VS", pShader, nullptr, nullptr, entry, ARRAYSIZE(entry)))
	{
		return false;
	}
	m_pGSTransform = (ID3D11GeometryShader *)pShader;

	D3D11_INPUT_ELEMENT_DESC vertexTransformLayout[] =
	{
		{ "JOINTINDEX", 0, DXGI_FORMAT_R32_SINT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NUMJOINTS", 0, DXGI_FORMAT_R32_SINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	if (!CreateInputLayout(pGraphics, vertexTransformLayout, ARRAYSIZE(vertexTransformLayout), pCompiledTransformShader.Get(), m_pILTransform))
	{
		return false;
	}

	////////////////////
	// Constant Buffers
	m_pCBInterpolationInfo = new ConstantBuffer();
	if (!m_pCBInterpolationInfo->Initialize(pGraphics, sizeof(float) * 4))
	{
		return false;
	}

	return true;
}

void AnimatedShader::MapData(const Graphics * const pGraphics, const ShaderParameters * const pShaderParameters)
{
	AnimatedShaderParameters *pParams = (AnimatedShaderParameters *)pShaderParameters;

	m_CBPerObject.WVPMatrix = pParams->wvp;
	m_CBPerObject.WorldMatrix = pParams->world;
	m_CBPerObject.InvWorld = pParams->invWorld;
	m_CBPerObject.Material = pParams->material;

	m_CBOptions.useColor = pParams->useColor;
	m_CBOptions.useTexture = pParams->useTexture;
	m_CBOptions.useLight = pParams->useLight;

	if (pParams->transformOnHardware)
	{
		Interpolate(pGraphics, ((AnimatedShaderParameters *)pShaderParameters)->pInterpolationData);
	}
	else
	{
		m_pCBSkin->Map(pGraphics, pParams->pFinalMatrices, sizeof(XMFLOAT4X4) * pParams->numJoints);
	}

	ID3D11DeviceContext *pContext = pGraphics->GetContext();

	pContext->IASetInputLayout(m_pInputLayout.Get());

	pContext->VSSetShader(m_pVertexShader.Get(), nullptr, 0);
	pContext->PSSetShader(m_pPixelShader.Get(), nullptr, 0);

	m_pCBPerObject->Map(pGraphics, &m_CBPerObject, sizeof(CBPerObject));

	m_pCBPerFrame->Map(pGraphics, &m_CBPerFrame, sizeof(CBPerFrame));

	m_pCBOptions->Map(pGraphics, &m_CBOptions, sizeof(CBOptions));

	ID3D11Buffer *pVSBuffers[] = { m_pCBPerObject->Get(), m_pCBSkin->Get() };
	pContext->VSSetConstantBuffers(0, 2, pVSBuffers);

	ID3D11Buffer *pPSBuffers[] = { m_pCBPerObject->Get(), m_pCBPerFrame->Get(), m_pCBOptions->Get() };
	pContext->PSSetConstantBuffers(0, 3, pPSBuffers);

	if (m_CBOptions.useTexture && pParams->pTexture && pParams->pSampler)
	{
		ID3D11ShaderResourceView *pTextures[] = { pParams->pTexture->GetTexture() };
		pContext->PSSetShaderResources(0, 1, pTextures);

		ID3D11SamplerState *pSamplers[] = { pParams->pSampler->GetSampler() };
		pContext->PSSetSamplers(0, 1, pSamplers);
	}
}

void AnimatedShader::Interpolate(const Graphics * const pGraphics, const ShaderInterpolationData * const pShaderInterpolationData)
{
	ID3D11DeviceContext *pContext = pGraphics->GetContext();

	m_pCBInterpolationInfo->Map(pGraphics, &pShaderInterpolationData->lerp, sizeof(float) * 2);
	ID3D11Buffer *pBuffers[] = { m_pCBInterpolationInfo->Get(), pShaderInterpolationData->pBeginAnimBuffer, 
		pShaderInterpolationData->pEndAnimBuffer, pShaderInterpolationData->pJointsBuffer };

	pContext->VSSetConstantBuffers(0, 4, pBuffers);

	pContext->VSSetShader(m_pVSTransform.Get(), nullptr, 0);
	pContext->GSSetShader(m_pGSTransform.Get(), nullptr, 0);
	pContext->PSSetShader(nullptr, nullptr, 0);

	UINT stride = sizeof(float) * 2;
	UINT offset[] = { 0 };
	pContext->IASetVertexBuffers(0, 1, m_pJointBuffer.GetAddressOf(), &stride, offset);
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	pContext->IASetInputLayout(m_pILTransform.Get());

	pContext->SOSetTargets(1, m_pStreamOutBuffer.GetAddressOf(), offset);

	pContext->Draw(pShaderInterpolationData->numJoints, 0);

	ID3D11Buffer *pNullBuffer[] = { nullptr };
	pContext->SOSetTargets(1, pNullBuffer, offset);
	pContext->GSSetShader(nullptr, nullptr, 0);
	
	//ID3D11Buffer *pNullBuffers[] = { nullptr, nullptr, nullptr, nullptr };
	//pContext->VSSetConstantBuffers(0, 4, pNullBuffers);

	pContext->CopyResource(m_pCBSkin->Get(), m_pStreamOutBuffer.Get());
}

void AnimatedShader::SetPerFrameData(PointLight pointLight, XMFLOAT3 viewPos)
{
	m_CBPerFrame.PointLight = pointLight;
	m_CBPerFrame.ViewPos = viewPos;
}