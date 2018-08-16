#include "SpriteBatch.h"

SpriteBatch::SpriteBatch(Graphics * const pGraphics, const int batchSize)
: m_BatchSize(batchSize), m_CurrBatchSize(0), m_DrawBatch(false), m_pGraphics(pGraphics)
{
	m_pVertices = new Vertex2D[4 * batchSize];
	m_pModel = new SpriteModel(batchSize);
	m_pShader = new SpriteShader();
	m_pShaderParameters = new SpriteShaderParameters();
}

SpriteBatch::SpriteBatch(const int batchSize)
	: m_BatchSize(batchSize), m_CurrBatchSize(0), m_DrawBatch(false), m_pGraphics(nullptr)
{
	m_pVertices = new Vertex2D[4 * batchSize];
	m_pModel = new SpriteModel(batchSize);
	m_pShader = new SpriteShader();
	m_pShaderParameters = new SpriteShaderParameters();
}

SpriteBatch::~SpriteBatch()
{
	delete[] m_pVertices;
	delete m_pModel;
	delete m_pShader;
	delete m_pShaderParameters;

	m_pGraphics = nullptr;
	m_pSampler = nullptr;
}

bool SpriteBatch::Initialize()
{
	if (!m_pGraphics)
	{
		return false;
	}

	if (!m_pModel->Initialize(m_pGraphics))
	{
		return false;
	}

	if (!m_pShader->Initialize(m_pGraphics, "spriteShader"))
	{
		return false;
	}

	return true;
}

void SpriteBatch::Shutdown()
{
}

void SpriteBatch::SetGraphics(Graphics * const pGraphics)
{
	m_pGraphics = pGraphics;
}

Graphics *SpriteBatch::GetGraphics()
{
	return m_pGraphics;
}

void SpriteBatch::SetSampler(Sampler * const pSampler)
{
	m_pSampler = pSampler;
}

void SpriteBatch::DrawSingle(const Camera * const pCamera, const Sprite &sprite, Texture * const pTexture, Sampler * const pSampler)
{
	if (m_DrawBatch || !m_pGraphics)
	{
		return;
	}

	float texWidth = pTexture->GetWidth();
	float texHeight = pTexture->GetHeight();

	m_pShaderParameters->pTexture = pTexture;
	m_pShaderParameters->pSampler = pSampler ? pSampler : m_pSampler;
	m_pShaderParameters->projMatrix = pCamera->GetOrthoMatrix();
	m_pShaderParameters->textureInfo.x = texWidth;
	m_pShaderParameters->textureInfo.y = texHeight;

	m_pShader->MapData(m_pGraphics, m_pShaderParameters);

	float spriteWidth = sprite.textureRect.width - sprite.textureRect.x;
	float spriteHeight = sprite.textureRect.height - sprite.textureRect.y;

	m_pVertices[0].m_Position = XMFLOAT3(sprite.position.x, sprite.position.y + spriteHeight, 0.0f);
	m_pVertices[1].m_Position = XMFLOAT3(sprite.position.x, sprite.position.y, 0.0f);
	m_pVertices[2].m_Position = XMFLOAT3(sprite.position.x + spriteWidth, sprite.position.y, 0.0f);
	m_pVertices[3].m_Position = XMFLOAT3(sprite.position.x + spriteWidth, sprite.position.y + spriteHeight, 0.0f);

	m_pVertices[0].m_Texture = XMFLOAT2(sprite.textureRect.x, sprite.textureRect.y + spriteHeight);
	m_pVertices[1].m_Texture = XMFLOAT2(sprite.textureRect.x, sprite.textureRect.y);
	m_pVertices[2].m_Texture = XMFLOAT2(sprite.textureRect.x + spriteWidth, sprite.textureRect.y);
	m_pVertices[3].m_Texture = XMFLOAT2(sprite.textureRect.x + spriteWidth, sprite.textureRect.y + spriteHeight);

	m_pVertices[0].m_Color = sprite.color;
	m_pVertices[1].m_Color = sprite.color;
	m_pVertices[2].m_Color = sprite.color;
	m_pVertices[3].m_Color = sprite.color;

	m_pModel->MapSprites(m_pGraphics, m_pVertices, 1);

	m_pModel->Draw(m_pGraphics, 1);
}

void SpriteBatch::DrawSingleOnCenter(const Camera * const pCamera, const Sprite &sprite, Texture * const pTexture, Sampler * const pSampler)
{
	if (m_DrawBatch || !m_pGraphics)
	{
		return;
	}

	float texWidth = pTexture->GetWidth();
	float texHeight = pTexture->GetHeight();

	m_pShaderParameters->pTexture = pTexture;
	m_pShaderParameters->pSampler = pSampler ? pSampler : m_pSampler;
	m_pShaderParameters->projMatrix = pCamera->GetOrthoMatrix();
	m_pShaderParameters->textureInfo.x = texWidth;
	m_pShaderParameters->textureInfo.y = texHeight;

	m_pShader->MapData(m_pGraphics, m_pShaderParameters);

	float spriteWidth = sprite.textureRect.width - sprite.textureRect.x;
	float spriteHeight = sprite.textureRect.height - sprite.textureRect.y;
	float spriteHalfWidth = spriteWidth * 0.5f;
	float spriteHalfHeight = spriteHeight * 0.5f;

	m_pVertices[0].m_Position = XMFLOAT3(sprite.position.x - spriteHalfWidth, sprite.position.y - spriteHalfHeight + spriteHeight, 0.0f);
	m_pVertices[1].m_Position = XMFLOAT3(sprite.position.x - spriteHalfWidth, sprite.position.y - spriteHalfHeight, 0.0f);
	m_pVertices[2].m_Position = XMFLOAT3(sprite.position.x - spriteHalfWidth + spriteWidth, sprite.position.y - spriteHalfHeight, 0.0f);
	m_pVertices[3].m_Position = XMFLOAT3(sprite.position.x - spriteHalfWidth + spriteWidth, sprite.position.y - spriteHalfHeight + spriteHeight, 0.0f);

	m_pVertices[0].m_Texture = XMFLOAT2(sprite.textureRect.x, sprite.textureRect.y + spriteHeight);
	m_pVertices[1].m_Texture = XMFLOAT2(sprite.textureRect.x, sprite.textureRect.y);
	m_pVertices[2].m_Texture = XMFLOAT2(sprite.textureRect.x + spriteWidth, sprite.textureRect.y);
	m_pVertices[3].m_Texture = XMFLOAT2(sprite.textureRect.x + spriteWidth, sprite.textureRect.y + spriteHeight);

	m_pVertices[0].m_Color = sprite.color;
	m_pVertices[1].m_Color = sprite.color;
	m_pVertices[2].m_Color = sprite.color;
	m_pVertices[3].m_Color = sprite.color;

	m_pModel->MapSprites(m_pGraphics, m_pVertices, 1);

	m_pModel->Draw(m_pGraphics, 1);
}

void SpriteBatch::BeginDrawBatch(Texture * const pTexture, Sampler * const pSampler)
{
	m_CurrBatchSize = 0;
	m_DrawBatch = true;

	m_pShaderParameters->pTexture = pTexture;
	m_pShaderParameters->pSampler = pSampler ? pSampler : m_pSampler;

	float texWidth = pTexture->GetWidth();
	float texHeight = pTexture->GetHeight();

	m_pShaderParameters->textureInfo.x = texWidth;
	m_pShaderParameters->textureInfo.y = texHeight;
}

void SpriteBatch::DrawBatch(const Sprite &sprite)
{
	float spriteWidth = sprite.textureRect.width - sprite.textureRect.x;
	float spriteHeight = sprite.textureRect.height - sprite.textureRect.y;

	int currentBatch = m_CurrBatchSize * 4;

	m_pVertices[currentBatch + 0].m_Position = XMFLOAT3(sprite.position.x, sprite.position.y + spriteHeight, 0.0f);
	m_pVertices[currentBatch + 1].m_Position = XMFLOAT3(sprite.position.x, sprite.position.y, 0.0f);
	m_pVertices[currentBatch + 2].m_Position = XMFLOAT3(sprite.position.x + spriteWidth, sprite.position.y, 0.0f);
	m_pVertices[currentBatch + 3].m_Position = XMFLOAT3(sprite.position.x + spriteWidth, sprite.position.y + spriteHeight, 0.0f);
	
	m_pVertices[currentBatch + 0].m_Texture = XMFLOAT2(sprite.textureRect.x, sprite.textureRect.y + spriteHeight);
	m_pVertices[currentBatch + 1].m_Texture = XMFLOAT2(sprite.textureRect.x, sprite.textureRect.y);
	m_pVertices[currentBatch + 2].m_Texture = XMFLOAT2(sprite.textureRect.x + spriteWidth, sprite.textureRect.y);
	m_pVertices[currentBatch + 3].m_Texture = XMFLOAT2(sprite.textureRect.x + spriteWidth, sprite.textureRect.y + spriteHeight);

	m_pVertices[currentBatch + 0].m_Color = sprite.color;
	m_pVertices[currentBatch + 1].m_Color = sprite.color;
	m_pVertices[currentBatch + 2].m_Color = sprite.color;
	m_pVertices[currentBatch + 3].m_Color = sprite.color;

	m_CurrBatchSize++;
}

void SpriteBatch::DrawBatchOnCenter(const Sprite &sprite)
{
	float spriteWidth = sprite.textureRect.width - sprite.textureRect.x;
	float spriteHeight = sprite.textureRect.height - sprite.textureRect.y;
	float spriteHalfWidth = spriteWidth * 0.5f;
	float spriteHalfHeight = spriteHeight * 0.5f;

	int currentBatch = m_CurrBatchSize * 4;

	m_pVertices[currentBatch + 0].m_Position = XMFLOAT3(sprite.position.x - spriteHalfWidth, sprite.position.y - spriteHalfHeight + spriteHeight, 0.0f);
	m_pVertices[currentBatch + 1].m_Position = XMFLOAT3(sprite.position.x - spriteHalfWidth, sprite.position.y - spriteHalfHeight, 0.0f);
	m_pVertices[currentBatch + 2].m_Position = XMFLOAT3(sprite.position.x - spriteHalfWidth + spriteWidth, sprite.position.y - spriteHalfHeight, 0.0f);
	m_pVertices[currentBatch + 3].m_Position = XMFLOAT3(sprite.position.x - spriteHalfWidth + spriteWidth, sprite.position.y - spriteHalfHeight + spriteHeight, 0.0f);

	m_pVertices[currentBatch + 0].m_Texture = XMFLOAT2(sprite.textureRect.x, sprite.textureRect.y + spriteHeight);
	m_pVertices[currentBatch + 1].m_Texture = XMFLOAT2(sprite.textureRect.x, sprite.textureRect.y);
	m_pVertices[currentBatch + 2].m_Texture = XMFLOAT2(sprite.textureRect.x + spriteWidth, sprite.textureRect.y);
	m_pVertices[currentBatch + 3].m_Texture = XMFLOAT2(sprite.textureRect.x + spriteWidth, sprite.textureRect.y + spriteHeight);

	m_pVertices[currentBatch + 0].m_Color = sprite.color;
	m_pVertices[currentBatch + 1].m_Color = sprite.color;
	m_pVertices[currentBatch + 2].m_Color = sprite.color;
	m_pVertices[currentBatch + 3].m_Color = sprite.color;

	m_CurrBatchSize++;
}

void SpriteBatch::EndBatchDraw(const Camera * const pCamera)
{
	if (!m_pGraphics)
	{
		return;
	}

	m_pShaderParameters->projMatrix = pCamera->GetOrthoMatrix();

	m_pShader->MapData(m_pGraphics, m_pShaderParameters);

	m_pModel->MapSprites(m_pGraphics, m_pVertices, m_CurrBatchSize);

	m_pModel->Draw(m_pGraphics, m_CurrBatchSize);

	m_DrawBatch = false;
}