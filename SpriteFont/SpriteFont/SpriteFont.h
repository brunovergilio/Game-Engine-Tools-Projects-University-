#ifndef SPRITEFONT_H
#define SPRITEFONT_H

#include "SpriteFontGenerator.h"

class SpriteFont
{
public:
	SpriteFont();
	~SpriteFont();

	int Generate(const int argc, char * const argv[]);

private:
	int ProcessFontName(const int numArgs, char * const pArgs[], FontSettings & settings);
	bool WriteToFile(const char * const pFilename, const SpriteFontGenerator & gen, const FontSettings & settings);
	bool ParseArgs(const int numArgs, char * const pArgs[], FontSettings & settings);
	void ShowFormat();

	SpriteFont(const SpriteFont &) = delete;
	SpriteFont(SpriteFont &&) = delete;
	SpriteFont & operator =(const SpriteFont &) = delete;
	SpriteFont & operator =(SpriteFont &&) = delete;
};

#endif