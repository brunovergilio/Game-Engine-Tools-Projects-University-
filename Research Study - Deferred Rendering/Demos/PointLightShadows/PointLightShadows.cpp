#include "PointLightShadows.h"
#include "Primitive.h"
#include "BVVector_SIMD.h"
#include "Allocator.h"
#include "Material.h"

using namespace BVMemory;


PointLightShadows::PointLightShadows(const char * const pWindowName, const unsigned int width, const unsigned int height)
	: Engine(pWindowName, width, height)
{
}


PointLightShadows::~PointLightShadows()
{
	delete m_VBSphere;
	delete m_IBSphere;

	delete m_VBGrid;
	delete m_IBGrid;

	delete m_VBBox;
	delete m_IBBox;

	delete m_VBCone;
	delete m_IBCone;

	delete m_pCBObject;
	delete m_pCBMaterial;

	delete m_pVS;
	delete m_pVSPos;
	delete m_pPS;
	delete m_pInputLayout;

	delete m_pBluewallTexture;
	delete m_pDryRocksTexture;

	delete m_pSamplerState;

	delete m_pShadowMap;

	delete m_pSamplerClamp;

	delete m_pDepthBiasRS;
}


bool PointLightShadows::StartUp()
{
	Engine::StartUp();

	BVVector eyePos = VectorSet(-6.0f, 20.0f, -23.0f);
	BVVector eyeDir = VectorNormalize(VectorSet(6.0f, -20.0f, 23.0f));
	BVVector up = VectorSet(0.0f, 1.0f, 0.0f);
	m_Camera.LookAt(eyePos, eyeDir, up);
	m_Camera.UpdateViewMatrix();

	m_Camera.SetPerspective(1.0f, 1000.0f, ((float)m_Width) / ((float)m_Height), PI * 0.25f);

	{
		Primitive prim;
		prim.CreateGeosphere(1.0f, 3);

		m_VBSphere = new VertexBuffer();
		m_VBSphere->Create(prim.m_pVertices, prim.m_NumVertices, sizeof(Vertex3D));

		m_IBSphere = new IndexBuffer();
		m_IBSphere->Create(prim.m_pIndices, prim.m_NumIndices);

		prim.CreateGrid(5.0f, 5.0f, 12, 12);

		m_VBGrid = new VertexBuffer();
		m_VBGrid->Create(prim.m_pVertices, prim.m_NumVertices, sizeof(Vertex3D));

		m_IBGrid = new IndexBuffer();
		m_IBGrid->Create(prim.m_pIndices, prim.m_NumIndices);

		prim.CreateBox(2.0f, 2.0f, 2.0f);

		m_VBBox = new VertexBuffer();
		m_VBBox->Create(prim.m_pVertices, prim.m_NumVertices, sizeof(Vertex3D));

		m_IBBox = new IndexBuffer();
		m_IBBox->Create(prim.m_pIndices, prim.m_NumIndices);

		prim.CreateCone(24, 8, 1.0f, 1.0f, false);

		m_VBCone = new VertexBuffer();
		m_VBCone->Create(prim.m_pVertices, prim.m_NumVertices, sizeof(Vertex3D));

		m_IBCone = new IndexBuffer();
		m_IBCone->Create(prim.m_pIndices, prim.m_NumIndices);
	}

	m_pCBObject = new ConstantBuffer();
	m_pCBObject->CreateDynamic(sizeof(CBObject));

	m_pCBMaterial = new ConstantBuffer();
	m_pCBMaterial->CreateDynamic(sizeof(Material));

	m_pInputLayout = new InputLayout();

	m_pVS = new VertexShader();
	m_pVS->CompileFromFile("GBuffer.vs.hlsl", "VS", m_pInputLayout, Vertex3D::m_InputLayout, Vertex3D::m_InputLayoutSize);

	m_pVSPos = new VertexShader();
	m_pVSPos->CompileFromFile("GBuffer.vs.hlsl", "VS_PosOnly", m_pInputLayout, Vertex3D::m_InputLayout, Vertex3D::m_InputLayoutSize);

	m_pPS = new PixelShader();
	m_pPS->CompileFromFile("GBuffer.ps.hlsl", "PS");

	m_pBluewallTexture = new ShaderResourceView();
	m_pBluewallTexture->CreateFromFile("bluewall.jpg");

	m_pDryRocksTexture = new ShaderResourceView();
	m_pDryRocksTexture->CreateFromFile("dry_rocks.jpg");

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

	m_pSamplerState = new SamplerState();
	m_pSamplerState->Create(samplerDesc);

	// ==================
	// Hemispheric Light
	m_Hems.m_LightType = LightType::Hemispheric;
	m_Hems.m_Enabled = true;

	// ==================
	// Spot light
	m_TestPoint.m_LightType = LightType::Point;
	m_TestPoint.m_Attenuation = BVFloat3(0.5f, 0.0f, 0.2f);
	//m_TestPoint.m_Color = BVFloat4(0.5f, 0.0f, 0.8f);
	m_TestPoint.m_Color = BVFloat4(0.9f, 0.9f, 0.9f);
	m_TestPoint.m_Position = BVFloat3(0.0f, 6.0f, 0.0f);
	m_TestPoint.m_Range = 15.0f;
	m_TestPoint.m_Intensity = 15.0f;
	m_TestPoint.m_Enabled = true;
	m_TestPoint.m_CastsShadow = true;
	m_TestPointY = 6.0f;

	// ==================
	// Shadow Data
	m_pShadowMap = new ShadowMap();
	m_pShadowMap->Create(2048, 1, 1, 1);

	D3D11_SAMPLER_DESC samplerClampDesc;
	ZeroMemory(&samplerClampDesc, sizeof(D3D11_SAMPLER_DESC));
	samplerClampDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	samplerClampDesc.AddressU = samplerClampDesc.AddressV = samplerClampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerClampDesc.MaxAnisotropy = 1;
	samplerClampDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
	samplerClampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	m_pSamplerClamp = new SamplerState();
	m_pSamplerClamp->Create(samplerClampDesc);

	D3D11_RASTERIZER_DESC depthBiasDesc = {
		D3D11_FILL_SOLID,
		D3D11_CULL_FRONT,
		FALSE,
		50,
		D3D11_DEFAULT_DEPTH_BIAS_CLAMP,
		5.0f,
		TRUE,
		FALSE,
		FALSE,
		FALSE
	};

	m_pDepthBiasRS = new RasterizerState();
	m_pDepthBiasRS->Create(depthBiasDesc);

	m_MovePointLight = true;

	return true;
}


void PointLightShadows::Update()
{
	static float deltaX = 0.0f, newDeltaX = 0.0f;
	static float deltaY = 0.0f, newDeltaY = 0.0f;

	static POINT p;
	GetCursorPos(&p);
	ScreenToClient(m_pWindow->GetHandle(), &p);
	newDeltaX = (float)p.x * 2.0f - 1.0f;
	newDeltaY = (float)p.y * -2.0f + 1.0f;

	if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
	{
		float dx = AngleToRadians((newDeltaX - deltaX) * 0.25f);
		m_Camera.RotateY(dx);

		float dy = AngleToRadians((newDeltaY - deltaY) * 0.25f);
		m_Camera.Pitch(-dy);
	}

	deltaX = newDeltaX;
	deltaY = newDeltaY;

	if (GetAsyncKeyState('W') & 0x8000)
	{
		m_Camera.Walk(m_pTimer->GetDelta() * 9.0f);
	}
	if (GetAsyncKeyState('S') & 0x8000)
	{
		m_Camera.Walk(m_pTimer->GetDelta() * -9.0f);
	}

	if (GetAsyncKeyState('A') & 0x8000)
	{
		m_Camera.Strafe(m_pTimer->GetDelta() * -9.0f);
	}
	if (GetAsyncKeyState('D') & 0x8000)
	{
		m_Camera.Strafe(m_pTimer->GetDelta() * 9.0f);
	}

	if (GetAsyncKeyState('N') & 0x8000)
	{
		m_MovePointLight = true;
	}
	if (GetAsyncKeyState('M') & 0x8000)
	{
		m_MovePointLight = false;
	}

	if (GetAsyncKeyState('R') & 0x8000)
	{
		if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
		{
			m_TestPoint.m_Range += 0.02f;
		}
		else
		{
			m_TestPoint.m_Range -= 0.02f;
		}
	}

	if (GetAsyncKeyState('I') & 0x8000)
	{
		if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
		{
			m_TestPoint.m_Intensity += 0.1f;
		}
		else
		{
			m_TestPoint.m_Intensity -= 0.1f;
		}
	}

	if (GetAsyncKeyState('K') & 0x8000)
	{
		if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
		{
			m_TestPoint.m_Attenuation.x += 0.02f;
		}
		else
		{
			m_TestPoint.m_Attenuation.x -= 0.02f;
			if (m_TestPoint.m_Attenuation.x <= 0.0f)
			{
				m_TestPoint.m_Attenuation.x = 0.01f;
			}
		}
	}

	if (GetAsyncKeyState('L') & 0x8000)
	{
		if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
		{
			m_TestPoint.m_Attenuation.y += 0.02f;
		}
		else
		{
			m_TestPoint.m_Attenuation.y -= 0.02f;
			if (m_TestPoint.m_Attenuation.y <= 0.0f)
			{
				m_TestPoint.m_Attenuation.y = 0.0f;
			}
		}
	}

	if (GetAsyncKeyState(VK_OEM_1) & 0x8000)
	{
		if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
		{
			m_TestPoint.m_Attenuation.z += 0.02f;
		}
		else
		{
			m_TestPoint.m_Attenuation.z -= 0.02f;
			if (m_TestPoint.m_Attenuation.z <= 0.0f)
			{
				m_TestPoint.m_Attenuation.z = 0.0f;
			}
		}
	}

	if (GetAsyncKeyState('C') & 0x8000)
	{
		m_TestPoint.m_CastsShadow = true;
	}

	if (GetAsyncKeyState('V') & 0x8000)
	{
		m_TestPoint.m_CastsShadow = false;
	}

	m_Camera.UpdateViewMatrix();
	m_Camera.UpdateProjMatrix();
}

void PointLightShadows::Render()
{
	static float step = 0.1f;
	if (m_MovePointLight)
	{
		m_TestPointY += step;
		if (m_TestPointY > 12.0f || m_TestPointY < 1.0f)
		{
			step *= -1.0f;
		}
		m_TestPoint.m_Position.y = m_TestPointY;
	}

	BVMatrix mPointLight = MatrixScaling(0.4f, 0.4f, 0.4f) *
		MatrixTranslation(m_TestPoint.m_Position.x, m_TestPoint.m_Position.y, m_TestPoint.m_Position.z);
	BVMatrix mGrid = MatrixTranslation(0.0f, -1.0f, 4.0f);
	BVMatrix mSphere[4] = { MatrixTranslation(0.0f, 3.0f, 4.0f), MatrixTranslation(4.0f, 3.0f, 0.0f),
		MatrixTranslation(0.0f, 3.0f, -4.0f), MatrixTranslation(-4.0f, 3.0f, 0.0f) };
	BVMatrix mBox[4] = { MatrixTranslation(0.0f, 3.0f, 8.0f), MatrixTranslation(8.0f, 3.0f, 0.0f),
		MatrixTranslation(0.0f, 3.0f, -8.0f), MatrixTranslation(-8.0f, 3.0f, 0.0f) };

	m_pGraphics->SetInputLayout(m_pInputLayout);
	m_pGraphics->SetPrimitiveTopology(Topology::TriangleList);

	// ========================
	// Shadow pass
	// ========================

	m_pShadowMap->SetPointShadowPass(0, m_TestPoint);

	m_pGraphics->SetRasterizerState(m_pDepthBiasRS);

	m_pShadowMap->SetPointShadowParams(mGrid);

	m_pGraphics->SetVertexBuffers(&m_VBGrid, 1);
	m_pGraphics->SetIndexBuffer(m_IBGrid);
	m_pGraphics->DrawIndexed(m_IBGrid->GetNumIndices());

	m_pGraphics->SetVertexBuffers(&m_VBSphere, 1);
	m_pGraphics->SetIndexBuffer(m_IBSphere);

	for (unsigned int i = 0; i < 4; i++)
	{
		m_pShadowMap->SetPointShadowParams(mSphere[i]);

		m_pGraphics->DrawIndexed(m_IBSphere->GetNumIndices());
	}

	m_pGraphics->SetVertexBuffers(&m_VBBox, 1);
	m_pGraphics->SetIndexBuffer(m_IBBox);

	for (unsigned int i = 0; i < 4; i++)
	{
		m_pShadowMap->SetPointShadowParams(mBox[i]);

		m_pGraphics->DrawIndexed(m_IBBox->GetNumIndices());
	}

	m_pGraphics->SetRasterizerState(nullptr);

	m_pShadowMap->UnsetPointShadowPass();

	// ========================
	// GBuffer pass
	// ========================

	BVMatrix view = m_Camera.GetViewM();
	BVMatrix proj = m_Camera.GetProjM();

	m_pGraphics->ClearDepthStencilView(m_pGraphics->GetMainDepthStencilView(), true, 1.0f, false);
	m_pGraphics->ClearRenderTargetView(m_pGraphics->GetMainRenderTargetView());

	m_pDeferredShading->SetGBufferPass();

	m_pGraphics->SetShader(ShaderType::Vertex, m_pVS);
	m_pGraphics->SetConstantBuffers(ShaderType::Vertex, &m_pCBObject, 1);

	m_pGraphics->SetShader(ShaderType::Pixel, m_pPS);
	m_pGraphics->SetConstantBuffers(ShaderType::Pixel, &m_pCBMaterial, 1);
	m_pGraphics->SetSamplers(ShaderType::Pixel, &m_pSamplerState, 1);

	m_Material.m_HemsTopColor = BVFloat4(0.3f, 0.3f, 0.3f, 1.0f);
	m_Material.m_HemsBottomColor = BVFloat4(0.3f, 0.3f, 0.3f, 1.0f);
	m_Material.m_Specular = BVFloat4(0.5f, 0.5f, 0.5f, 4.5f);
	m_Material.m_UseTexture = 1;
	m_Material.m_UseColor = 0;
	m_pCBMaterial->Map(&m_Material, sizeof(Material));

	m_pGraphics->SetShaderResources(ShaderType::Pixel, &m_pDryRocksTexture, 1);

	MatrixToFloat4x4(mGrid * view * proj, m_CBObject.m_WVP);
	MatrixToFloat4x4(mGrid, m_CBObject.m_World);
	MatrixToFloat4x4(MatrixWorldInvTranspose(mGrid), m_CBObject.m_InvWorld);
	m_pCBObject->Map(&m_CBObject, sizeof(CBObject));

	m_pGraphics->SetVertexBuffers(&m_VBGrid, 1);
	m_pGraphics->SetIndexBuffer(m_IBGrid);
	m_pGraphics->DrawIndexed(m_IBGrid->GetNumIndices());

	m_Material.m_Specular = BVFloat4(0.8f, 0.8f, 0.8f, 4.5f);
	m_pCBMaterial->Map(&m_Material, sizeof(Material));

	m_pGraphics->SetShaderResources(ShaderType::Pixel, &m_pBluewallTexture, 1);

	m_pGraphics->SetVertexBuffers(&m_VBSphere, 1);
	m_pGraphics->SetIndexBuffer(m_IBSphere);

	for (unsigned int i = 0; i < 4; i++)
	{
		MatrixToFloat4x4(mSphere[i] * view * proj, m_CBObject.m_WVP);
		MatrixToFloat4x4(mSphere[i], m_CBObject.m_World);
		MatrixToFloat4x4(MatrixWorldInvTranspose(mSphere[i]), m_CBObject.m_InvWorld);
		m_pCBObject->Map(&m_CBObject, sizeof(CBObject));

		m_pGraphics->DrawIndexed(m_IBSphere->GetNumIndices());
	}

	m_pGraphics->SetVertexBuffers(&m_VBBox, 1);
	m_pGraphics->SetIndexBuffer(m_IBBox);

	for (unsigned int i = 0; i < 4; i++)
	{
		MatrixToFloat4x4(mBox[i] * view * proj, m_CBObject.m_WVP);
		MatrixToFloat4x4(mBox[i], m_CBObject.m_World);
		MatrixToFloat4x4(MatrixWorldInvTranspose(mBox[i]), m_CBObject.m_InvWorld);
		m_pCBObject->Map(&m_CBObject, sizeof(CBObject));

		m_pGraphics->DrawIndexed(m_IBBox->GetNumIndices());
	}

	// Draw Light as a sphere
	m_Material.m_UseColor = 1;
	m_Material.m_UseTexture = 0;
	m_Material.m_Diffuse = BVFloat4(1.0f, 1.0f, 1.0f, 1.0f);
	m_Material.m_HemsTopColor = BVFloat4(1.0f, 1.0f, 1.0f, 1.0f);
	m_Material.m_HemsBottomColor = BVFloat4(1.0f, 1.0f, 1.0f, 1.0f);
	m_pCBMaterial->Map(&m_Material, sizeof(Material));
	MatrixToFloat4x4(mPointLight * view * proj, m_CBObject.m_WVP);
	MatrixToFloat4x4(mPointLight, m_CBObject.m_World);
	MatrixToFloat4x4(MatrixWorldInvTranspose(mPointLight), m_CBObject.m_InvWorld);
	m_pCBObject->Map(&m_CBObject, sizeof(CBObject));
	m_pGraphics->SetVertexBuffers(&m_VBSphere, 1);
	m_pGraphics->SetIndexBuffer(m_IBSphere);
	m_pGraphics->DrawIndexed(m_IBSphere->GetNumIndices());

	m_pDeferredShading->UnsetGBufferPass();


	// ========================
	// Light pass
	// ========================

	Light pLights[] = { m_Hems, m_TestPoint };

	unsigned int pIndices[] = { 0, 0 };
	m_pDeferredShading->RenderLights(m_Camera, pLights, 2, m_pShadowMap, pIndices);


	// ========================
	// Forward pass
	// ========================

	//m_pDeferredShading->RenderGBuffers(m_Camera.GetProj());

	//m_pShadowMap->RenderPointShadowMap(0);

	m_pGraphics->Present();
}