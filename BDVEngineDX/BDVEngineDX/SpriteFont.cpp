#include "SpriteFont.h"

SpriteFont::SpriteFont()
	: m_NumRows(14), m_NumColumns(16), m_InitialChar(32), m_FinalChar(255), m_pSpriteBatch(nullptr)
{
	m_pGlyphs = new RectF[m_NumRows * m_NumColumns];
	m_pTexture = new Texture();
}

SpriteFont::SpriteFont(SpriteBatch * const pSpriteBatch)
	: m_NumRows(14), m_NumColumns(16), m_InitialChar(32), m_FinalChar(255), m_pSpriteBatch(pSpriteBatch)
{
	m_pGlyphs = new RectF[m_NumRows * m_NumColumns];
	m_pTexture = new Texture();
}

SpriteFont::~SpriteFont()
{
	delete[] m_pGlyphs;
	delete m_pTexture;

	m_pSpriteBatch = nullptr;
}

bool SpriteFont::Initialize(const Graphics * const pGraphics, const char * const pFontName, const int fontSize, const bool bold, const bool italic, const bool aliased)
{
	HDC hDC = CreateCompatibleDC(nullptr);
	
	LOGFONT fontInfo;
	ZeroMemory(&fontInfo, sizeof(LOGFONT));
	fontInfo.lfHeight = fontSize;
	fontInfo.lfWeight = bold ? FW_BOLD : FW_NORMAL;
	fontInfo.lfItalic = italic;
	fontInfo.lfQuality = aliased ? ANTIALIASED_QUALITY : NONANTIALIASED_QUALITY;
	strcpy_s(fontInfo.lfFaceName, pFontName);
	HFONT hFont = CreateFontIndirect(&fontInfo);
	if (!hFont)
	{
		return false;
	}
	HFONT hOldFont = (HFONT)SelectObject(hDC, hFont);
	
	TEXTMETRIC metrics;
	GetTextMetrics(hDC, &metrics);
	
	BITMAPINFO bmInfo;
	memset(&bmInfo.bmiHeader, 0, sizeof(BITMAPINFOHEADER));
	bmInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmInfo.bmiHeader.biWidth = metrics.tmMaxCharWidth * m_NumColumns;
	bmInfo.bmiHeader.biHeight = metrics.tmHeight * m_NumRows;
	bmInfo.bmiHeader.biPlanes = 1;
	bmInfo.bmiHeader.biBitCount = 32;
	bmInfo.bmiHeader.biSizeImage = ((bmInfo.bmiHeader.biWidth * bmInfo.bmiHeader.biBitCount + 31) / 32) * 4 * bmInfo.bmiHeader.biHeight;
	bmInfo.bmiHeader.biCompression = BI_RGB;

	// Bitmaps' origin are usually bottom-left, so negate it to correct that
	bmInfo.bmiHeader.biHeight = -metrics.tmHeight * m_NumRows;
	
	//create a new bitmap and select it in the memory dc
	BYTE *pbase;
	HBITMAP hBmp = CreateDIBSection(hDC,
		&bmInfo, DIB_RGB_COLORS, (void**)&pbase, nullptr, 0);
	HBITMAP hOldBmp = (HBITMAP)SelectObject(hDC, hBmp);
	
	ABC *pCharABC = new ABC[m_FinalChar - m_InitialChar + 1];
	GetCharABCWidths(hDC, m_InitialChar, m_FinalChar, pCharABC);
	
	//draw the text
	SetBkMode(hDC, TRANSPARENT);
	SetTextColor(hDC, RGB(255, 255, 255));
	
	unsigned char c;
	int left, top, bottom;
	
	for (int row = 0; row < m_NumRows; row++)
	{
		top = row * metrics.tmHeight;
		bottom = top + metrics.tmHeight;
	
		for (int col = 0; col < m_NumColumns; col++)
		{
			left = col * metrics.tmMaxCharWidth;
			c = (unsigned char)(row * m_NumColumns + col + m_InitialChar);
	
			// draw char:
			TextOut(hDC, left, top, (const char*)&c, 1);

			ABC charABC = pCharABC[c - m_InitialChar];
			m_pGlyphs[c - m_InitialChar].x = (float)left;
			m_pGlyphs[c - m_InitialChar].y = (float)top;
			m_pGlyphs[c - m_InitialChar].width = (float)left + (float)(charABC.abcA + (int)charABC.abcB + charABC.abcC);
			m_pGlyphs[c - m_InitialChar].height = (float)top + (float)metrics.tmHeight;
		}
	}
	delete[] pCharABC;
	
	SelectObject(hDC, hOldBmp);
	SelectObject(hDC, hOldFont);
	
	unsigned char *pDib = new unsigned char[bmInfo.bmiHeader.biSizeImage];
	GetDIBits(hDC, hBmp, 0, bmInfo.bmiHeader.biHeight, pDib, (BITMAPINFO*)&bmInfo.bmiHeader, DIB_RGB_COLORS);
	
	for (UINT i = 0; i < bmInfo.bmiHeader.biSizeImage; i += 4)
	{
		if (pDib[i] == 0 && pDib[i + 1] == 0 && pDib[i + 2] == 0)
		{
			pDib[i + 3] = 0;
		}
		else
		{
			pDib[i + 3] = 255;
		}
	}
	
	DeleteObject(hBmp);
	DeleteObject(hFont);
	DeleteDC(hDC);

	// Update bitmap's height again, cause it was negated before to correct the origin
	bmInfo.bmiHeader.biHeight *= -1;

	if (!m_pTexture->Initialize(pGraphics, pDib, bmInfo.bmiHeader.biWidth, bmInfo.bmiHeader.biHeight, 4))
	{
		delete[] pDib;
		return false;
	}

	delete[] pDib;
	
	return true;
}

bool SpriteFont::Initialize(const Graphics * const pGraphics, const char * const pFilename, const char * const pInfoFilename)
{
	char pInfoFilenameFull[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, pInfoFilenameFull);
	strcat_s(pInfoFilenameFull, MAX_PATH, "\\Data\\Fonts\\");
	strcat_s(pInfoFilenameFull, MAX_PATH, pInfoFilename);

	if (!m_pTexture->Initialize(pGraphics, pFilename))
	{
		return false;
	}

#pragma warning (disable : 4996)
	FILE *pFile = fopen(pInfoFilenameFull, "rb");
#pragma warning (default : 4996)
	if (pFile)
	{
		unsigned int charData;
		fread(&charData, sizeof(unsigned int), 1, pFile);
		m_InitialChar = (unsigned char)charData;

		fread(&charData, sizeof(unsigned int), 1, pFile);
		m_FinalChar = (unsigned char)charData;

		m_pGlyphs = new RectF[m_FinalChar - m_InitialChar];
		fread(m_pGlyphs, sizeof(RectF) * (m_FinalChar - m_InitialChar), 1, pFile);

		fclose(pFile);
	}
	else
	{
		return false;
	}

	return true;
}

void SpriteFont::Shutdown()
{
}

void SpriteFont::RenderText(const Camera * const pCamera, const char * const pText, const float x, const float y, const XMFLOAT4 &color)
{
	Sprite glyph;
	glyph.color = color;

	m_pSpriteBatch->BeginDrawBatch(m_pTexture);

	float posX = x;
	unsigned char c;
	for (unsigned int i = 0; i < strlen(pText); i++)
	{
		c = (unsigned char)pText[i];
		if ((unsigned char)pText[i] < m_InitialChar)
		{
			c = m_InitialChar;
		}
		if ((unsigned char)pText[i] > m_FinalChar)
		{
			c = m_FinalChar;
		}

		glyph.position.x = posX;
		glyph.position.y = y;
		glyph.textureRect = m_pGlyphs[c - m_InitialChar];

		m_pSpriteBatch->DrawBatch(glyph);

		posX += glyph.textureRect.width - glyph.textureRect.x;
	}

	m_pSpriteBatch->EndBatchDraw(pCamera);
}

void SpriteFont::SetSpriteBatch(SpriteBatch * const pSpriteBatch)
{
	m_pSpriteBatch = pSpriteBatch;
}