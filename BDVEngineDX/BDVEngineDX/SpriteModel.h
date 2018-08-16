#ifndef SPRITEMODEL_H
#define SPRITEMODEL_H

#include "Model.h"

class SpriteModel : public Model
{
public:
	SpriteModel(const UINT spriteBatchSize);
	virtual ~SpriteModel();

	bool Initialize(const Graphics * const pGraphics);

	void MapSprites(const Graphics * const pGraphics, const Vertex2D * const pVertices, const UINT numSprites);

	void Draw(const Graphics * const pGraphics, const UINT numSprites);

private:
	void Draw(const Graphics * const pGraphics) override;

	// Not needed
	SpriteModel() = delete;
	SpriteModel(const SpriteModel &) = delete;
	SpriteModel & operator = (const SpriteModel &) = delete;

private:
	int m_SpriteBatchSize;

	ComPtr<ID3D11DepthStencilState> m_DepthStencilState;
	ComPtr<ID3D11BlendState> m_pBlendState;
};
#endif