#include "Model.h"

Model::Model()
	: m_NumVertices(0U), m_NumIndices(0U), m_VertexSize(0U)
{
}

Model::~Model()
{
}

bool Model::Initialize(const Graphics * const pGraphics, const void * const pVertices, const UINT vertexCount, const UINT vertexSize,
	const void * const pIndices, const UINT indexCount, const UINT indexSize, 
	const D3D11_USAGE vertexUsage, const D3D11_USAGE indexUsage,
	const UINT vertexAccess, const UINT indexAccess)
{
	ID3D11Device *pDevice = pGraphics->GetDevice();

	D3D11_BUFFER_DESC vertexDesc;
	ZeroMemory(&vertexDesc, sizeof(D3D11_BUFFER_DESC));
	vertexDesc.Usage = vertexUsage;
	vertexDesc.ByteWidth = vertexSize * vertexCount;
	vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexDesc.CPUAccessFlags = vertexAccess;
	vertexDesc.MiscFlags = 0;
	vertexDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = pVertices;

	if (FAILED(pDevice->CreateBuffer(&vertexDesc, &vertexData, m_pVertexBuffer.ReleaseAndGetAddressOf())))
	{
		return false;
	}

	D3D11_BUFFER_DESC indexDesc;
	ZeroMemory(&indexDesc, sizeof(D3D11_BUFFER_DESC));
	indexDesc.Usage = indexUsage;
	indexDesc.ByteWidth = indexSize * indexCount;
	indexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexDesc.CPUAccessFlags = indexAccess;
	indexDesc.MiscFlags = 0;
	indexDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = pIndices;

	if (FAILED(pDevice->CreateBuffer(&indexDesc, &indexData, m_pIndexBuffer.ReleaseAndGetAddressOf())))
	{
		return false;
	}

	return true;
}

void Model::Shutdown()
{
	m_pVertexBuffer.Reset();
	m_pIndexBuffer.Reset();
}

const UINT Model::GetNumVertices() const
{
	return m_NumVertices;
}

const UINT Model::GetNumIndices() const
{
	return m_NumIndices;
}