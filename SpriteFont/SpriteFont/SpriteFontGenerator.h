#ifndef SPRITEFONTGENERATOR_H
#define SPRITEFONTGENERATOR_H

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#define SPRITEFONT_MAX_FONT_SIZE 32

enum class BVResult : unsigned int
{
	kSuccess,
	kSpriteFont_Fail
};

struct FontSettings
{
	char m_FontName[SPRITEFONT_MAX_FONT_SIZE];
	int m_Size;
	unsigned int m_FirstChar;
	unsigned int m_LastChar;
	unsigned int m_NumColumns;
	bool m_Bold;
	bool m_Italic;
	bool m_Aliased;

	FontSettings()
		: m_Size(12), m_FirstChar(0U), m_LastChar(255U), m_NumColumns(32), m_Bold(false), m_Italic(false), m_Aliased(false)
	{
		memset(m_FontName, 0, SPRITEFONT_MAX_FONT_SIZE);
	}

	FontSettings(const char * const pFontName, const unsigned int size, const unsigned int firstChar, const unsigned int lastChar,
		const unsigned int numColumns, const bool bold, const bool italic, const bool aliased)
		: m_Size(size), m_FirstChar(firstChar), m_LastChar(lastChar), m_NumColumns(numColumns), m_Bold(bold), m_Italic(italic), m_Aliased(aliased)
	{
		strcpy_s(m_FontName, pFontName);
	}
};

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

class SpriteFontGenerator
{
public:
	SpriteFontGenerator();
	~SpriteFontGenerator();

	BVResult Create(const char * const pFontName, const unsigned int size, const unsigned int firstChar, const unsigned int lastChar,
		const unsigned int numColumns, const bool bold, const bool italic, const bool aliased);
	BVResult Create(const FontSettings & settings);

	void Destroy();

	inline const unsigned char * const GetDIB() const { return m_pDIB; }
	inline const RectF * const GetGlyphs() const { return m_pGlyphs; }
	inline const unsigned int GetWidth() const { return m_Width; }
	inline const unsigned int GetHeight() const { return m_Height; }

private:
	SpriteFontGenerator(const SpriteFontGenerator &) = delete;
	SpriteFontGenerator(SpriteFontGenerator &&) = delete;
	SpriteFontGenerator & operator =(const SpriteFontGenerator &) = delete;
	SpriteFontGenerator & operator =(SpriteFontGenerator &&) = delete;

private:
	unsigned int m_Width;
	unsigned int m_Height;
	unsigned char *m_pDIB;
	RectF *m_pGlyphs;
};

#endif