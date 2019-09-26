#ifndef RENDERINFO_H
#define RENDERINFO_H

#include "RasterizerState.h"
#include "BlendState.h"
#include "DepthStencilState.h"

namespace BVGraphics
{
	enum class DrawCallType
	{
		VERTEX,
		INDEXED
	};

	struct MeshData
	{
		MeshData() { ZeroMemory(this, sizeof(MeshData)); }
		~MeshData() {}

		D3D11_PRIMITIVE_TOPOLOGY m_PrimitiveTopology;
		unsigned int m_NumVertexBuffers;
		unsigned int m_VertexStartSlot;
		ID3D11Buffer **m_pVertexBuffer;
		unsigned int m_VertexStride;
		unsigned int m_VertexOffset;
		ID3D11Buffer *m_pIndexBuffer;
		DXGI_FORMAT m_IndexFormat;
		unsigned int m_IBOffset;
	};

	struct VertexShaderData
	{
		VertexShaderData() { ZeroMemory(this, sizeof(VertexShaderData)); }
		~VertexShaderData() {}

		ID3D11InputLayout *m_pInputLayout;
		ID3D11VertexShader *m_pVertexShader;
		ID3D11Buffer **m_pVSBuffers;
		unsigned int m_VSNumBuffers;
		ID3D11ShaderResourceView **m_pVSSRVs;
		unsigned int m_VSNumSRVs;
		ID3D11SamplerState **m_pVSSamplers;
		unsigned int m_VSNumSamplers;
	};

	struct HullShaderData
	{
		HullShaderData() { ZeroMemory(this, sizeof(HullShaderData)); }
		~HullShaderData() {}

		ID3D11HullShader *m_pHullShader;
		ID3D11Buffer **m_pHSBuffers;
		unsigned int m_HSNumBuffers;
		ID3D11ShaderResourceView **m_pHSSRVs;
		unsigned int m_HSNumSRVs;
		ID3D11SamplerState **m_pHSSamplers;
		unsigned int m_HSNumSamplers;
	};

	struct DomainShaderData
	{
		DomainShaderData() { ZeroMemory(this, sizeof(DomainShaderData)); }
		~DomainShaderData() {}

		ID3D11DomainShader *m_pDomainShader;
		ID3D11Buffer **m_pDSBuffers;
		unsigned int m_DSNumBuffers;
		ID3D11ShaderResourceView **m_pDSSRVs;
		unsigned int m_DSNumSRVs;
		ID3D11SamplerState **m_pDSSamplers;
		unsigned int m_DSNumSamplers;
	};

	struct GeometryShaderData
	{
		GeometryShaderData() { ZeroMemory(this, sizeof(GeometryShaderData)); }
		~GeometryShaderData() {}

		ID3D11GeometryShader *m_pGeometryShader;
		ID3D11Buffer **m_pGSBuffers;
		unsigned int m_GSNumBuffers;
		ID3D11ShaderResourceView **m_pGSSRVs;
		unsigned int m_GSNumSRVs;
		ID3D11SamplerState **m_pGSSamplers;
		unsigned int m_GSNumSamplers;
	};

	struct PixelShaderData
	{
		PixelShaderData() { ZeroMemory(this, sizeof(PixelShaderData)); }
		~PixelShaderData() {}

		ID3D11PixelShader *m_pPixelShader;
		ID3D11Buffer **m_pPSBuffers;
		unsigned int m_PSNumBuffers;
		ID3D11ShaderResourceView **m_pPSSRVs;
		unsigned int m_PSNumSRVs;
		ID3D11SamplerState **m_pPSSamplers;
		unsigned int m_PSNumSamplers;
	};

	struct ComputeShaderData
	{
		ComputeShaderData() { ZeroMemory(this, sizeof(ComputeShaderData)); }
		~ComputeShaderData() {}

		ID3D11ComputeShader *m_pComputeShader;
		ID3D11Buffer **m_pCSBuffers;
		unsigned int m_CSNumBuffers;
		ID3D11ShaderResourceView **m_pCSSRVs;
		unsigned int m_CSNumSRVs;
		ID3D11SamplerState **m_pCSSamplers;
		unsigned int m_CSNumSamplers;
	};

	struct DrawCall
	{
		DrawCall() { ZeroMemory(this, sizeof(DrawCall)); }
		~DrawCall() {}

		MeshData *m_pMeshData;
		VertexShaderData *m_pVertexShaderData;
		HullShaderData *m_pHullShaderData;
		DomainShaderData *m_pDomainShaderData;
		GeometryShaderData *m_pGeometryShaderData;
		PixelShaderData *m_pPixelShaderData;
		ComputeShaderData *m_pComputeShaderData;
		RasterizerState *m_pRasterizerState;
		BlendState *m_pBlendState;
		DepthStencilState *m_pDepthStencilState;
	};
}
#endif