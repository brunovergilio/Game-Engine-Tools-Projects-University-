#include "BasicGameObject.h"

BasicGameObject::BasicGameObject()
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	m_pShaderParameters = new BasicShaderParameters();
}

BasicGameObject::BasicGameObject(BDVModel * const pModel, BasicShader * const pShader)
	: GameObject(pModel, pShader)
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	m_pShaderParameters = new BasicShaderParameters();
}

BasicGameObject::BasicGameObject(BDVModel * const pModel, BasicShader * const pShader, const XMFLOAT4 &color, Texture * const pTexture, Sampler * const pSampler, const Material &material)
	: GameObject(pModel, pShader)
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	m_pShaderParameters = new BasicShaderParameters();
	m_pShaderParameters->material = material;
	m_pShaderParameters->material.m_Color = color;
	m_pShaderParameters->pTexture = pTexture;
	m_pShaderParameters->pSampler = pSampler;
}

BasicGameObject::~BasicGameObject()
{
	delete m_pShaderParameters;
}

void BasicGameObject::Update(const float dt)
{
	UNREFERENCED_PARAMETER(dt);
}

void BasicGameObject::Render(const Graphics * const pGraphics, Camera * const pCamera)
{
	if (!(m_pModel && m_pShader))
	{
		return;
	}

	XMStoreFloat4x4(&m_pShaderParameters->wvp, XMLoadFloat4x4(&m_WorldMatrix) * pCamera->GetViewMatrixXM() * pCamera->GetProjMatrixXM());
	m_pShaderParameters->world = m_WorldMatrix;
	XMMATRIX inv = XMLoadFloat4x4(&m_WorldMatrix);
	inv.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	XMStoreFloat4x4(&m_pShaderParameters->invWorld, XMMatrixTranspose(XMMatrixInverse(nullptr, inv)));

	m_pShader->MapData(pGraphics, m_pShaderParameters);

	m_pModel->Draw(pGraphics);
}

void BasicGameObject::SetColor(const XMFLOAT4 &color)
{
	m_pShaderParameters->material.m_Color = color;
}

void BasicGameObject::SetTexture(Texture * const pTexture)
{
	m_pShaderParameters->pTexture = pTexture;
}

void BasicGameObject::SetSampler(Sampler * const pSampler)
{
	m_pShaderParameters->pSampler = pSampler;
}

void BasicGameObject::SetMaterial(const Material &material)
{
	m_pShaderParameters->material = material;
}

void BasicGameObject::SetNormalMap(Texture * const pTexture)
{
	m_pShaderParameters->pNormalMap = pTexture;
}

void BasicGameObject::UseColor(const bool value)
{
	m_pShaderParameters->useColor = value;
}

void BasicGameObject::UseTexture(const bool value)
{
	m_pShaderParameters->useTexture = value;
}

void BasicGameObject::UseLight(const bool value)
{
	m_pShaderParameters->useLight = value;
}

void BasicGameObject::UseNormalMapping(const bool value)
{
	m_pShaderParameters->useNormalMapping = value;
}

bool BasicGameObject::UseColor() const
{
	return m_pShaderParameters->useColor;
}

bool BasicGameObject::UseTexture() const
{
	return m_pShaderParameters->useTexture;
}

bool BasicGameObject::UseLight() const
{
	return m_pShaderParameters->useLight;
}

bool BasicGameObject::UseNormalMapping() const
{
	return m_pShaderParameters->useNormalMapping;
}