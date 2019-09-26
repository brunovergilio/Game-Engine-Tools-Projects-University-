#include "InstancedPointLights.h"
#include "Primitive.h"
#include "BVVector_SIMD.h"
#include "Allocator.h"
#include "Material.h"
#include <random>

using namespace BVMemory;


InstancedPointLights::InstancedPointLights(const char * const pWindowName, const unsigned int width, const unsigned int height)
	: Engine(pWindowName, width, height)
{
}


InstancedPointLights::~InstancedPointLights()
{
	BV_SAFE_DELETE(m_pVBSphere);
	BV_SAFE_DELETE(m_pIBSphere);
	BV_SAFE_DELETE(m_pVBGrid);
	BV_SAFE_DELETE(m_pIBGrid);
	BV_SAFE_DELETE(m_pCBObject);
	BV_SAFE_DELETE(m_pCBMaterial);
	BV_SAFE_DELETE(m_pVS);
	BV_SAFE_DELETE(m_pPS);
	BV_SAFE_DELETE(m_pInputLayout);
	BV_SAFE_DELETE(m_pBrownRocksTexture);
	BV_SAFE_DELETE(m_pBlueRocksTexture);
	BV_SAFE_DELETE(m_pSamplerState);
	
	BV_SAFE_DELETE(m_pVSInstanced);
	BV_SAFE_DELETE(m_pVBInstancedData);
	BV_SAFE_DELETE(m_pInputLayoutInstanced);
	BV_SAFE_DELETE(m_pCBObjectInstanced);
}


bool InstancedPointLights::StartUp()
{
	Engine::StartUp();

	BVVector eyePos = VectorSet(-15.0f, 50.0f, -50.0f);
	BVVector eyeDir = VectorNormalize(VectorSet(15.0f, -50.0f, 50.0f));
	BVVector up = VectorSet(0.0f, 1.0f, 0.0f);
	m_Camera.LookAt(eyePos, eyeDir, up);
	m_Camera.UpdateViewMatrix();

	m_Camera.SetPerspective(1.0f, 1000.0f, ((float)m_Width) / ((float)m_Height), PI * 0.25f);

	{
		Primitive prim;
		prim.CreateGeosphere(1.0f, 3);

		m_pVBSphere = new VertexBuffer();
		m_pVBSphere->Create(prim.m_pVertices, prim.m_NumVertices, sizeof(Vertex3D));

		m_pIBSphere = new IndexBuffer();
		m_pIBSphere->Create(prim.m_pIndices, prim.m_NumIndices);

		prim.CreateGrid(5.0f, 5.0f, 12, 12);

		m_pVBGrid = new VertexBuffer();
		m_pVBGrid->Create(prim.m_pVertices, prim.m_NumVertices, sizeof(Vertex3D));

		m_pIBGrid = new IndexBuffer();
		m_pIBGrid->Create(prim.m_pIndices, prim.m_NumIndices);
	}

	m_pCBObject = new ConstantBuffer();
	m_pCBObject->CreateDynamic(sizeof(CBObject));

	m_pCBMaterial = new ConstantBuffer();
	m_pCBMaterial->CreateDynamic(sizeof(Material));

	m_pInputLayout = new InputLayout();

	m_pVS = new VertexShader();
	m_pVS->CompileFromFile("GBuffer.vs.hlsl", "VS", m_pInputLayout, Vertex3D::m_InputLayout, Vertex3D::m_InputLayoutSize);

	m_pPS = new PixelShader();
	m_pPS->CompileFromFile("GBuffer.ps.hlsl", "PS");

	m_pBrownRocksTexture = new ShaderResourceView();
	m_pBrownRocksTexture->CreateFromFile("brown_rocks.jpg");

	m_pBlueRocksTexture = new ShaderResourceView();
	m_pBlueRocksTexture->CreateFromFile("blue_rocks.jpg");

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

	CreateInstancedData();
	CreateLights();

	m_GBufferIndex = 5;

	return true;
}


void InstancedPointLights::Update()
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

	if (GetAsyncKeyState('0') & 0x8000)
	{
		m_GBufferIndex = 0;
	}

	if (GetAsyncKeyState('1') & 0x8000)
	{
		m_GBufferIndex = 1;
	}

	if (GetAsyncKeyState('2') & 0x8000)
	{
		m_GBufferIndex = 2;
	}

	if (GetAsyncKeyState('3') & 0x8000)
	{
		m_GBufferIndex = 3;
	}

	if (GetAsyncKeyState('4') & 0x8000)
	{
		m_GBufferIndex = 4;
	}

	if (GetAsyncKeyState('5') & 0x8000)
	{
		m_GBufferIndex = 5;
	}

	m_Camera.UpdateViewMatrix();
	m_Camera.UpdateProjMatrix();
}

void InstancedPointLights::Render()
{
	// ========================
	// GBuffer pass
	// ========================

	m_pGraphics->ClearDepthStencilView(m_pGraphics->GetMainDepthStencilView(), true, 1.0f, false);
	m_pGraphics->ClearRenderTargetView(m_pGraphics->GetMainRenderTargetView());

	m_pDeferredShading->SetGBufferPass();

	m_pGraphics->SetShader(ShaderType::Vertex, m_pVS);
	m_pGraphics->SetConstantBuffers(ShaderType::Vertex, &m_pCBObject, 1);

	m_pGraphics->SetShader(ShaderType::Pixel, m_pPS);
	m_pGraphics->SetConstantBuffers(ShaderType::Pixel, &m_pCBMaterial, 1);
	m_pGraphics->SetShaderResources(ShaderType::Pixel, &m_pBrownRocksTexture, 1);
	m_pGraphics->SetSamplers(ShaderType::Pixel, &m_pSamplerState, 1);

	m_pGraphics->SetInputLayout(m_pInputLayout);
	m_pGraphics->SetPrimitiveTopology(Topology::TriangleList);

	m_Material.m_HemsTopColor = BVFloat4(0.4f, 0.6f, 0.5f, 1.0f);
	m_Material.m_HemsBottomColor = BVFloat4(0.5f, 0.0f, 0.0f, 1.0f);
	m_Material.m_Specular = BVFloat4(0.7f, 0.7f, 0.7f, 4.5f);
	m_Material.m_UseTexture = 1;
	m_Material.m_UseColor = 0;
	m_pCBMaterial->Map(&m_Material, sizeof(Material));

	static float angle = 0.0f;
	static float step = 0.02f;
	if (angle >= PI)
	{
		step = -step;
	}
	if (angle < 0.0f)
	{
		step = -step;
	}
	angle += step;

	BVMatrix m;

	BVMatrix view = m_Camera.GetViewM();
	BVMatrix proj = m_Camera.GetProjM();

	m = MatrixIdentity();
	MatrixToFloat4x4(m * view * proj, m_CBObject.m_WVP);
	MatrixToFloat4x4(m, m_CBObject.m_World);
	MatrixToFloat4x4(MatrixWorldInvTranspose(m), m_CBObject.m_InvWorld);
	m_pCBObject->Map(&m_CBObject, sizeof(CBObject));

	m_pGraphics->SetVertexBuffers(&m_pVBGrid, 1);
	m_pGraphics->SetIndexBuffer(m_pIBGrid);
	m_pGraphics->DrawIndexed(m_pIBGrid->GetNumIndices());

	m_pGraphics->SetShaderResources(ShaderType::Pixel, &m_pBlueRocksTexture, 1);
	m_Material.m_Specular = BVFloat4(0.7f, 0.7f, 0.7f, 4.5f);
	m_pCBMaterial->Map(&m_Material, sizeof(Material));

	m_pGraphics->SetShader(ShaderType::Vertex, m_pVSInstanced);
	m_pGraphics->SetConstantBuffers(ShaderType::Vertex, &m_pCBObjectInstanced, 1);
	MatrixToFloat4x4(view * proj, m_CBObjectInstanced.m_ViewProj);
	m_pCBObjectInstanced->Map(&m_CBObjectInstanced, sizeof(CBObjectInstanced));

	VertexBuffer * pVBs[] = { m_pVBSphere, m_pVBInstancedData };
	m_pGraphics->SetInputLayout(m_pInputLayoutInstanced);
	m_pGraphics->SetVertexBuffers(pVBs, 2);
	m_pGraphics->SetIndexBuffer(m_pIBSphere);
	m_pGraphics->DrawIndexedInstanced(m_pIBSphere->GetNumIndices(), 100);
	m_pGraphics->SetVertexBuffers(nullptr, 2);

	m_pDeferredShading->UnsetGBufferPass();


	// ========================
	// Light pass
	// ========================
	m_pDeferredShading->SetLightPass(m_Camera.GetViewM(), m_Camera.GetProjM());

	//Light pLights[] = { m_Hems };

	//m_pDeferredShading->RenderLightQuads(pLights, 1);

	for (size_t i = 0; i < 200; i++)
	{
		m_PointLights[i].m_Intensity += m_Steps[i];
		if (m_PointLights[i].m_Intensity > 9.0f)
		{
			m_PointLights[i].m_Intensity = 9.0f;
			m_Steps[i] = -m_Steps[i];
		}
		if (m_PointLights[i].m_Intensity < 3.0f)
		{
			m_PointLights[i].m_Intensity = 3.0f;
			m_Steps[i] = -m_Steps[i];
		}
	}

	m_pDeferredShading->RenderLightVolumesInstanced(m_Camera.GetViewM(), m_Camera.GetProjM(), m_PointLights, 200);
	m_pDeferredShading->UnsetLightPass();

	// ========================
	// Forward pass
	// ========================

	m_pDeferredShading->RenderGBuffers(m_Camera.GetProj());
	if (5U > m_GBufferIndex)
	{
		m_pDeferredShading->RenderGBuffer(m_GBufferIndex, m_Camera.GetProj());
	}

	m_pGraphics->Present();
}

void InstancedPointLights::CreateInstancedData()
{
	struct InstanceData
	{
		BVFloat44 world;
		BVFloat44 inv;
	} instances[100];
	
	m_pVBInstancedData = new VertexBuffer();
	m_pVBInstancedData->CreateDynamic(100, sizeof(InstanceData));

	BVMatrix scale = MatrixScaling(2.0f, 2.0f, 2.0f);
	BVMatrix world;

	float xPos = -22.0f;
	float zPos = 22.5f;
	float x, z, y = 4.0f;
	for (int i = 0; i < 10; i++)
	{
		x = xPos + 5.0f * i;
		for (int j = 0; j < 10; j++)
		{
			z = zPos - 5.0f * j;

			world = scale * MatrixTranslation(x, y, z);
			MatrixToFloat4x4(world, instances[i * 10 + j].world);
			MatrixToFloat4x4(MatrixWorldInvTranspose(world), instances[i * 10 + j].inv);
		}
	}

	m_pVBInstancedData->Map(instances, sizeof(InstanceData) * 100);

	m_pVSInstanced = new VertexShader();
	m_pInputLayoutInstanced = new InputLayout();
	m_pVSInstanced->CompileFromFile("GBufferInstanced.vs.hlsl", "VS",
		m_pInputLayoutInstanced, Vertex3D::m_InputLayoutInstanced, Vertex3D::m_InputLayoutInstancedSize);

	m_pCBObjectInstanced = new ConstantBuffer();
	m_pCBObjectInstanced->CreateDynamic(sizeof(CBObjectInstanced));
}

void InstancedPointLights::CreateLights()
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dist(0.0f, 0.5f);
	std::uniform_real_distribution<float> dist2(3.0f, 9.0f);
	std::uniform_real_distribution<float> dist3(2.1f, 10.0f);

	for (size_t i = 0; i < 200; i++)
	{
		m_PointLights[i].m_LightType = LightType::Point;
		m_PointLights[i].m_Color = BVFloat4(dist(gen), dist(gen), dist(gen), 1.0f);
		m_PointLights[i].m_Intensity = dist2(gen);
		m_PointLights[i].m_Range = dist3(gen);
		m_PointLights[i].m_Attenuation = BVFloat3(dist(gen), dist(gen), dist(gen));
		m_PointLights[i].m_Enabled = true;
		m_Steps[i] = 0.2f;
	}

	float xPos = -20.0f;
	float zPos = 20.5f;
	float x, z, y = 8.0f;
	for (int i = 0; i < 20; i++)
	{
		x = xPos + 2.5f * i;
		for (int j = 0; j < 10; j++)
		{
			z = zPos - 5.0f * j;

			m_PointLights[i * 10 + j].m_Position = BVFloat3(x, y, z);
		}
	}
}
