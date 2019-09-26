#include "SpriteBatch.h"
#include "BVMatrix_SIMD.h"
#include "RendererD3D11.h"

using namespace BVMath::SIMD;

namespace BVEngine
{
	SpriteBatch::SpriteBatch(RendererD3D11 * const pGraphics, Allocator * const pAllocator)
		: m_pGraphics(pGraphics), m_Width(0.0f), m_Height(0.0f), m_SpriteCount(0), m_pSprites(nullptr),
		m_pVB(nullptr), m_pIB(nullptr), m_pCBVertexData(nullptr), m_pCBPixelData(nullptr), m_pIA(nullptr),
		m_pVS(nullptr), m_pPS(nullptr), m_pSamplerState(nullptr), m_pBlendState(nullptr), m_pDepthStencilState(nullptr),
		m_pAllocator(pAllocator)
	{
	}


	SpriteBatch::~SpriteBatch()
	{
		Clear();
	}


	bool SpriteBatch::Create(const float screenWidth, const float screenHeight, const unsigned int spriteCount)
	{
		Clear();

		GraphicsResult result = GraphicsResult::SUCCESS;

		m_SpriteCount = spriteCount;

		if (m_pAllocator)
		{
			void *pMem = m_pAllocator->AllocateU(sizeof(SpriteData) * m_SpriteCount);
			m_pSprites = reinterpret_cast<SpriteData *>(pMem);

			m_pVB = new(m_pAllocator->AllocateU(sizeof(VertexBuffer))) VertexBuffer();
			m_pIB = new(m_pAllocator->AllocateU(sizeof(IndexBuffer))) IndexBuffer();
			m_pCBVertexData = new(m_pAllocator->AllocateU(sizeof(ConstantBuffer))) ConstantBuffer();
			m_pCBPixelData = new(m_pAllocator->AllocateU(sizeof(ConstantBuffer))) ConstantBuffer();

			m_pIA = new(m_pAllocator->AllocateU(sizeof(InputLayout))) InputLayout();
			m_pVS = new(m_pAllocator->AllocateU(sizeof(VertexShader))) VertexShader();
			m_pPS = new(m_pAllocator->AllocateU(sizeof(PixelShader))) PixelShader();

			m_pSamplerState = new(m_pAllocator->AllocateU(sizeof(SamplerState))) SamplerState();
			m_pBlendState = new(m_pAllocator->AllocateU(sizeof(BlendState))) BlendState();
			m_pDepthStencilState = new(m_pAllocator->AllocateU(sizeof(DepthStencilState))) DepthStencilState();
		}
		else
		{
			m_pSprites = new SpriteData[m_SpriteCount];

			m_pVB = new VertexBuffer();
			m_pIB = new IndexBuffer();
			m_pCBVertexData = new ConstantBuffer();
			m_pCBPixelData = new ConstantBuffer();

			m_pIA = new InputLayout();
			m_pVS = new VertexShader();
			m_pPS = new PixelShader();

			m_pSamplerState = new SamplerState();
			m_pBlendState = new BlendState();
			m_pDepthStencilState = new DepthStencilState();
		}

		do
		{
			result = m_pVB->CreateDynamic(4 * m_SpriteCount, sizeof(Vertex2D));
			if (GraphicsResult::SUCCESS != result)
			{
				break;
			}

			{
				unsigned int *pIndices = new unsigned int[6 * m_SpriteCount];
				for (size_t i = 0; i < m_SpriteCount; i++)
				{
					// Left handed
					pIndices[i * 6 + 0] = i * 4 + 0;
					pIndices[i * 6 + 1] = i * 4 + 1;
					pIndices[i * 6 + 2] = i * 4 + 2;

					pIndices[i * 6 + 3] = i * 4 + 0;
					pIndices[i * 6 + 4] = i * 4 + 2;
					pIndices[i * 6 + 5] = i * 4 + 3;
				}

				result = m_pIB->Create(pIndices, 6 * m_SpriteCount);
				if (GraphicsResult::SUCCESS != result)
				{
					delete[] pIndices;
					break;
				}
				delete[] pIndices;
			}

			result = m_pCBVertexData->CreateDynamic(sizeof(CBVertexData));
			if (GraphicsResult::SUCCESS != result)
			{
				break;
			}

			result = m_pCBPixelData->CreateDynamic(sizeof(CBPixelData));
			if (GraphicsResult::SUCCESS != result)
			{
				break;
			}

			result = m_pVS->CompileFromFile("Sprite.vs.hlsl", "VS", m_pIA, Vertex2D::m_InputLayout, Vertex2D::m_InputLayoutSize);
			if (GraphicsResult::SUCCESS != result)
			{
				break;
			}

			result = m_pPS->CompileFromFile("Sprite.ps.hlsl", "PS");
			if (GraphicsResult::SUCCESS != result)
			{
				break;
			}

			{
				D3D11_SAMPLER_DESC samplerDesc;
				ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));
				samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
				samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
				samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
				samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
				samplerDesc.MinLOD = 0.0f;
				samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
				samplerDesc.MaxAnisotropy = 1;
				samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
				//samplerDesc.BorderColor[0] = 0.0f;
				//samplerDesc.BorderColor[1] = 0.0f;
				//samplerDesc.BorderColor[2] = 0.0f;
				//samplerDesc.BorderColor[3] = 0.0f;

				result = m_pSamplerState->Create(samplerDesc);
				if (GraphicsResult::SUCCESS != result)
				{
					break;
				}
			}

			{
				D3D11_BLEND_DESC blendDesc;
				ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));
				blendDesc.AlphaToCoverageEnable = false;
				blendDesc.IndependentBlendEnable = false;
				blendDesc.RenderTarget[0].BlendEnable = true;
				blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
				blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
				blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
				blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
				blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
				blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
				blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
				
				result = m_pBlendState->Create(blendDesc);
				if (GraphicsResult::SUCCESS != result)
				{
					break;
				}
			}

			{
				D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
				ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
				depthStencilDesc.DepthEnable = false;
				depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
				depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
				depthStencilDesc.StencilEnable = true;
				depthStencilDesc.StencilReadMask = 0xFF;
				depthStencilDesc.StencilWriteMask = 0xFF;
				depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
				depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
				depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
				depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
				depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
				depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
				depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
				depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

				result = m_pDepthStencilState->Create(depthStencilDesc);
				if (GraphicsResult::SUCCESS != result)
				{
					break;
				}
			}
		} while (0);

		Resize(screenWidth, screenHeight);

		return GraphicsResult::SUCCESS == result;
	}


	void SpriteBatch::Draw(const Sprite & sprite, ShaderResourceView * const pTexture)
	{
		if (pTexture)
		{
			ComPtr<ID3D11Texture2D> pResource;
			pTexture->Get()->GetResource((ID3D11Resource **)pResource.GetAddressOf());
			D3D11_TEXTURE2D_DESC desc;
			pResource->GetDesc(&desc);

			m_CBVertexData.m_TextureInfo.x = (float)desc.Width;
			m_CBVertexData.m_TextureInfo.y = (float)desc.Height;
		}
		else
		{
			m_CBVertexData.m_TextureInfo.x = 1.0f;
			m_CBVertexData.m_TextureInfo.y = 1.0f;
		}

		float spriteWidth = sprite.textureRect.width - sprite.textureRect.x;
		float spriteHeight = sprite.textureRect.height - sprite.textureRect.y;

		m_pSprites[0].m_pVertices[0].m_Position = BVFloat3(sprite.position.x, sprite.position.y + sprite.size.y, 0.0f);
		m_pSprites[0].m_pVertices[1].m_Position = BVFloat3(sprite.position.x, sprite.position.y, 0.0f);
		m_pSprites[0].m_pVertices[2].m_Position = BVFloat3(sprite.position.x + sprite.size.x, sprite.position.y, 0.0f);
		m_pSprites[0].m_pVertices[3].m_Position = BVFloat3(sprite.position.x + sprite.size.x, sprite.position.y + sprite.size.y, 0.0f);

		if (sprite.fullQuad)
		{
			m_pSprites[0].m_pVertices[0].m_Texture = BVFloat2(0.0f, m_CBVertexData.m_TextureInfo.y);
			m_pSprites[0].m_pVertices[1].m_Texture = BVFloat2(0.0f, 0.0f);
			m_pSprites[0].m_pVertices[2].m_Texture = BVFloat2(m_CBVertexData.m_TextureInfo.x, 0.0f);
			m_pSprites[0].m_pVertices[3].m_Texture = BVFloat2(m_CBVertexData.m_TextureInfo.x, m_CBVertexData.m_TextureInfo.y);
		}
		else
		{
			m_pSprites[0].m_pVertices[0].m_Texture = BVFloat2(sprite.textureRect.x, sprite.textureRect.y + spriteHeight);
			m_pSprites[0].m_pVertices[1].m_Texture = BVFloat2(sprite.textureRect.x, sprite.textureRect.y);
			m_pSprites[0].m_pVertices[2].m_Texture = BVFloat2(sprite.textureRect.x + spriteWidth, sprite.textureRect.y);
			m_pSprites[0].m_pVertices[3].m_Texture = BVFloat2(sprite.textureRect.x + spriteWidth, sprite.textureRect.y + spriteHeight);
		}

		m_CBPixelData.m_Color = BVFloat3(sprite.color.x, sprite.color.y, sprite.color.z);
		m_CBPixelData.m_UseTexture = pTexture != nullptr;

		// Draw
		m_pGraphics->SetInputLayout(m_pIA);

		m_pGraphics->SetShader(ShaderType::Vertex, m_pVS);
		m_pGraphics->SetConstantBuffers(ShaderType::Vertex, &m_pCBVertexData, 1);
		m_pCBVertexData->Map(&m_CBVertexData, sizeof(CBVertexData));

		m_pGraphics->SetShader(ShaderType::Pixel, m_pPS);
		m_pGraphics->SetConstantBuffers(ShaderType::Pixel, &m_pCBPixelData, 1);
		if (pTexture)
		{
			m_pGraphics->SetShaderResources(ShaderType::Pixel, &pTexture, 1);
			m_pGraphics->SetSamplers(ShaderType::Pixel, &m_pSamplerState, 1);
		}
		m_pCBPixelData->Map(&m_CBPixelData, sizeof(CBPixelData));

		m_pVB->Map(m_pSprites, sizeof(SpriteData));

		float blendFactor[4] =
		{
			1.0f
		};

		m_pGraphics->SetPrimitiveTopology(Topology::TriangleList);
		m_pGraphics->SetVertexBuffers(&m_pVB, 1);
		m_pGraphics->SetIndexBuffer(m_pIB);

		m_pGraphics->SetDepthStencilState(m_pDepthStencilState);
		m_pGraphics->SetBlendState(m_pBlendState, blendFactor);

		m_pGraphics->DrawIndexed(6);

		m_pGraphics->SetBlendState(nullptr, nullptr);
		m_pGraphics->SetDepthStencilState(nullptr);
	}


	void SpriteBatch::Resize(const float width, const float height)
	{
		m_Width = width;
		m_Height = height;

		MatrixToFloat4x4(MatrixOrthographicOffCenterLH_DX(width, 0.0f, 0.0f, height, 0.0f, 1.0f), m_CBVertexData.m_Ortho);
	}


	void SpriteBatch::Clear()
	{
		if (m_pSprites)
		{
			if (m_pAllocator)
			{
				m_pAllocator->FreeU(m_pDepthStencilState);
				m_pAllocator->FreeU(m_pBlendState);
				m_pAllocator->FreeU(m_pSamplerState);

				m_pAllocator->FreeU(m_pPS);
				m_pAllocator->FreeU(m_pVS);
				m_pAllocator->FreeU(m_pIA);

				m_pAllocator->FreeU(m_pCBPixelData);
				m_pAllocator->FreeU(m_pCBVertexData);
				m_pAllocator->FreeU(m_pIB);
				m_pAllocator->FreeU(m_pVB);
				
				m_pAllocator->FreeU(m_pSprites);
			}
			else
			{
				delete[] m_pSprites;

				delete m_pVB;
				delete m_pIB;
				delete m_pCBVertexData;
				delete m_pCBPixelData;

				delete m_pIA;
				delete m_pVS;
				delete m_pPS;

				delete m_pSamplerState;
				delete m_pBlendState;
				delete m_pDepthStencilState;
			}
		}
	}
}