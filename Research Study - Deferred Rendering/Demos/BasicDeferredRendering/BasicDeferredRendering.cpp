#include "BasicDeferredRendering.h"
#include "Primitive.h"
#include "BVVector_SIMD.h"
#include "Allocator.h"
#include "Material.h"

using namespace BVMemory;


BasicDeferredRendering::BasicDeferredRendering(const char * const pWindowName, const unsigned int width, const unsigned int height)
	: Engine(pWindowName, width, height)
{
}


BasicDeferredRendering::~BasicDeferredRendering()
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
	delete m_pBrownRocksTexture;
	delete m_pDryRocksTexture;

	delete m_pSamplerState;
}


bool BasicDeferredRendering::StartUp()
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

	m_pBrownRocksTexture = new ShaderResourceView();
	m_pBrownRocksTexture->CreateFromFile("brown_rocks.jpg");

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

	m_GBufferIndex = 1;

	return true;
}


void BasicDeferredRendering::Update()
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

	m_Camera.UpdateViewMatrix();
	m_Camera.UpdateProjMatrix();
}

void BasicDeferredRendering::Render()
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

	m_Material.m_HemsTopColor = BVFloat4(0.2f, 0.4f, 0.3f, 1.0f);
	m_Material.m_HemsBottomColor = BVFloat4(0.3f, 0.0f, 0.0f, 1.0f);
	m_Material.m_Specular = BVFloat4(0.8f, 0.8f, 0.8f, 4.5f);
	m_Material.m_UseTexture = 1;
	m_Material.m_UseColor = 0;
	m_pCBMaterial->Map(&m_Material, sizeof(Material));

	BVMatrix r;
	BVMatrix m;

	m = MatrixTranslation(0.0f, -2.0f, 4.0f);
	MatrixToFloat4x4(m * m_Camera.GetViewM() * m_Camera.GetProjM(), m_CBObject.m_WVP);
	MatrixToFloat4x4(m, m_CBObject.m_World);
	MatrixToFloat4x4(MatrixWorldInvTranspose(m), m_CBObject.m_InvWorld);
	m_pCBObject->Map(&m_CBObject, sizeof(CBObject));

	m_pGraphics->SetVertexBuffers(&m_VBGrid, 1);
	m_pGraphics->SetIndexBuffer(m_IBGrid);
	m_pGraphics->DrawIndexed(m_IBGrid->GetNumIndices());

	m_pGraphics->SetVertexBuffers(&m_VBSphere, 1);
	m_pGraphics->SetIndexBuffer(m_IBSphere);

	m_pGraphics->SetShaderResources(ShaderType::Pixel, &m_pDryRocksTexture, 1);

	m = MatrixIdentity();
	MatrixToFloat4x4(m * m_Camera.GetViewM() * m_Camera.GetProjM(), m_CBObject.m_WVP);
	MatrixToFloat4x4(m, m_CBObject.m_World);
	MatrixToFloat4x4(MatrixWorldInvTranspose(m), m_CBObject.m_InvWorld);
	m_pCBObject->Map(&m_CBObject, sizeof(CBObject));

	m_pGraphics->DrawIndexed(m_IBSphere->GetNumIndices());

	m = MatrixTranslation(3.0f, 0.0f, 0.0f);
	MatrixToFloat4x4(m * m_Camera.GetViewM() * m_Camera.GetProjM(), m_CBObject.m_WVP);
	MatrixToFloat4x4(m, m_CBObject.m_World);
	MatrixToFloat4x4(MatrixWorldInvTranspose(m), m_CBObject.m_InvWorld);
	m_pCBObject->Map(&m_CBObject, sizeof(CBObject));

	m_pGraphics->DrawIndexed(m_IBSphere->GetNumIndices());

	m = MatrixTranslation(-3.0f, 0.0f, 0.0f);
	MatrixToFloat4x4(m * m_Camera.GetViewM() * m_Camera.GetProjM(), m_CBObject.m_WVP);
	MatrixToFloat4x4(m, m_CBObject.m_World);
	MatrixToFloat4x4(MatrixWorldInvTranspose(m), m_CBObject.m_InvWorld);
	m_pCBObject->Map(&m_CBObject, sizeof(CBObject));

	m_pGraphics->DrawIndexed(m_IBSphere->GetNumIndices());

	m_pGraphics->SetShaderResources(ShaderType::Pixel, &m_pBrownRocksTexture, 1);

	m = MatrixTranslation(0.0f, 0.0f, 3.0f);
	MatrixToFloat4x4(m * m_Camera.GetViewM() * m_Camera.GetProjM(), m_CBObject.m_WVP);
	MatrixToFloat4x4(m, m_CBObject.m_World);
	MatrixToFloat4x4(MatrixWorldInvTranspose(m), m_CBObject.m_InvWorld);
	m_pCBObject->Map(&m_CBObject, sizeof(CBObject));

	m_pGraphics->DrawIndexed(m_IBSphere->GetNumIndices());

	m = MatrixTranslation(0.0f, 0.0f, -3.0f);
	MatrixToFloat4x4(m * m_Camera.GetViewM() * m_Camera.GetProjM(), m_CBObject.m_WVP);
	MatrixToFloat4x4(m, m_CBObject.m_World);
	MatrixToFloat4x4(MatrixWorldInvTranspose(m), m_CBObject.m_InvWorld);
	m_pCBObject->Map(&m_CBObject, sizeof(CBObject));

	m_pGraphics->DrawIndexed(m_IBSphere->GetNumIndices());


	m_pDeferredShading->UnsetGBufferPass();

	if (5U > m_GBufferIndex)
	{
		m_pDeferredShading->RenderGBuffer(m_GBufferIndex, m_Camera.GetProj());
	}
	m_pDeferredShading->RenderGBuffers(m_Camera.GetProj());

	// ========================
	// Forward pass
	// ========================

	m_pGraphics->Present();
}