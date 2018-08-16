#include "SpriteModel.h"

SpriteModel::SpriteModel(const UINT spriteBatchSize)
	: m_SpriteBatchSize(spriteBatchSize)
{
}

SpriteModel::~SpriteModel()
{
}

bool SpriteModel::Initialize(const Graphics * const pGraphics)
{
	m_VertexSize = sizeof(Vertex2D);

	m_NumVertices = 4;
	m_NumIndices = 6;

	UINT *pIndices = new UINT[6 * m_SpriteBatchSize];
	for (int i = 0; i < m_SpriteBatchSize; i++)
	{
		// Left handed
		//pIndices[i * 6 + 0] = i * 4 + 0;
		//pIndices[i * 6 + 1] = i * 4 + 1;
		//pIndices[i * 6 + 2] = i * 4 + 2;

		//pIndices[i * 6 + 3] = i * 4 + 0;
		//pIndices[i * 6 + 4] = i * 4 + 2;
		//pIndices[i * 6 + 5] = i * 4 + 3;

		// Right handed
		pIndices[i * 6 + 2] = i * 4 + 0;
		pIndices[i * 6 + 1] = i * 4 + 1;
		pIndices[i * 6 + 0] = i * 4 + 2;

		pIndices[i * 6 + 5] = i * 4 + 0;
		pIndices[i * 6 + 4] = i * 4 + 2;
		pIndices[i * 6 + 3] = i * 4 + 3;
	}

	ID3D11Device *pDevice = pGraphics->GetDevice();

	D3D11_BUFFER_DESC vertexDesc;
	ZeroMemory(&vertexDesc, sizeof(D3D11_BUFFER_DESC));
	vertexDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexDesc.ByteWidth = m_VertexSize * m_NumVertices * m_SpriteBatchSize;
	vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexDesc.MiscFlags = 0;
	vertexDesc.StructureByteStride = 0;

	if (FAILED(pDevice->CreateBuffer(&vertexDesc, nullptr, m_pVertexBuffer.ReleaseAndGetAddressOf())))
	{
		return false;
	}

	D3D11_BUFFER_DESC indexDesc;
	ZeroMemory(&indexDesc, sizeof(D3D11_BUFFER_DESC));
	indexDesc.Usage = D3D11_USAGE_IMMUTABLE;
	indexDesc.ByteWidth = sizeof(UINT) * m_NumIndices * m_SpriteBatchSize;
	indexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexDesc.CPUAccessFlags = 0;
	indexDesc.MiscFlags = 0;
	indexDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = pIndices;

	if (FAILED(pDevice->CreateBuffer(&indexDesc, &indexData, m_pIndexBuffer.ReleaseAndGetAddressOf())))
	{
		delete[] pIndices;
		return false;
	}
	delete[] pIndices;

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
	depthStencilDesc.DepthEnable = false;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	if (FAILED(pDevice->CreateDepthStencilState(&depthStencilDesc, m_DepthStencilState.ReleaseAndGetAddressOf())))
	{
		return false;
	}

	D3D11_BLEND_DESC transparentDesc = { 0 };
	transparentDesc.AlphaToCoverageEnable = false;
	transparentDesc.IndependentBlendEnable = false;

	transparentDesc.RenderTarget[0].BlendEnable = true;
	transparentDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	transparentDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	transparentDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	transparentDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	transparentDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	transparentDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	transparentDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	if (FAILED(pDevice->CreateBlendState(&transparentDesc, m_pBlendState.ReleaseAndGetAddressOf())))
	{
		return false;
	}

	return true;
}

void SpriteModel::MapSprites(const Graphics * const pGraphics, const Vertex2D * const pVertices, const UINT numSprites)
{
	ID3D11DeviceContext *pContext = pGraphics->GetContext();

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (SUCCEEDED(pContext->Map(m_pVertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		Vertex2D * const pVertexData = (Vertex2D *)mappedResource.pData;
		for (size_t i = 0; i < numSprites; i++)
		{
			pVertexData[i * 4 + 0] = pVertices[i * 4 + 0];
			pVertexData[i * 4 + 1] = pVertices[i * 4 + 1];
			pVertexData[i * 4 + 2] = pVertices[i * 4 + 2];
			pVertexData[i * 4 + 3] = pVertices[i * 4 + 3];
		}
		pContext->Unmap(m_pVertexBuffer.Get(), 0);
	}
}

void SpriteModel::Draw(const Graphics * const pGraphics)
{
	UNREFERENCED_PARAMETER(pGraphics);
}

void SpriteModel::Draw(const Graphics * const pGraphics, const UINT numSprites)
{
	ID3D11DeviceContext *pContext = pGraphics->GetContext();

	UINT offset = 0;
	float blendFactor[4] =
	{
		1.0f
	};

	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pContext->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), &m_VertexSize, &offset);
	pContext->IASetIndexBuffer(m_pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	ID3D11DepthStencilState *pPrevDepthStencilState;
	pContext->OMGetDepthStencilState(&pPrevDepthStencilState, nullptr);
	pContext->OMSetDepthStencilState(m_DepthStencilState.Get(), 0);

	ID3D11BlendState *pPrevBlendState;
	float prevBlendFactor[4];
	UINT prevSampleMask;
	pContext->OMGetBlendState(&pPrevBlendState, prevBlendFactor, &prevSampleMask);
	pContext->OMSetBlendState(m_pBlendState.Get(), blendFactor, 0xffffffff);

	pContext->DrawIndexed(6 * numSprites, 0, 0);

	pContext->OMSetDepthStencilState(pPrevDepthStencilState, 0);
	pContext->OMSetBlendState(pPrevBlendState, prevBlendFactor, prevSampleMask);
}