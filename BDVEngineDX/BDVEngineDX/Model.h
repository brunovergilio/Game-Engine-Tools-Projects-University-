#ifndef MODEL_H
#define MODEL_H

#include "Graphics.h"
#include "Vertex.h"

class Model
{
public:
	Model();
	virtual ~Model();

	bool Initialize(const Graphics * const pGraphics, const void * const pVertices, const UINT vertexCount, const UINT vertexSize,
		const void * const pIndices, const UINT indexCount, const UINT indexSize = 4, 
		const D3D11_USAGE vertexUsage = D3D11_USAGE_DEFAULT, const D3D11_USAGE indexUsage = D3D11_USAGE_DEFAULT,
		const UINT vertexAccess = 0, const UINT indexAccess = 0);

	virtual void Draw(const Graphics * const pGraphics) = 0;

	void Shutdown();

	const UINT GetNumVertices() const;
	const UINT GetNumIndices() const;

private:
	// Not needed
	Model(const Model &) = delete;
	Model & operator = (const Model &) = delete;

protected:
	ComPtr<ID3D11Buffer> m_pVertexBuffer;
	ComPtr<ID3D11Buffer> m_pIndexBuffer;

	UINT m_NumVertices;
	UINT m_NumIndices;
	UINT m_VertexSize;
};

#endif