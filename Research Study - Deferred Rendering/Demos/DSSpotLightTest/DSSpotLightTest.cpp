#include "DSSpotLightTest.h"
#include "Primitive.h"
#include "BVVector_SIMD.h"
#include "Allocator.h"
#include "Material.h"

using namespace BVMemory;


DSSpotLightTest::DSSpotLightTest(const char * const pWindowName, const unsigned int width, const unsigned int height)
	: Engine(pWindowName, width, height)
{
}


DSSpotLightTest::~DSSpotLightTest()
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

	delete m_VShader;
	delete m_PShader;
	delete m_pInputLayout;

	delete m_pBluewallTexture;

	delete m_pSamplerState;
}


bool DSSpotLightTest::StartUp()
{
	Engine::StartUp();

	BVVector eyePos = VectorSet(-6.0f, 25.0f, -15.0f);
	BVVector eyeDir = VectorNormalize(VectorSet(6.0f, -25.0f, 15.0f));
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

	m_VShader = new VertexShader();
	m_VShader->CompileFromFile("GBuffer.vs.hlsl", "VS", m_pInputLayout, Vertex3D::m_InputLayout, Vertex3D::m_InputLayoutSize);

	m_PShader = new PixelShader();
	m_PShader->CompileFromFile("GBuffer.ps.hlsl", "PS");

	m_pBluewallTexture = new ShaderResourceView();
	m_pBluewallTexture->CreateFromFile("bluewall.jpg");

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
	m_TestSpot.m_LightType = LightType::Spot;
	VectorToFloat3(VectorSet(0.0f, -1.0f, 0.0f), m_TestSpot.m_Direction);
	m_TestSpot.m_Attenuation = BVFloat3(0.5f, 0.0f, 0.2f);
	m_TestSpot.m_Color = BVFloat4(0.5f, 0.0f, 0.8f);
	m_TestSpot.m_Position = BVFloat3(0.0f, 0.0f, 0.0f);
	m_TestSpot.m_Range = 10.0f;
	m_TestSpot.m_Intensity = 15.0f;
	m_TestSpot.m_CosInnerCone = 0.8f;
	m_TestSpot.m_CosOuterCone = 0.7f;
	m_TestSpot.m_Enabled = true;

	m_SpotDist = 8.0f;

	m_GBufferIndex = 5;

	return true;
}


void DSSpotLightTest::Update()
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

	if (GetAsyncKeyState('F') & 0x8000)
	{
		if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
		{
			m_SpotDist += 0.02f;
		}
		else
		{
			m_SpotDist -= 0.02f;
		}
	}

	if (GetAsyncKeyState('R') & 0x8000)
	{
		if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
		{
			m_TestSpot.m_Range += 0.02f;
		}
		else
		{
			m_TestSpot.m_Range -= 0.02f;
		}
	}

	if (GetAsyncKeyState('I') & 0x8000)
	{
		if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
		{
			m_TestSpot.m_Intensity += 0.1f;
		}
		else
		{
			m_TestSpot.m_Intensity -= 0.1f;
		}
	}

	if (GetAsyncKeyState('K') & 0x8000)
	{
		if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
		{
			m_TestSpot.m_Attenuation.x += 0.02f;
		}
		else
		{
			m_TestSpot.m_Attenuation.x -= 0.02f;
			if (m_TestSpot.m_Attenuation.x <= 0.0f)
			{
				m_TestSpot.m_Attenuation.x = 0.01f;
			}
		}
	}

	if (GetAsyncKeyState('L') & 0x8000)
	{
		if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
		{
			m_TestSpot.m_Attenuation.y += 0.02f;
		}
		else
		{
			m_TestSpot.m_Attenuation.y -= 0.02f;
			if (m_TestSpot.m_Attenuation.y <= 0.0f)
			{
				m_TestSpot.m_Attenuation.y = 0.0f;
			}
		}
	}

	if (GetAsyncKeyState(VK_OEM_1) & 0x8000)
	{
		if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
		{
			m_TestSpot.m_Attenuation.z += 0.02f;
		}
		else
		{
			m_TestSpot.m_Attenuation.z -= 0.02f;
			if (m_TestSpot.m_Attenuation.z <= 0.0f)
			{
				m_TestSpot.m_Attenuation.z = 0.0f;
			}
		}
	}

	if (GetAsyncKeyState('C') & 0x8000)
	{
		if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
		{
			m_TestSpot.m_CosInnerCone -= 0.01f;
		}
		else
		{
			m_TestSpot.m_CosInnerCone += 0.01f;
		}

		if (m_TestSpot.m_CosInnerCone > 1.0f)
		{
			m_TestSpot.m_CosInnerCone = 1.0f;
		}
		if (m_TestSpot.m_CosInnerCone < m_TestSpot.m_CosOuterCone)
		{
			m_TestSpot.m_CosInnerCone = m_TestSpot.m_CosOuterCone;
		}
	}

	if (GetAsyncKeyState('V') & 0x8000)
	{
		if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
		{
			m_TestSpot.m_CosOuterCone -= 0.01f;
		}
		else
		{
			m_TestSpot.m_CosOuterCone += 0.01f;
		}

		if (m_TestSpot.m_CosOuterCone > m_TestSpot.m_CosInnerCone)
		{
			m_TestSpot.m_CosOuterCone = m_TestSpot.m_CosInnerCone;
		}
		if (m_TestSpot.m_CosOuterCone < -1.0f)
		{
			m_TestSpot.m_CosOuterCone = -1.0f;
		}
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

void DSSpotLightTest::Render()
{
	// ========================
	// GBuffer pass
	// ========================

	m_pGraphics->ClearDepthStencilView(m_pGraphics->GetMainDepthStencilView(), true, 1.0f, false);
	m_pGraphics->ClearRenderTargetView(m_pGraphics->GetMainRenderTargetView());

	m_pDeferredShading->SetGBufferPass();

	m_pGraphics->SetShader(ShaderType::Vertex, m_VShader);
	m_pGraphics->SetConstantBuffers(ShaderType::Vertex, &m_pCBObject, 1);

	m_pGraphics->SetShader(ShaderType::Pixel, m_PShader);
	m_pGraphics->SetConstantBuffers(ShaderType::Pixel, &m_pCBMaterial, 1);
	m_pGraphics->SetShaderResources(ShaderType::Pixel, &m_pBluewallTexture, 1);
	m_pGraphics->SetSamplers(ShaderType::Pixel, &m_pSamplerState, 1);

	m_pGraphics->SetInputLayout(m_pInputLayout);
	m_pGraphics->SetPrimitiveTopology(Topology::TriangleList);

	m_Material.m_HemsTopColor = BVFloat4(0.4f, 0.6f, 0.5f, 1.0f);
	m_Material.m_HemsBottomColor = BVFloat4(0.5f, 0.0f, 0.0f, 1.0f);
	m_Material.m_Specular = BVFloat4(0.5f, 0.5f, 0.5f, 4.5f);
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

	BVMatrix r;
	BVMatrix m;
	float radius;

	BVMatrix view = m_Camera.GetViewM();
	BVMatrix proj = m_Camera.GetProjM();

	m = MatrixTranslation(0.0f, -1.0f, 4.0f);
	MatrixToFloat4x4(m * view * proj, m_CBObject.m_WVP);
	MatrixToFloat4x4(m, m_CBObject.m_World);
	MatrixToFloat4x4(MatrixWorldInvTranspose(m), m_CBObject.m_InvWorld);
	m_pCBObject->Map(&m_CBObject, sizeof(CBObject));

	m_pGraphics->SetVertexBuffers(&m_VBGrid, 1);
	m_pGraphics->SetIndexBuffer(m_IBGrid);
	m_pGraphics->DrawIndexed(m_IBGrid->GetNumIndices());

	m_Material.m_Specular = BVFloat4(0.8f, 0.8f, 0.8f, 4.5f);
	m_pCBMaterial->Map(&m_Material, sizeof(Material));

	m = MatrixIdentity();
	MatrixToFloat4x4(m * view * proj, m_CBObject.m_WVP);
	MatrixToFloat4x4(m, m_CBObject.m_World);
	MatrixToFloat4x4(MatrixWorldInvTranspose(m), m_CBObject.m_InvWorld);
	m_pCBObject->Map(&m_CBObject, sizeof(CBObject));

	m_pGraphics->SetVertexBuffers(&m_VBSphere, 1);
	m_pGraphics->SetIndexBuffer(m_IBSphere);
	m_pGraphics->DrawIndexed(m_IBSphere->GetNumIndices());

	radius = tanf(m_TestSpot.m_CosOuterCone) * m_TestSpot.m_Range;
	BVMatrix rotZ = MatrixRotationZ(angle);

	BVVector vPos = VectorSet(m_SpotDist, 0.0f, 0.0f) * rotZ;

	BVVector vConeOrient = VectorSet(0.0f, -1.0f, 0.0f);
	BVVector vNewOrient = VectorNormalize(VectorSet(0.0f, -1.0f, 0.0f) - vPos);

	if (!VectorIsEqual(vConeOrient, vNewOrient, 0.01f))
	{
		BVVector vCross = VectorNormalize(VectorCross(vConeOrient, vNewOrient));
		r = MatrixRotationAxis(vCross, acosf(VectorDot(vConeOrient, vNewOrient)));
	}
	else
	{
		r = MatrixIdentity();
	}
	m.r[3] = vPos;
	m.r[3].m128_f32[3] = 1.0f;
	m = r * m;

	m_Material.m_UseColor = 1;
	m_Material.m_UseTexture = 0;
	m_Material.m_Diffuse = BVFloat4(0.8f, 0.0f, 1.0f, 1.0f);
	m_Material.m_HemsTopColor = BVFloat4(1.0f, 1.0f, 1.0f, 1.0f);
	m_Material.m_HemsBottomColor = BVFloat4(1.0f, 1.0f, 1.0f, 1.0f);
	m_pCBMaterial->Map(&m_Material, sizeof(Material));

	MatrixToFloat4x4(m * view * proj, m_CBObject.m_WVP);
	MatrixToFloat4x4(m, m_CBObject.m_World);
	MatrixToFloat4x4(MatrixWorldInvTranspose(m), m_CBObject.m_InvWorld);
	m_pCBObject->Map(&m_CBObject, sizeof(CBObject));

	m_pGraphics->SetVertexBuffers(&m_VBCone, 1);
	m_pGraphics->SetIndexBuffer(m_IBCone);
	m_pGraphics->DrawIndexed(m_IBCone->GetNumIndices());

	m_pDeferredShading->UnsetGBufferPass();


	// ========================
	// Light pass
	// ========================

	VectorToFloat3(vPos, m_TestSpot.m_Position);

	Light pLights[] = { m_Hems, m_TestSpot };
	m_pDeferredShading->RenderLights(m_Camera, pLights, 2);

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