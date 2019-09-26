#ifndef SPRITEBATCH_H
#define SPRITEBATCH_H

#include "Allocator.h"
#include "BVMath.h"

#include "VertexFormat.h"
#include "RendererD3D11.h"

using namespace BVMemory;
using namespace BVMath;
using namespace BVGraphics;

namespace BVEngine
{
#define MAXIMUM_SPRITE_COUNT 10

	struct RectF
	{
		float x;
		float y;
		float width;
		float height;

		RectF() : x(0.0f), y(0.0f), width(0.0f), height(0.0f) {}
		RectF(const float x, const float y, const float width, const float height)
			: x(x), y(y), width(width), height(height) {}
	};

	struct Sprite
	{
		BVFloat2 position;
		BVFloat2 size;
		BVFloat4 color;
		RectF textureRect;
		BVFloat2 scale;
		float angle;
		bool fullQuad;

		Sprite() : position(0.0f, 0.0f), size(0.0f, 0.0f), scale(1.0f, 1.0f), color(1.0f, 1.0f, 1.0f, 1.0f),
			textureRect(0.0f, 0.0f, 1.0f, 1.0f), angle(0.0f), fullQuad(false) {}
		Sprite(const BVFloat2 &_position, const BVFloat2 & _size, const RectF &_textureRect, const BVFloat4 &_color = BVFloat4(1.0f, 1.0f, 1.0f, 1.0f),
			const BVFloat2 &_scale = BVFloat2(1.0f, 1.0f), const float _angle = 0.0f, const bool _fullQuad = false)
			: position(_position), size(_size), scale(_scale), color(_color), textureRect(_textureRect), angle(_angle), fullQuad(_fullQuad) {}
	};

	class SpriteBatch
	{
	public:
		SpriteBatch(RendererD3D11 * const pGraphics, Allocator * const pAllocator = nullptr);
		~SpriteBatch();

		bool Create(const float screenWidth, const float screenHeight, const unsigned int spriteCount = MAXIMUM_SPRITE_COUNT);
		void Resize(const float width, const float height);

		void Draw(const Sprite & sprite, ShaderResourceView * const pTexture = nullptr);

	private:
		void Clear();

		SpriteBatch(const SpriteBatch & other) = delete;
		SpriteBatch(SpriteBatch && other) = delete;
		SpriteBatch & operator = (const SpriteBatch & other) = delete;
		SpriteBatch & operator = (SpriteBatch && other) = delete;

	private:
		RendererD3D11 *m_pGraphics;

		// Screen dimensions
		float m_Width;
		float m_Height;
		struct CBVertexData
		{
			BVFloat44 m_Ortho;
			BVFloat2 m_TextureInfo;
		} m_CBVertexData;

		struct CBPixelData
		{
			BVFloat3 m_Color;
			int m_UseTexture;
		} m_CBPixelData;
		
		// Sprite count
		unsigned int m_SpriteCount;
		// Sprites
		struct SpriteData
		{
			Vertex2D m_pVertices[4];
		} *m_pSprites;

		// Gpu Buffers
		VertexBuffer *m_pVB;
		IndexBuffer *m_pIB;
		ConstantBuffer *m_pCBVertexData;
		ConstantBuffer *m_pCBPixelData;

		// Shaders
		InputLayout *m_pIA;
		VertexShader *m_pVS;
		PixelShader *m_pPS;

		// States
		SamplerState *m_pSamplerState;
		BlendState *m_pBlendState;
		DepthStencilState *m_pDepthStencilState;

		Allocator *m_pAllocator;
	};
}
#endif