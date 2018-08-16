#include "SpriteFontGenerator.h"

SpriteFontGenerator::SpriteFontGenerator()
	: m_Width(0U), m_Height(0U), m_pDIB(nullptr), m_pGlyphs(nullptr)
{
}


SpriteFontGenerator::~SpriteFontGenerator()
{
	Destroy();
}


BVResult SpriteFontGenerator::Create(const char * const pFontName, const unsigned int size, const unsigned int firstChar, const unsigned int lastChar, const unsigned int numColumns, const bool bold, const bool italic, const bool aliased)
{
	FontSettings settings(pFontName, size, firstChar, lastChar, numColumns, bold, italic, aliased);
	return Create(settings);
}


BVResult SpriteFontGenerator::Create(const FontSettings & settings)
{
	Destroy();

	unsigned int firstChar = settings.m_FirstChar;
	unsigned int lastChar = settings.m_LastChar;
	unsigned int totalChars = lastChar - firstChar + 1;
	const int numColumns = settings.m_NumColumns;
	const int numRows = totalChars % numColumns == 0 ? (totalChars / numColumns) : (totalChars / numColumns) + 1;

	m_pGlyphs = new RectF[totalChars];

	HDC hDC = CreateCompatibleDC(nullptr);

	LOGFONT fontInfo;
	ZeroMemory(&fontInfo, sizeof(LOGFONT));
	fontInfo.lfHeight = settings.m_Size;
	fontInfo.lfWeight = settings.m_Bold ? FW_BOLD : FW_NORMAL;
	fontInfo.lfItalic = settings.m_Italic;
	fontInfo.lfQuality = settings.m_Aliased ? ANTIALIASED_QUALITY : NONANTIALIASED_QUALITY;
	strcpy_s(fontInfo.lfFaceName, settings.m_FontName);
	HFONT hFont = CreateFontIndirect(&fontInfo);
	if (!hFont)
	{
		return BVResult::kSpriteFont_Fail;
	}
	HFONT hOldFont = (HFONT)SelectObject(hDC, hFont);

	TEXTMETRIC metrics;
	GetTextMetrics(hDC, &metrics);

	BITMAPINFO bmInfo;
	memset(&bmInfo.bmiHeader, 0, sizeof(BITMAPINFOHEADER));
	bmInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmInfo.bmiHeader.biWidth = metrics.tmMaxCharWidth * numColumns;
	bmInfo.bmiHeader.biHeight = metrics.tmHeight * numRows;
	bmInfo.bmiHeader.biPlanes = 1;
	bmInfo.bmiHeader.biBitCount = 32;
	bmInfo.bmiHeader.biSizeImage = ((bmInfo.bmiHeader.biWidth * bmInfo.bmiHeader.biBitCount + 31) / 32) * 4 * bmInfo.bmiHeader.biHeight;
	bmInfo.bmiHeader.biCompression = BI_RGB;

	// Bitmaps' origin are usually bottom-left, so negate it to correct that
	bmInfo.bmiHeader.biHeight = -metrics.tmHeight * numRows;

	//create a new bitmap and select it in the memory dc
	BYTE *pbase;
	HBITMAP hBmp = CreateDIBSection(hDC,
		&bmInfo, DIB_RGB_COLORS, (void**)&pbase, nullptr, 0);
	HBITMAP hOldBmp = (HBITMAP)SelectObject(hDC, hBmp);

	ABC *pCharABC = new ABC[lastChar - firstChar + 1];
	GetCharABCWidthsW(hDC, firstChar, lastChar, pCharABC);

	//draw the text
	SetBkMode(hDC, TRANSPARENT);
	SetTextColor(hDC, RGB(255, 255, 255));

	wchar_t c;
	int left, top, bottom;
	bool done = false;
	for (int row = 0; row < numRows; row++)
	{
		top = row * metrics.tmHeight;
		bottom = top + metrics.tmHeight;

		for (int col = 0; col < numColumns; col++)
		{
			c = (wchar_t)(row * numColumns + col + firstChar);
			if (c >(wchar_t)lastChar)
			{
				done = true;
				break;
			}

			left = col * metrics.tmMaxCharWidth;

			// draw char
			TextOutW(hDC, left, top, &c, 1);

			ABC charABC = pCharABC[c - firstChar];
			m_pGlyphs[c - firstChar].x = (float)left;
			m_pGlyphs[c - firstChar].y = (float)top;
			m_pGlyphs[c - firstChar].width = (float)left + (float)(charABC.abcA + (int)charABC.abcB + charABC.abcC);
			m_pGlyphs[c - firstChar].height = (float)top + (float)metrics.tmHeight;
		}

		if (done)
		{
			break;
		}
	}
	delete[] pCharABC;

	SelectObject(hDC, hOldBmp);
	SelectObject(hDC, hOldFont);

	m_pDIB = new unsigned char[bmInfo.bmiHeader.biSizeImage];
	GetDIBits(hDC, hBmp, 0, bmInfo.bmiHeader.biHeight, m_pDIB, (BITMAPINFO*)&bmInfo.bmiHeader, DIB_RGB_COLORS);

	for (UINT i = 0; i < bmInfo.bmiHeader.biSizeImage; i += 4)
	{
		if (m_pDIB[i] == 0 && m_pDIB[i + 1] == 0 && m_pDIB[i + 2] == 0)
		{
			m_pDIB[i + 3] = 0;
		}
		else
		{
			m_pDIB[i + 3] = 255;
		}
	}

	DeleteObject(hBmp);
	DeleteObject(hFont);
	DeleteDC(hDC);

	// Update bitmap's height again, cause it was negated before to correct the origin
	bmInfo.bmiHeader.biHeight *= -1;

	m_Width = bmInfo.bmiHeader.biWidth;
	m_Height = bmInfo.bmiHeader.biHeight;

	return BVResult::kSuccess;
}


void SpriteFontGenerator::Destroy()
{
	if (m_pDIB) { delete m_pDIB; m_pDIB = nullptr; }
	if (m_pGlyphs) { delete m_pGlyphs; m_pGlyphs = nullptr; }
}