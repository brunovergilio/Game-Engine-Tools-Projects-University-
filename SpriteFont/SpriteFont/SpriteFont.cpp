#include "SpriteFont.h"
#include <cstdio>
#include <cstdlib>
#include <FreeImage.h>

SpriteFont::SpriteFont()
{
}

SpriteFont::~SpriteFont()
{
}

int SpriteFont::Generate(const int argc, char * const argv[])
{
	int ret = 0;

	FontSettings settings;
	if (1 == argc)
	{
		ret = 1;
	}
	else if (ParseArgs(argc, argv, settings))
	{
		printf("Generating file %s.png\n\n", argv[1]);

		SpriteFontGenerator gen;
		if (BVResult::kSuccess == gen.Create(settings))
		{
			WriteToFile(argv[1], gen, settings);
			printf("Done.\n");
		}
		else
		{
			ret = 1;
		}
	}
	else
	{
		ret = 1;
	}

	if (ret)
	{
		ShowFormat();
	}

	return ret;
}


bool SpriteFont::WriteToFile(const char * const pFilename, const SpriteFontGenerator & gen, const FontSettings & settings)
{
	// Write Font Atlas
	char pImageFilename[MAX_PATH];
	strcpy_s(pImageFilename, pFilename);
	strcat_s(pImageFilename, ".png");

	FIBITMAP *pDestImage = FreeImage_ConvertFromRawBits((BYTE *)gen.GetDIB(), gen.GetWidth(), gen.GetHeight(),
		gen.GetWidth() * 4, 32, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK, TRUE);
	FreeImage_Save(FREE_IMAGE_FORMAT::FIF_PNG, pDestImage, pImageFilename);
	FreeImage_Unload(pDestImage);

	// Write Font Data
	unsigned int totalChars = settings.m_LastChar - settings.m_FirstChar + 1;

	char pInfoFilename[MAX_PATH];
	strcpy_s(pInfoFilename, pFilename);
	strcat_s(pInfoFilename, ".bvfont");

	FILE *pFile = nullptr;
	fopen_s(&pFile, pInfoFilename, "wb");
	if (pFile)
	{
		fwrite(&settings, sizeof(FontSettings), 1, pFile);
		fwrite(gen.GetGlyphs(), sizeof(RectF) * totalChars, 1, pFile);

		fclose(pFile);
	}
	else
	{
		return false;
	}

	return true;
}

bool SpriteFont::ParseArgs(const int numArgs, char * const pArgs[], FontSettings & settings)
{
	printf("Parsing arguments...\n");

	bool ok = true;
	bool skipNext = false;

	if (numArgs > 2)
	{
		int indexAfterString = ProcessFontName(numArgs, pArgs, settings);

		for (int i = indexAfterString; i < numArgs; i++)
		{
			if (strstr(pArgs[i], "-c") || strstr(pArgs[i], "-C"))
			{
				if (i + 1 >= numArgs)
				{
					printf("Invalid arguments.\n");

					ok = false;
					break;
				}
				else
				{
					settings.m_NumColumns = strtol(pArgs[i + 1], nullptr, 10);
					if (settings.m_NumColumns > 0)
					{
						skipNext = true;
					}
					else
					{
						printf("Invalid arguments.\n");

						ok = false;
						break;
					}
				}
			}
			else if (strstr(pArgs[i], "-s") || strstr(pArgs[i], "-S"))
			{
				if (i + 1 >= numArgs)
				{
					printf("Invalid arguments.\n");

					ok = false;
					break;
				}
				else
				{
					settings.m_Size = strtol(pArgs[i + 1], nullptr, 10);
					if (settings.m_Size > 0)
					{
						skipNext = true;
					}
					else
					{
						printf("Invalid arguments.\n");

						ok = false;
						break;
					}
				}
			}
			else if (strstr(pArgs[i], "-b") || strstr(pArgs[i], "-B"))
			{
				settings.m_Bold = true;
			}
			else if (strstr(pArgs[i], "-i") || strstr(pArgs[i], "-I"))
			{
				settings.m_Italic = true;
			}
			else if (strstr(pArgs[i], "-a") || strstr(pArgs[i], "-A"))
			{
				settings.m_Aliased = true;
			}
			else if (strstr(pArgs[i], "-r") || strstr(pArgs[i], "-R"))
			{
				if (i + 1 >= numArgs)
				{
					printf("Invalid arguments.\n");

					ok = false;
					break;
				}
				else
				{
					bool rangeOK = true;
					char *pMinLimit, *pMaxLimit;
					pMinLimit = strtok_s(pArgs[i + 1], "-", &pMaxLimit);
					if (pMinLimit && pMaxLimit)
					{
						settings.m_FirstChar = (unsigned int)strtoul(pMinLimit, nullptr, 10);
						settings.m_LastChar = (unsigned int)strtoul(pMaxLimit, nullptr, 10);

						if (settings.m_FirstChar >= settings.m_LastChar)
						{
							rangeOK = false;
						}
					}
					else
					{
						rangeOK = false;
					}

					if (rangeOK)
					{
						skipNext = true;
					}
					else
					{
						printf("Invalid arguments.\n");

						ok = false;
						break;
					}
				}
			}
			else
			{
				if (!skipNext)
				{
					printf("Invalid arguments.\n");

					ok = false;
					break;
				}
				skipNext = false;
			}
		}
	}
	else
	{
		printf("Invalid number of arguments.\n");

		ok = false;
	}

	if (ok)
	{
		printf("Parsing OK.\n");
	}
	else
	{
		printf("Error generating font file.\n");
	}

	printf("\n\n");

	return ok;
}

int SpriteFont::ProcessFontName(const int numArgs, char * const pArgs[], FontSettings & settings)
{
	int i = 2;
	for (; i < numArgs; i++)
	{
		if (strstr(pArgs[i], "-c") || strstr(pArgs[i], "-C")
			|| strstr(pArgs[i], "-s") || strstr(pArgs[i], "-S")
			|| strstr(pArgs[i], "-b") || strstr(pArgs[i], "-B")
			|| strstr(pArgs[i], "-i") || strstr(pArgs[i], "-I")
			|| strstr(pArgs[i], "-a") || strstr(pArgs[i], "-A")
			|| strstr(pArgs[i], "-r") || strstr(pArgs[i], "-R"))
		{
			break;
		}
		else
		{
			if (0 != settings.m_FontName[0])
			{
				strcat_s(settings.m_FontName, " ");
			}
			strcat_s(settings.m_FontName, pArgs[i]);
		}
	}

	return i;
}

void SpriteFont::ShowFormat()
{
	printf("Usage:\n\n");
	printf("SpriteFont.exe <OutFile> <FontName> <Options>\n");
	printf("<OutFile> : Output File\n");
	printf("<FontName>: Font Name\n");
	printf("<Options>:\n");
	printf("* -c [N]      : Specifies the number of columns per row (Default: 32)\n");
	printf("* -s [N]      : Specifies the font size (Default: 12)\n");
	printf("* -b          : Bold (Default: False)\n");
	printf("* -i          : Italic (Default: False)\n");
	printf("* -a          : Antialiased (Default: False)\n");
	printf("* -r [N1]-[N2]: Range of unicode characters to include\n");
	printf("*               (Min: 0 | Max: 65535 | Default: 0-255)\n");
	printf("\n");
}