#ifndef SPRITEFONT_H
#define SPRITEFONT_H

#include "SpriteBatch.h"

class SpriteFont
{
public:
	SpriteFont();
	SpriteFont(SpriteBatch * const pSpriteBatch);
	~SpriteFont();

	bool Initialize(const Graphics * const pGraphics, const char * const pFontName, const int fontSize, const bool bold = false, const bool italic = false, const bool aliased = true);
	bool Initialize(const Graphics * const pGraphics, const char * const pFilename, const char * const pInfoFilename);
	void Shutdown();

	void RenderText(const Camera * const pCamera, const char * const pText, const float x, const float y, const XMFLOAT4 &color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

	Texture *GetTexture() { return m_pTexture; }

	void SetSpriteBatch(SpriteBatch * const pSpriteBatch);

private:
	// Not needed
	SpriteFont(const SpriteFont &) = delete;
	SpriteFont & operator = (const SpriteFont &) = delete;

private:
	int m_NumRows;
	int m_NumColumns;
	RectF *m_pGlyphs;

	Texture *m_pTexture;

	unsigned char m_InitialChar;
	unsigned char m_FinalChar;

	SpriteBatch *m_pSpriteBatch;
};

#endif