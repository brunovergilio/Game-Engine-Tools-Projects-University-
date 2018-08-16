#include "GameObject.h"

GameObject::GameObject()
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
}

GameObject::GameObject(Model * const pModel, Shader * const pShader)
	: m_pModel(pModel), m_pShader(pShader)
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
}

GameObject::~GameObject()
{
}

void GameObject::SetModel(Model * const pModel)
{
	m_pModel = pModel;
}

void GameObject::SetShader(Shader * const pShader)
{
	m_pShader = pShader;
}

void GameObject::SetWorldMatrix(CXMMATRIX world)
{
	XMStoreFloat4x4(&m_WorldMatrix, world);
}