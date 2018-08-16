#ifndef SPRITEBATCH_H
#define SPRITEBATCH_H

#include "Camera.h"
#include "SpriteModel.h"
#include "SpriteShader.h"

const int DEFAULT_BATCH_SIZE = 200;

struct RectF
{
	float x;
	float y;
	float width;
	float height;

	RectF() : x(0.0f), y(0.0f), width(0.0f), height(0.0f) {}
	RectF(const float _x, const float _y, const float _width, const float _height)
		: x(_x), y(_y), width(_width), height(_height) {}
};

struct Sprite
{
	XMFLOAT2 position;
	XMFLOAT2 scale;
	XMFLOAT4 color;
	RectF textureRect;
	float angle;

	Sprite() : position(0.0f, 0.0f), scale(1.0f, 1.0f), color(1.0f, 1.0f, 1.0f, 1.0f), textureRect(0.0f, 0.0f, 1.0f, 1.0f), angle(0.0f) {}
	Sprite(const XMFLOAT2 &_position, const RectF &_textureRect, const XMFLOAT4 &_color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 
		const XMFLOAT2 &_scale = XMFLOAT2(1.0f, 1.0f), const float _angle = 0.0f)
		: position(_position), scale(_scale), color(_color), textureRect(_textureRect), angle(_angle) {}
};

class SpriteBatch
{
public:
	SpriteBatch(const int size = DEFAULT_BATCH_SIZE);
	SpriteBatch(Graphics * const pGraphics, const int size = DEFAULT_BATCH_SIZE);
	~SpriteBatch();

	bool Initialize();
	void Shutdown();

	void SetGraphics(Graphics * const pGraphics);
	Graphics *GetGraphics();

	void SetSampler(Sampler * const pSampler);

	void DrawSingle(const Camera * const pCamera, const Sprite &sprite, Texture * const pTexture, Sampler * const pSampler = nullptr);
	void DrawSingleOnCenter(const Camera * const pCamera, const Sprite &sprite, Texture * const pTexture, Sampler * const pSampler = nullptr);

	void BeginDrawBatch(Texture * const pTexture, Sampler * const pSampler = nullptr);
	void DrawBatch(const Sprite &sprite);
	void DrawBatchOnCenter(const Sprite &sprite);
	void EndBatchDraw(const Camera * const pCamera);

private:
	// Not needed
	SpriteBatch(const SpriteBatch &) = delete;
	SpriteBatch & operator = (const SpriteBatch &) = delete;

private:
	int m_BatchSize;
	int m_CurrBatchSize;

	bool m_DrawBatch;

	Graphics *m_pGraphics;

	Vertex2D *m_pVertices;

	SpriteModel *m_pModel;
	SpriteShader *m_pShader;

	Sampler *m_pSampler;

	SpriteShaderParameters *m_pShaderParameters;
};

#endif